/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file shaderCompile.cxx
 * @author brian
 * @date 2020-12-22
 */

#include "shaderCompile.h"
#include "shaderObject.h"
#include "virtualFileSystem.h"
#include "config_putil.h"
#include "string_utils.h"
#include "internalName.h"
#include "thread.h"
#include "atomicAdjust.h"
#include "bamFile.h"
#include "bamWriter.h"
#include "shaderCompilerGlslang.h"
#include "load_prc_file.h"
#include "threadManager.h"
#include "config_shader.h"

static ShaderCompile *g_prog = nullptr;

/**
 *
 */
ShaderCompile::
ShaderCompile() :
  ProgramBase("shadercompile"),
  WithOutputFile(true, false, true) {
  _stage = ShaderModule::Stage::vertex;
  _preferred_extension = ".sho";
  _num_threads = 1;
  _sho = nullptr;
  _verbose = false;
  _num_skipped = 0;
  _curr_options = nullptr;

  set_program_brief("compiles shader source files into Panda shader object (.sho) files");
  set_program_description(
    "This program compiles raw shader source files (GLSL/HLSL/Cg) into Panda shader "
    "object files, or .sho files.  The shader object contains multiple compiled "
    "SPIR-V variations of a single shader source file, one for each unique "
    "combo/preprocessor combination.\n"
    "\n"
    "The possible combo/preprocessor variations are specified in the shader source "
    "file, using a syntax like this:\n\n"
    "#pragma combo HAS_LIGHTING 0 1\n\n"
    "This declares a combo named HAS_LIGHTING with a minimum value of 0 and a "
    "maximum value of 1.");

  clear_runlines();
  add_runline("[opts] input.glsl output.sho");
  add_runline("[opts] -o output.sho input.glsl");

  add_option
    ("o", "filename", 0,
     "Specify the filename to which the resulting .sho file will be written.  "
     "If this option is omitted, the last parameter name is taken to be the "
     "name of the output file.",
     &ShaderCompile::dispatch_filename, &_got_output_filename, &_output_filename);

  add_option
    ("s", "stage", 0,
     "Specify the shader stage of this particular source file.",
     &ShaderCompile::dispatch_stage, nullptr, &_stage);

  add_option
    ("j", "count", 0,
     "Specify the number of worker threads that should be used to compile the "
     "variations in parallel.",
     &ShaderCompile::dispatch_int, nullptr, &_num_threads);

  add_option
    ("v", "", 0,
     "Enable verbose output.",
     &ShaderCompile::dispatch_true, nullptr, &_verbose);
}

/**
 *
 */
bool ShaderCompile::
run() {

  if (_verbose) {
    shadermgr_cat->set_severity(NS_debug);
  }

  Shader::ShaderLanguage lang = Shader::SL_none;
  if (_input_filename.get_extension() == "glsl") {
    lang = Shader::SL_GLSL;
  } else if (_input_filename.get_extension() == "hlsl") {
    lang = Shader::SL_HLSL;
  } else {
    nout << "Error: unsupported shader language " << _input_filename.get_extension() << "\n";
    return false;
  }

  _sho = ShaderObject::read_source(lang, _stage, _input_filename);
  if (_sho == nullptr) {
    nout << "Error: failed to read shader object!\n";
    return false;
  }

  nout << "Compiling a " << _stage << " shader\n";

  // Create all of our permutations up front, makes threading simpler.
  size_t num_variations = _sho->get_total_combos();
  _variations.resize(num_variations);

  size_t n = _sho->get_num_combos();
  if (n > 0 && num_variations > 1) {
    // There are combos for this shader, so compile each possible variation of
    // combo values.

    int *indices = new int[n];
    memset(indices, 0, sizeof(int) * n);

    int index = 0;

    // Build the combo values for each variation we will be compiling.
    while (true) {
      VariationData &vdata = _variations[index];
      vdata.skip = false;
      vdata.builder.reset(_sho);

      for (size_t i = 0; i < n; i++) {
        const ShaderObject::Combo *combo = &(_sho->get_combo(i));
        vdata.builder.set_combo_value(i, combo->min_val + indices[i]);
      }

      // Evaluate the skip commands to see if we should skip this variation.
      for (size_t i = 0; i < _sho->get_num_skip_commands(); i++) {
        if (_sho->get_skip_command(i)->eval(vdata.builder) != 0) {
          // The expression evaluated to true for this variation.  Skip it.
          vdata.skip = true;
          if (_verbose) {
            nout << "Skipping variation " << index << " with defines:\n";
            for (size_t i = 0; i < _sho->get_num_combos(); i++) {
              const ShaderObject::Combo &combo = _sho->get_combo(i);
              nout << "\t" << combo.name->get_name()  << "\t" << vdata.builder._combo_values[i] << "\n";
            }
          }
          _num_skipped++;
          break;
        }
      }

      int next = n - 1;
      while ((next >= 0) &&
            ((indices[next] + 1) >= (_sho->get_combo(next).max_val - _sho->get_combo(next).min_val) + 1)) {
        next--;
      }

      if (next < 0) {
        break;
      }

      indices[next]++;

      for (int i = next + 1; i < n; i++) {
        indices[i] = 0;
      }

      index++;
    }

    //
    // Now for the real work.
    //

    ThreadManager::_num_threads = _num_threads;

    _non_skipped_variations.reserve(_variations.size());
    for (size_t i = 0; i < _variations.size(); ++i) {
      if (!_variations[i].skip) {
        _non_skipped_variations.push_back((int)i);
      }
    }
    size_t num_real_variations = _non_skipped_variations.size();

    nout << "Compiling " << num_real_variations << " combo variations for "
        << _input_filename.get_basename() << "\n";
    nout << "(" << _num_skipped << " skipped)\n";
    nout << ThreadManager::_num_threads << " threads\n";

    // Compile the first variation on the main thread to initialize glslang
    // and others without race conditions.
    compile_variation(num_real_variations - 1);
    ThreadManager::run_threads_on_individual("CompileVariations", (int)num_real_variations - 1, false,
                                            std::bind(&ShaderCompile::compile_variation, this, std::placeholders::_1));

  } else {
    // In this case we have no combos, and only one possible variation for this shader.
    // Compile the single variation.

    _variations[0].skip = false;
    _variations[0].builder.reset(_sho);
    _non_skipped_variations.push_back(0);
    compile_variation(0);
  }

  // Now write the .sho file.
  BamFile bam;
  if (!bam.open_write(_output_filename)) {
    nout << "Error: couldn't open " << _output_filename.get_fullpath() << " for writing.\n";
    return false;
  }
  if (!bam.write_object(_sho)) {
    nout << "Error: couldn't write the shader object\n";
    return false;
  }
  bam.close();

  return true;
}

/**
 * Compiles a single variation of the shader.
 */
void ShaderCompile::
compile_variation(int n) {
  assert(n >= 0 && n < (int)_non_skipped_variations.size());
  int v_index = _non_skipped_variations[n];
  assert(v_index >= 0 && v_index < (int)_variations.size());
  const VariationData &vdata = _variations[v_index];

  // It shouldn't be a skipped variation.
  assert(!vdata.skip);
  // Sanity check our index.
  assert(vdata.builder.get_module_index() == v_index);

  if (_verbose) {
    nout << "Compiling variation " << v_index << " with defines:\n";
    for (size_t i = 0; i < _sho->get_num_combos(); i++) {
      const ShaderObject::Combo &combo = _sho->get_combo(i);
      nout << "\t" << combo.name->get_name() << "\t" << vdata.builder._combo_values[i] << "\n";
    }
  }

  // Call get_module() to compile the variation and store it on the
  // ShaderObject.
  ShaderModule *mod = vdata.builder.get_module(true);
  if (mod == nullptr) {
    nout << "Failed to compile variation " << n << "!\n";
    exit(1);
  }
}

/**
 *
 */
bool ShaderCompile::
handle_args(Args &args) {
  if (args.empty()) {
    nout << "Must specify the source file on command line.\n";
    return false;
  }

  _input_filename = Filename::from_os_specific(args[0]);
  args.pop_front();

  if (!check_last_arg(args, 0)) {
    return false;
  }

  return ProgramBase::handle_args(args);
}

/**
 *
 */
bool ShaderCompile::
dispatch_stage(const std::string &opt, const std::string &arg, void *var) {
  if (arg == "vertex" || arg == "vert" || arg == "v") {
    *((ShaderModule::Stage *)var) = ShaderModule::Stage::vertex;
  } else if (arg == "fragment" || arg == "frag" || arg == "f" || arg == "pixel" || arg == "p") {
    *((ShaderModule::Stage *)var) = ShaderModule::Stage::fragment;
  } else if (arg == "geometry" || arg == "geom" || arg == "g") {
    *((ShaderModule::Stage *)var) = ShaderModule::Stage::geometry;
  } else if (arg == "tessellation_control" || arg == "tess_control" || arg == "tc") {
    *((ShaderModule::Stage *)var) = ShaderModule::Stage::tess_control;
  } else if (arg == "tessellation_evaluation" || arg == "tess_eval" || arg == "te") {
    *((ShaderModule::Stage *)var) = ShaderModule::Stage::tess_evaluation;
  } else {
    nout << "Invalid stage argument: " << arg << "\n";
    return false;
  }

  return true;
}

/**
 * Entry point.
 */
int
main(int argc, char *argv[]) {
  load_prc_file_data("shadercompile", "model-path .");

  ShaderCompile prog;
  g_prog = &prog;
  prog.parse_command_line(argc, argv);
  return prog.run() ? 0 : 1;
}

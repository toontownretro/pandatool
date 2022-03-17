/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file shaderCompile.cxx
 * @author lachbr
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

#define VARIABLE_PREFIX '$'
#define VARIABLE_OPEN_BRACE '['
#define VARIABLE_CLOSE_BRACE ']'
#define FUNCTION_PARAMETER_SEPARATOR ','

static AtomicAdjust::Integer progress = 0;
static ShaderCompilerGlslang compiler;
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
  _sho = new ShaderObject;
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
  // First, find and open the source file.
  VirtualFileSystem *vfs = VirtualFileSystem::get_global_ptr();
  PT(VirtualFile) vf = vfs->find_file(_input_filename, get_model_path());
  if (vf == nullptr) {
    nout << "Error: Could not find shader file: " << _input_filename << "\n";
    return false;
  }
  _source_vf = vf;

  _shader_source = vf->read_file(true);

  nout << "Compiling a " << _stage << " shader\n";

  //if (_verbose) {
  //  vector_uchar code;
  //  VirtualFile::simple_read_file(_source_stream, code);
  //  for (size_t i = 0; i < code.size(); i++) {
  //    nout << code[i];
  //  }
  //}

  // Now collect all the combos specified in the source.
  collect_combos();

  size_t num_variations = _sho->get_total_combos();

  // Create all of our permutations up front, makes threading simpler.
  _sho->resize_permutations(num_variations);
  _variations.resize(num_variations);

  size_t n = _sho->get_num_combos();

  int *indices = new int[n];
  memset(indices, 0, sizeof(int) * n);

  int index = 0;

  // Build the combo values for each variation we will be compiling.
  while (true) {
    VariationData &vdata = _variations[index];
    vdata.skip = false;

    for (size_t i = 0; i < n; i++) {
      const ShaderObject::Combo *combo = &(_sho->get_combo(i));
      vdata.options.set_define(combo->name, combo->min_val + indices[i]);
    }

    _curr_options = &vdata.options;
    // Evaluate the skip commands to see if we should skip this variation.
    for (size_t i = 0; i < _skip_commands.size(); i++) {
      if (_skip_commands[i].eval() != 0) {
        // The expression evaluated to true for this variation.  Skip it.
        vdata.skip = true;
        if (_verbose) {
          nout << "Skipping variation " << index << " with defines:\n";
          for (size_t i = 0; i < vdata.options.get_num_defines(); i++) {
            const ShaderCompiler::Options::Define *define = vdata.options.get_define(i);
            nout << "\t" << define->name->get_name() << "\t" << define->value << "\n";
          }
        }
        _num_skipped++;
        break;
      }
    }
    _curr_options = nullptr;

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
  compile_variation(_non_skipped_variations[_non_skipped_variations.size() - 1]);
  ThreadManager::run_threads_on_individual("CompileVariations", (int)num_real_variations - 1, false,
                                           std::bind(&ShaderCompile::compile_variation, this, std::placeholders::_1));

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
  int v_index = _non_skipped_variations[n];
  const VariationData &vdata = _variations[v_index];

  // It shouldn't be a skipped variation.
  assert(!vdata.skip);

  if (_verbose) {
    nout << "Compiling variation " << v_index << " with defines:\n";
    for (size_t i = 0; i < vdata.options.get_num_defines(); i++) {
      const ShaderCompiler::Options::Define *define = vdata.options.get_define(i);
      nout << "\t" << define->name->get_name() << "\t" << define->value << "\n";
    }
  }

  std::istream *stream = _source_vf->open_read_file(true);
  if (stream == nullptr) {
    nout << "Error: Could not open shader file for reading: " << _input_filename << "\n";
    exit(1);
  }

  PT(ShaderModule) module = compiler.compile_now(
    _stage, *stream, _source_vf->get_filename(),
    vdata.options, nullptr);

  _source_vf->close_read_file(stream);

  if (module == nullptr) {
    nout << "Failed to compile variation " << n << "!\n";
    nout << "Defines:\n";
    for (size_t i = 0; i < vdata.options.get_num_defines(); i++) {
      const ShaderCompiler::Options::Define *define = vdata.options.get_define(i);
      nout << "\t" << define->name->get_name() << "\t" << define->value << "\n";
    }
    exit(1);
  }

  _sho->set_permutation(v_index, module);
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
 * Parses the shader source, looking for combos.  Discovered combo definitions
 * are added to the shader object.
 */
void ShaderCompile::
collect_combos() {
  vector_string lines;
  tokenize(_shader_source, lines, "\n");

  for (size_t i = 0; i < lines.size(); i++) {
    const std::string &line = lines[i];

    if (line.size() < 7) {
      // Can't contain combo.
      continue;
    }

    if (line.substr(0, 7) != "#pragma") {
      // Line doesn't start with #pragma, can't possibly be a combo definition.
      continue;
    }

    // Get everything after the #pragma.
    std::string combo_line = line.substr(7);
    vector_string combo_words;
    extract_words(combo_line, combo_words);

    if (combo_words.size() == 0) {
      // It's not a #pragma combo.
      continue;
    }

    if (combo_words[0] == "combo") {
      // It's a combo command.

      // Must contain four words: combo, name, min val, max val.
      if (combo_words.size() != 4) {
        nout << "Error: Invalid combo definition at line " << i + 1 << " of "
            << _input_filename.get_fullpath() << "\n";
        continue;
      }

      std::string name = combo_words[1];
      int min_val, max_val;
      if (!string_to_int(combo_words[2], min_val)) {
        nout << "Error: Invalid min combo value at line " << i + 1 << " of "
            << _input_filename.get_fullpath() << "\n";
        continue;
      }
      if (!string_to_int(combo_words[3], max_val)) {
        nout << "Error: Invalid max combo value at line " << i + 1 << " of "
            << _input_filename.get_fullpath() << "\n";
        continue;
      }

      ShaderObject::Combo combo;
      combo.name = InternalName::make(name);
      combo.min_val = min_val;
      combo.max_val = max_val;
      if (_verbose) {
        nout << "Found combo " << name << " with min value " << min_val
            << " and max value " << max_val << "\n";
      }
      _sho->add_combo(std::move(combo));

    } else if (combo_words[0] == "skip") {
      // It's a skip command.  Everything after the skip is the expression.

      std::string expression;
      for (size_t i = 1; i < combo_words.size(); i++) {
        expression += combo_words[i];
        if (i < combo_words.size() - 1) {
          expression += " ";
        }
      }

      if (_verbose) {
        nout << "Skip expression: " << expression << "\n";
      }

      // Parse the expression to build up an actual skip command.
      _skip_commands.push_back(r_expand_expression(expression, 0));
    }
  }
}

/**
 *
 */
ShaderCompile::SkipCommand ShaderCompile::
r_expand_expression(const std::string &str, size_t p) {
  SkipCommand cmd;

  std::string literal;
  // Search for the beginning of a command.
  while (p < str.length()) {
    if (p + 1 < str.length() && str[p] == VARIABLE_PREFIX &&
        str[p + 1] == VARIABLE_OPEN_BRACE) {
        // Found a command.  Expand it.
        if (_verbose) {
          nout << "command: " << str << "\n";
        }
        cmd = r_expand_command(str, p);

    } else {
      // Must just be a literal value.
      cmd.cmd = SkipCommand::C_literal;
      literal += str[p];
      p++;
    }
  }

  if (cmd.cmd == SkipCommand::C_literal) {
    bool ret = string_to_int(literal, cmd.value);
    if (!ret) {
      nout << "Error!  Invalid literal integer: " << literal << "\n";
      assert(0);
    }
  }

  return cmd;
}

/**
 * Tokenizes the function parameters, skipping nested variables/functions.
 */
void ShaderCompile::
tokenize_params(const std::string &str, vector_string &tokens,
                bool expand) {
  size_t p = 0;
  while (p < str.length()) {
    // Skip initial whitespace.
    while (p < str.length() && isspace(str[p])) {
      p++;
    }

    std::string token;
    while (p < str.length() && str[p] != FUNCTION_PARAMETER_SEPARATOR) {
      if (p + 1 < str.length() && str[p] == VARIABLE_PREFIX &&
          str[p + 1] == VARIABLE_OPEN_BRACE) {
        // Skip a nested variable reference.
        token += r_scan_variable(str, p);
      } else {
        token += str[p];
        p++;
      }
    }

    // Back up past trailing whitespace.
    size_t q = token.length();
    while (q > 0 && isspace(token[q - 1])) {
      q--;
    }

    tokens.push_back(token.substr(0, q));
    p++;

    if (p == str.length()) {
      // In this case, we have just read past a trailing comma symbol
      // at the end of the string, so we have one more empty token.
      tokens.push_back(std::string());
    }
  }
}

/**
 *
 */
ShaderCompile::SkipCommand ShaderCompile::
r_expand_command(const std::string &str, size_t &vp) {
  SkipCommand cmd;

  std::string varname;
  size_t whitespace_at = 0;

  size_t p = vp + 2;
  while (p < str.length() && str[p] != VARIABLE_CLOSE_BRACE) {
    if (p + 1 < str.length() && str[p] == VARIABLE_PREFIX &&
        str[p + 1] == VARIABLE_OPEN_BRACE) {
      if (whitespace_at == 0) {
        nout << "Error!  Nested skip commands can only be function arguments.\n";
        assert(0);
      }

      varname += r_scan_variable(str, p);
    } else {
      if (whitespace_at == 0 && isspace(str[p])) {
        whitespace_at = p - (vp + 2);
      }
      varname += str[p];
      p++;
    }
  }

  if (p < str.length()) {
    assert(str[p] == VARIABLE_CLOSE_BRACE);
    p++;
  } else {
    nout << "Warning!  Unclosed variable reference:\n"
         << str.substr(vp) << "\n";
  }

  vp = p;

  // Check for a function expansion.
  if (whitespace_at != 0) {
    std::string funcname = varname.substr(0, whitespace_at);
    p = whitespace_at;
    while (p < varname.length() && isspace(varname[p])) {
      p++;
    }

    vector_string params;
    tokenize_params(varname.substr(p), params, false);

    if (funcname == "and") {
      cmd.cmd = SkipCommand::C_and;
    } else if (funcname == "or") {
      cmd.cmd = SkipCommand::C_or;
    } else if (funcname == "not") {
      cmd.cmd = SkipCommand::C_not;
    } else if (funcname == "eq") {
      cmd.cmd = SkipCommand::C_eq;
    } else if (funcname == "neq") {
      cmd.cmd = SkipCommand::C_neq;
    } else {
      nout << "Error!  Unknown skip function: " << funcname << "\n";
      assert(0);
    }

    for (size_t i = 0; i < params.size(); i++) {
      if (_verbose) {
        nout << "param " << i << ": " << params[i] << "\n";
      }
      cmd.arguments.push_back(r_expand_expression(params[i], 0));
    }
  } else {
    // Not a function, must be a combo variable reference.
    cmd.cmd = SkipCommand::C_ref;
    cmd.name = varname;
  }

  return cmd;
}

/**
 *
 */
std::string ShaderCompile::
r_scan_variable(const std::string &str, size_t &vp) {

  // Search for the end of the variable name: an unmatched square
  // bracket.
  size_t start = vp;
  size_t p = vp + 2;
  while (p < str.length() && str[p] != VARIABLE_CLOSE_BRACE) {
    if (p + 1 < str.length() && str[p] == VARIABLE_PREFIX &&
        str[p + 1] == VARIABLE_OPEN_BRACE) {
      // Here's a nested variable!  Scan past it, matching braces
      // properly.
      r_scan_variable(str, p);
    } else {
      p++;
    }
  }

  if (p < str.length()) {
    assert(str[p] == VARIABLE_CLOSE_BRACE);
    p++;
  } else {
    nout << "Warning!  Unclosed variable reference:\n"
         << str.substr(vp) << "\n";
  }

  vp = p;
  return str.substr(start, vp - start);
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
 *
 */
int ShaderCompile::SkipCommand::
eval() {
  switch (cmd) {

  case C_and: {
    // All of the arguments must evaluate to true.
    for (size_t i = 0; i < arguments.size(); i++) {
      if (!arguments[i].eval()) {
        return 0;
      }
    }

    return 1;
  }

  case C_or: {
    // At least one argument must evaluate to true.
    for (size_t i = 0; i < arguments.size(); i++) {
      if (arguments[i].eval()) {
        return 1;
      }
    }

    return 0;
  }

  case C_not: {
    return !arguments[0].eval();
  }

  case C_eq: {
    return arguments[0].eval() == arguments[1].eval();
  }

  case C_neq: {
    return arguments[0].eval() != arguments[1].eval();
  }

  case C_literal: {
    return value;
  }

  case C_ref: {
    return g_prog->_curr_options->get_define(name)->value;
  }

  default:
    return 0;
  }
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

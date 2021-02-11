/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file pmatToRso.cxx
 * @author lachbr
 * @date 2021-02-06
 */

#include "pmatToRso.h"
#include "material.h"
#include "renderState.h"
#include "textureAttrib.h"
#include "string_utils.h"
#include "bamFile.h"

/**
 *
 */
PMatToRso::
PMatToRso() :
  ProgramBase("pmat2rso"),
  WithOutputFile(true, false, true) {

  textures_header_only = true;

  set_program_brief("convert a .pmat file into a .rso file");
  set_program_description
    ("This program compiles a source .pmat material file into a binary .rso "
     "render state object.  It is similar to a .bam file, but it stores render "
     "state information.  As with .bam files, .rso files are tied to a specific "
     "version of Panda.  The .ptex files referenced by the .pmat are assumed to "
     "have already been compiled into .txos.  The texture filename is simply "
     "changed to have a .txo extension and be relative to the root installation "
     "directory, which you must specify on the command line.");

  add_runline("[opts] -o output.rso input.pmat");
  add_runline("[opts] input.pmat output.rso");

  add_path_store_options();
  add_path_replace_options();

  add_option
    ("o", "filename", 0,
     "Specify the filename to which the resulting .rso file will be written.  "
     "If this option is omitted, the last parameter name is taken to be the "
     "name of the output file.",
     &PMatToRso::dispatch_filename, &_got_output_filename, &_output_filename);
}

/**
 *
 */
bool PMatToRso::
run() {
  DSearchPath path = get_model_path();
  path.append_directory(".");

  Filename fullpath = _input_filename;
  if (!fullpath.resolve_filename(path)) {
    std::cerr << "Couldn't locate the input material!\n";
    return false;
  }

  if (_output_filename.compare_timestamps(fullpath) > 0) {
    // Up to date.
    nout << "Output material is up-to-date.\n";
    return true;
  }

  PT(Material) pmat = Material::load(_input_filename, path);
  if (pmat == nullptr) {
    std::cerr << "Couldn't load input material!\n";
    return false;
  }

  CPT(RenderState) state = RenderState::make(pmat);
  if (state == nullptr || state->is_empty()) {
    std::cerr << "Couldn't convert material to render state!\n";
    return false;
  }

  DSearchPath remap_path;
  remap_path.append_directory(".");

  // Now go through each Texture and remap the filename to where it should be
  // as a .txo in the install tree.
  const TextureAttrib *ta;
  state->get_attrib_def(ta);
  for (int i = 0; i < ta->get_num_on_stages(); i++) {
    TextureStage *stage = ta->get_on_stage(i);
    Texture *tex = ta->get_on_texture(stage);

    if (!tex->has_filename()) {
      continue;
    }

    tex->clear_alpha_filename();
    tex->clear_alpha_fullpath();

    Filename tex_fullpath = tex->get_fullpath();
    tex_fullpath.set_extension("txo");

    Filename txo_resolved, txo_output;
    _path_replace->full_convert_path(
      tex_fullpath, remap_path, txo_resolved, txo_output);

    nout << tex_fullpath << " -> " << txo_resolved << " : " << txo_output << "\n";

    tex->set_fullpath(txo_resolved);
    tex->set_filename(txo_output);
    tex->set_loaded_from_txo();
  }

  if (!state->write_rso(_output_filename)) {
    std::cerr << "Failed to write " << _output_filename.get_fullpath() << "\n";
    return false;
  }

  return true;
}

/**
 *
 */
bool PMatToRso::
handle_args(Args &args) {
  if (args.empty()) {
    nout << "You didn't specify an input material!\n";
    return false;
  }

  // If the user specified a path store option, we need to set the bam-
  // texture-mode Config.prc variable directly to support this (otherwise the
  // bam code will do what it wants to do anyway).
  if (_got_path_store) {
    bam_texture_mode = BamFile::BTM_unchanged;
    bam_material_mode = BamFile::BTM_unchanged;

  } else {
    // Otherwise, the default path store is absolute; then the bam-texture-
    // mode can do the appropriate thing to it.
    _path_replace->_path_store = PS_absolute;
  }

  _input_filename = args[0];

  return true;
}

/**
 *
 */
int
main(int argc, char *argv[]) {
  PMatToRso prog;
  prog.parse_command_line(argc, argv);
  return prog.run() ? 0 : 1;
}

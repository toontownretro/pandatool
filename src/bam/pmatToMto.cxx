/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file pmatToMto.cxx
 * @author lachbr
 * @date 2021-02-06
 */

#include "pmatToMto.h"
#include "material.h"
#include "materialPool.h"
#include "materialAttrib.h"
#include "materialParamTexture.h"
#include "textureAttrib.h"
#include "string_utils.h"
#include "bamFile.h"
#include "modelIndex.h"
#include "config_putil.h"

/**
 *
 */
PMatToMto::
PMatToMto() :
  ProgramBase("pmat2mto"),
  WithOutputFile(true, false, true) {

  textures_header_only = true;

  _got_index_filename = false;

  set_program_brief("convert a .pmat file into a .mto file");
  set_program_description
    ("This program compiles a source .pmat material file into a binary .mto "
     "material object.  It is similar to a .bam file, but it stores material "
     "information.  As with .bam files, .mto files are tied to a specific "
     "version of Panda.  The .ptex files referenced by the .pmat are assumed to "
     "have already been compiled into .txos.  The texture filename is simply "
     "changed to have a .txo extension and be relative to the root installation "
     "directory, which you must specify on the command line.");

  add_runline("[opts] -o output.mto input.pmat");
  add_runline("[opts] input.pmat output.mto");

  add_option
    ("o", "filename", 0,
     "Specify the filename to which the resulting .mto file will be written.  "
     "If this option is omitted, the last parameter name is taken to be the "
     "name of the output file.",
     &PMatToMto::dispatch_filename, &_got_output_filename, &_output_filename);

  add_option
    ("i", "filename", 0,
     "Specify the model index filename for the model tree that the material "
     "its referenced textures reside in.  This is used to remap the texture "
     "pathnames to the built/installed versions and make them relative to "
     "the root installation directory.",
     &PMatToMto::dispatch_filename, &_got_index_filename, &_index_filename);
}

/**
 *
 */
bool PMatToMto::
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

  PT(Material) pmat = MaterialPool::load_material(_input_filename, path);
  if (pmat == nullptr) {
    std::cerr << "Couldn't load input material!\n";
    return false;
  }

  // Remap each texture to the install tree version using the model index.

  ModelIndex *index = ModelIndex::get_global_ptr();
  if (_got_index_filename) {
    if (!index->read_index(_index_filename)) {
      nout << "Failed to read model tree index: " << _index_filename << "\n";
      return false;
    }
  }

  if (index->get_num_trees() == 0) {
    std::cerr << "I don't have an index for any model trees!\n";
    return false;
  }

  ModelIndex::Tree *tree = index->get_tree(index->get_num_trees() - 1);
  ModelIndex::AssetIndex *tex_index = tree->_asset_types["textures"];
  if (tex_index == nullptr) {
    std::cerr << "No texture index in model tree " << tree->_name << "\n";
    return false;
  }

  // Now go through each texture parameter and remap the filename to where it
  // should be as a .txo in the install tree.
  for (size_t i = 0; i < pmat->get_num_params(); i++) {
    MaterialParamBase *param = pmat->get_param(i);
    if (!param->is_of_type(MaterialParamTexture::get_class_type())) {
      continue;
    }

    Texture *tex = DCAST(MaterialParamTexture, param)->get_value();

    if (tex == nullptr || !tex->has_filename()) {
      continue;
    }

    tex->clear_alpha_filename();
    tex->clear_alpha_fullpath();

    auto it = tex_index->_assets.find(tex->get_filename().get_basename_wo_extension());
    if (it == tex_index->_assets.end()) {
      std::cerr << "Could not locate texture "
                << tex->get_filename()
                << " in the model index!  Add it to a Sources.pp file.\n";
      return false;
    }

    ModelIndex::Asset *tex_asset = (*it).second;

    nout << "Remapping " << tex->get_filename() << " to " << tex_asset->_built << "\n";

    tex->set_fullpath(tex_asset->_built);

    // Make it relative to the install root.
    Filename rel_path = tex_asset->_built;
    rel_path.make_canonical();
    rel_path.make_relative_to(tree->_install_dir, false);
    tex->set_filename(rel_path);

    tex->set_loaded_from_txo();
  }

  if (!pmat->write_mto(_output_filename)) {
    std::cerr << "Failed to write " << _output_filename.get_fullpath() << "\n";
    return false;
  }

  return true;
}

/**
 *
 */
bool PMatToMto::
handle_args(Args &args) {
  if (args.empty()) {
    nout << "You didn't specify an input material!\n";
    return false;
  }

  // If the user specified a path store option, we need to set the bam-
  // texture-mode Config.prc variable directly to support this (otherwise the
  // bam code will do what it wants to do anyway).
  if (true || _got_path_store) {
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
  PMatToMto prog;
  prog.parse_command_line(argc, argv);
  return prog.run() ? 0 : 1;
}

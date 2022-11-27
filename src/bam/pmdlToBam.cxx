/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file pmdlToBam.cxx
 * @author lachbr
 * @date 2021-09-13
 */

#include "pmdlToBam.h"
#include "config_gobj.h"
#include "config_putil.h"
#include "bamFile.h"
#include "pmdlLoader.h"
#include "load_egg_file.h"
#include "pandaNode.h"
#include "nodePath.h"
#include "geomNode.h"
#include "renderState.h"
#include "materialAttrib.h"
#include "textureAttrib.h"
#include "modelRoot.h"
#include "modelIndex.h"

/**
 *
 */
PmdlToBam::
PmdlToBam() :
  ProgramBase("pmdl2bam"),
  WithOutputFile(true, false, true)
{
  textures_header_only = true;
  _got_index_filename = false;

  set_program_brief("converts a .pmdl model file to a .bam file");
  set_program_description(
    "This program is used to convert source .pmdl model description files "
    "into binary .bam files, suitable for loading at runtime.");

  add_runline("[opts] -o output.bam input.pmdl");
  add_runline("[opts] input.pmdl output.bam");

  add_option
    ("o", "filename", 0,
     "Specify the filename to which the resulting .bam file will be written.  "
     "If this option is omitted, the last parameter name is taken to be the "
     "name of the output file.",
     &PmdlToBam::dispatch_filename, &_got_output_filename, &_output_filename);

  add_option
    ("i", "filename", 0,
     "Specify the model index filename for the model tree that the model and "
     "its referenced textures reside in.  This is used to remap the texture "
     "pathnames to the built/installed versions and make them relative to "
     "the root installation directory.",
     &PmdlToBam::dispatch_filename, &_got_index_filename, &_index_filename);
}

/**
 *
 */
bool PmdlToBam::
run() {
  DSearchPath path = get_model_path();
  path.append_directory(".");

  Filename fullpath = _input_filename;
  if (!fullpath.resolve_filename(path)) {
    std::cerr << "Couldn't locate the input model!\n";
    return false;
  }

  // Load up the data description.
  PT(PMDLDataDesc) data = new PMDLDataDesc;
  if (!data->load(fullpath, path)) {
    nout << "Could not load input model " << fullpath << "\n";
    return false;
  }

  path.append_directory(fullpath.get_dirname());

  // Now turn it into a scene graph.
  PT(PandaNode) root = load_pmdl_data(data);

  if (root == nullptr) {
    nout << "Failed to build scene graph from model file " << fullpath << "\n";
    return false;
  }

  ModelIndex *index = ModelIndex::get_global_ptr();
  if (_got_index_filename) {
    if (!index->read_index(_index_filename)) {
      nout << "Failed to read model tree index: " << _index_filename << "\n";
      return false;
    }
  }

  if (index->get_num_trees() == 0) {
    nout << "WARNING: No model tree indexes loaded\n";
  }

  // Redirect any materials and textures to their install tree counterpart.
  if (index->get_num_trees() > 0) {
    // If we have a model tree index, use that to remap textures and materials
    // where applicable.  If a texture or material is referenced that exists
    // in the model index, remap the filename to the coincident filename in the
    // install tree.

    collect_materials(root);

    ModelIndex::Tree *tree = index->get_tree(index->get_num_trees() - 1);

    auto mit = tree->_asset_types.find("materials");
    if (mit != tree->_asset_types.end()) {
      ModelIndex::AssetIndex *mat_assets = (*mit).second;

      for (auto it = _materials.begin(); it != _materials.end(); ++it) {
        Material *mat = *it;

        if (mat == nullptr) {
          nout << "ERROR: Collected a nullptr material.  It must've not "
               << "been found on disk.  Check above in console for which "
               << "material it didn't find.\n";
          return false;
        }

        auto mi = mat_assets->_assets.find(mat->get_fullpath());
        if (mi == mat_assets->_assets.end()) {
          nout << "Material " << mat->get_filename()
              << " does not exist in the model index!  Add it to a Sources.pp file.\n";
          return false;
        }

        ModelIndex::Asset *mat_asset = (*mi).second;

        nout << "Remapping material " << mat->get_filename() << " to " << mat_asset->_built << "\n";

        Filename rel_path = mat_asset->_built;
        rel_path.make_canonical();
        rel_path.make_relative_to(tree->_install_dir, false);
        mat->set_filename(rel_path);
        mat->set_fullpath(mat_asset->_built);
      }
    }

    auto tit = tree->_asset_types.find("textures");
    if (tit != tree->_asset_types.end()) {
      ModelIndex::AssetIndex *tex_assets = (*tit).second;
      // Also remap any textures that are part of a RenderState that didn't come
      // from a .pmat.
      for (auto it = _textures.begin(); it != _textures.end(); ++it) {
        Texture *tex = *it;

        auto ti = tex_assets->_assets.find(tex->get_fullpath());
        if (ti == tex_assets->_assets.end()) {
          nout << "Texture " << tex->get_filename()
              << " does not exist in the model index!  Add it to a Sources.pp file.\n";
          return false;
        }

        ModelIndex::Asset *tex_asset = (*ti).second;

        nout << "Remapping texture " << tex->get_filename() << " to " << tex_asset->_built << "\n";

        Filename rel_path = tex_asset->_built;
        rel_path.make_canonical();
        rel_path.make_relative_to(tree->_install_dir, false);
        tex->set_filename(rel_path);
        tex->set_fullpath(tex_asset->_built);
        tex->set_loaded_from_txo();
      }
    }
  }

  // This should be guaranteed because we pass false to the constructor,
  // above.
  nassertr(has_output_filename(), false);

  // Save it out!
  Filename filename = get_output_filename();
  filename.make_dir();
  nout << "Writing " << filename << "\n";
  BamFile bam_file;
  if (!bam_file.open_write(filename)) {
    nout << "Error in writing.\n";
    return false;
  }

  if (!bam_file.write_object(root)) {
    nout << "Error in writing.\n";
    return false;
  }

  return true;
}

/**
 *
 */
bool PmdlToBam::
handle_args(Args &args) {
  if (args.empty()) {
    nout << "You didn't specify an input model file!\n";
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
 * Recursively walks the scene graph, looking for RenderStates that have
 * filenames.
 */
void PmdlToBam::
collect_materials(PandaNode *node) {
  const RenderState *state = node->get_state();

  const MaterialAttrib *mattr;
  state->get_attrib_def(mattr);
  Material *mat = mattr->get_material();
  if (!mattr->is_off() && mat != nullptr && !mat->get_filename().empty()) {
    _materials.insert(mat);
  }

  const TextureAttrib *ta;
  state->get_attrib_def(ta);
  for (size_t i = 0; i < ta->get_num_on_stages(); i++) {
    Texture *tex = ta->get_on_texture(ta->get_on_stage(i));
    _textures.insert(tex);
  }

  if (node->is_geom_node()) {
    GeomNode *geom_node = DCAST(GeomNode, node);
    int num_geoms = geom_node->get_num_geoms();
    for (int i = 0; i < num_geoms; ++i) {
      state = geom_node->get_geom_state(i);

      state->get_attrib_def(mattr);
      mat = mattr->get_material();
      if (!mattr->is_off() && mat != nullptr && !mat->get_filename().empty()) {
        _materials.insert(mat);
      }

      state->get_attrib_def(ta);
      for (size_t i = 0; i < ta->get_num_on_stages(); i++) {
        Texture *tex = ta->get_on_texture(ta->get_on_stage(i));
        _textures.insert(tex);
      }
    }
  } else if (node->is_of_type(ModelRoot::get_class_type())) {
    // ModelRoots can contain material groups, so we need to remap those as
    // well.
    ModelRoot *model_root = DCAST(ModelRoot, node);
    for (size_t i = 0; i < model_root->get_num_material_groups(); i++) {
      const MaterialCollection &group = model_root->get_material_group(i);
      for (int j = 0; j < group.get_num_materials(); j++) {
        _materials.insert(group.get_material(j));
      }
    }
  }

  PandaNode::Children children = node->get_children();
  int num_children = children.get_num_children();
  for (int i = 0; i < num_children; ++i) {
    collect_materials(children.get_child(i));
  }
}

/**
 * Program entry point.
 */
int
main(int argc, char *argv[]) {
  PmdlToBam prog;
  prog.parse_command_line(argc, argv);
  return prog.run() ? 0 : 1;
}

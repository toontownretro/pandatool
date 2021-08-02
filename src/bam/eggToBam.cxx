/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file eggToBam.cxx
 * @author drose
 * @date 2000-06-28
 */

#include "eggToBam.h"

#include "config_putil.h"
#include "bamFile.h"
#include "load_egg_file.h"
#include "config_egg2pg.h"
#include "config_gobj.h"
#include "config_anim.h"
#include "pandaNode.h"
#include "geomNode.h"
#include "renderState.h"
#include "textureAttrib.h"
#include "dcast.h"
#include "graphicsPipeSelection.h"
#include "graphicsEngine.h"
#include "graphicsBuffer.h"
#include "graphicsStateGuardian.h"
#include "load_prc_file.h"
#include "windowProperties.h"
#include "frameBufferProperties.h"
#include "datagramIterator.h"
#include "datagram.h"
#include "virtualFileSystem.h"
#include "modelIndex.h"
#include "material.h"
#include "materialAttrib.h"
#include "modelRoot.h"

/**
 *
 */
EggToBam::
EggToBam() :
  EggToSomething("Bam", ".bam", true, false)
{
  // Filename references must be used to compare RenderStates.
  get_uniquify_states_ignore_filenames() = false;
  textures_header_only = true;

  set_program_brief("convert .egg files to .bam files");
  set_program_description
    ("This program reads Egg files and outputs Bam files, the binary format "
     "suitable for direct loading of animation and models into Panda.  Bam "
     "files are tied to a particular version of Panda, so should not be "
     "considered replacements for egg files, but they tend to be smaller and "
     "load much faster than the equivalent egg files.");

  // -f is always in effect for egg2bam.  It doesn't make sense to provide it
  // as an option to the user.
  remove_option("f");

  add_path_replace_options();
  add_path_store_options();

  add_option
    ("flatten", "flag", 0,
     "Specifies whether to flatten the egg hierarchy after it is loaded.  "
     "If flag is zero, the egg hierarchy will not be flattened, but will "
     "instead be written to the bam file exactly as it is.  If flag is "
     "non-zero, the hierarchy will be flattened so that unnecessary nodes "
     "(usually group nodes with only one child) are eliminated.  The default "
     "if this is not specified is taken from the egg-flatten Config.prc "
     "variable.",
     &EggToBam::dispatch_int, &_has_egg_flatten, &_egg_flatten);

  add_option
    ("combine-geoms", "flag", 0,
     "Specifies whether to combine sibling GeomNodes into a common GeomNode "
     "when possible.  This flag is only respected if flatten, above, is also "
     "enabled (or implicitly true from the Config.prc file).  The default if "
     "this is not specified is taken from the egg-combine-geoms Config.prc "
     "variable.",
     &EggToBam::dispatch_int, &_has_egg_combine_geoms, &_egg_combine_geoms);

  add_option
    ("suppress-hidden", "flag", 0,
     "Specifies whether to suppress hidden geometry.  If this is nonzero, "
     "egg geometry tagged as \"hidden\" will be removed from the final "
     "scene graph; otherwise, it will be preserved (but stashed).  The "
     "default is nonzero, to remove it.",
     &EggToBam::dispatch_int, nullptr, &_egg_suppress_hidden);

  add_option
    ("ls", "", 0,
     "Writes a scene graph listing to standard output after the egg "
     "file has been loaded, showing the nodes that will be written out.",
     &EggToBam::dispatch_none, &_ls);

  add_option
    ("C", "quality", 0,
     "Specify the quality level for lossy channel compression.  If this "
     "is specified, the animation channels will be compressed at this "
     "quality level, which is normally an integer value between 0 and 100, "
     "inclusive, where higher numbers produce larger files with greater "
     "quality.  Generally, 95 is the highest useful quality level.  Use "
     "-NC (described below) to disable channel compression.  If neither "
     "option is specified, the default comes from the Config.prc file.",
     &EggToBam::dispatch_int, &_has_compression_quality, &_compression_quality);

  add_option
    ("NC", "", 0,
     "Turn off lossy compression of animation channels.  Channels will be "
     "written exactly as they are, losslessly.",
     &EggToBam::dispatch_none, &_compression_off);

  redescribe_option
    ("cs",
     "Specify the coordinate system of the resulting " + _format_name +
     " file.  This may be "
     "one of 'y-up', 'z-up', 'y-up-left', or 'z-up-left'.  The default "
     "is z-up.");

  add_option
    ("i", "filename", 0,
     "Specify the model index filename for the model tree that the model and "
     "its referenced textures/materials reside in.  This is used to remap the "
     "texture pathnames to the built/installed versions and make them relative "
     "to the root installation directory.",
     &EggToBam::dispatch_filename, &_got_index_filename, &_index_filename);

  _force_complete = true;
  _got_index_filename = false;
  _egg_flatten = 0;
  _egg_combine_geoms = 0;
  _egg_suppress_hidden = 1;
}

/**
 *
 */
void EggToBam::
run() {
  if (_has_egg_flatten) {
    // If the user specified some -flatten, we need to set the corresponding
    // Config.prc variable.
    egg_flatten = (_egg_flatten != 0);
  }
  if (_has_egg_combine_geoms) {
    // Ditto with -combine_geoms.
    egg_combine_geoms = (_egg_combine_geoms != 0);
  }

  // We always set egg_suppress_hidden.
  egg_suppress_hidden = _egg_suppress_hidden;

  if (_compression_off) {
    // If the user specified -NC, turn off channel compression.
    compress_channels = false;

  } else if (_has_compression_quality) {
    // Otherwise, if the user specified a compression quality with -C, use
    // that quality level.
    compress_channels = true;
    compress_chan_quality = _compression_quality;
  }

  if (!_got_coordinate_system) {
    // If the user didn't specify otherwise, ensure the coordinate system is
    // Z-up.
    _data->set_coordinate_system(CS_zup_right);
  }

  ModelIndex *index = ModelIndex::get_global_ptr();
  if (_got_index_filename) {
    if (!index->read_index(_index_filename)) {
      nout << "Failed to read model tree index: " << _index_filename << "\n";
      exit(1);
    }
  }

  if (index->get_num_trees() == 0) {
    nout << "WARNING: No model tree indexes loaded\n";
  }

  PT(PandaNode) root = load_egg_data(_data);
  if (root == nullptr) {
    nout << "Unable to build scene graph from egg file.\n";
    exit(1);
  }

  if (index->get_num_trees() > 0) {
    // If we have a model tree index, use that to remap textures and materials
    // where applicable.  If a texture or material is referenced that exists
    // in the model index, remap the filename to the coincident filename in the
    // install tree.

    // Collect all the materials and textures from the RenderStates.  We need to
    // remap the filenames to the install tree.
    collect_materials(root);

    ModelIndex::Tree *tree = index->get_tree(index->get_num_trees() - 1);

    auto mit = tree->_asset_types.find("materials");
    if (mit != tree->_asset_types.end()) {
      ModelIndex::AssetIndex *mat_assets = (*mit).second;

      for (auto it = _materials.begin(); it != _materials.end(); ++it) {
        Material *mat = *it;

        auto mi = mat_assets->_assets.find(mat->get_filename().get_basename_wo_extension());
        if (mi == mat_assets->_assets.end()) {
          nout << "Material " << mat->get_filename()
              << " does not exist in the model index!  Add it to a Sources.pp file.\n";
          exit(1);
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

        auto ti = tex_assets->_assets.find(tex->get_filename().get_basename_wo_extension());
        if (ti == tex_assets->_assets.end()) {
          // Commenting this out to support the palettizer.
          //nout << "Texture " << tex->get_filename()
          //    << " does not exist in the model index!  Add it to a Sources.pp file.\n";
          //exit(1);
          continue;
        }

        ModelIndex::Asset *tex_asset = (*ti).second;

        nout << "Remapping texture " << tex->get_filename() << " to " << tex_asset->_built << "\n";

        Filename rel_path = tex_asset->_built;
        rel_path.make_canonical();
        rel_path.make_relative_to(tree->_install_dir, false);
        tex->set_filename(rel_path);
        tex->set_fullpath(tex_asset->_built);

        if (tex_asset->_built.get_extension() == "txo") {
          tex->set_loaded_from_txo();
        }
      }
    }
  }

  if (_ls) {
    root->ls(nout, 0);
  }

  // This should be guaranteed because we pass false to the constructor,
  // above.
  nassertv(has_output_filename());

  Filename filename = get_output_filename();
  filename.make_dir();
  nout << "Writing " << filename << "\n";
  BamFile bam_file;
  if (!bam_file.open_write(filename)) {
    nout << "Error in writing.\n";
    exit(1);
  }

  if (!bam_file.write_object(root)) {
    nout << "Error in writing.\n";
    exit(1);
  }
}

/**
 * Does something with the additional arguments on the command line (after all
 * the -options have been parsed).  Returns true if the arguments are good,
 * false otherwise.
 */
bool EggToBam::
handle_args(ProgramBase::Args &args) {
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

  return EggToSomething::handle_args(args);
}

/**
 * Recursively walks the scene graph, looking for RenderStates that have
 * filenames.
 */
void EggToBam::
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

int main(int argc, char *argv[]) {
  EggToBam prog;
  prog.parse_command_line(argc, argv);
  prog.run();
  return 0;
}

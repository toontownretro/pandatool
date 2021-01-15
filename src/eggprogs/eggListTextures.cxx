/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file eggListTextures.cxx
 * @author drose
 * @date 2005-05-23
 */

#include "eggListTextures.h"
#include "eggMaterialCollection.h"
#include "material.h"
#include "pnmImageHeader.h"

/**
 *
 */
EggListTextures::
EggListTextures() {
  set_program_brief("list textures referenced by an .egg file");
  set_program_description
    ("egg-list-textures reads an egg file and writes a list of the "
     "textures it references.  It is particularly useful for building "
     "up the textures.txa file used for egg-palettize, since the output "
     "format is crafted to be compatible with that file's input format.");
}

/**
 *
 */
void EggListTextures::
run() {
  if (!do_reader_options()) {
    exit(1);
  }

  EggMaterialCollection tc;
  tc.find_used_materials(_data);
  EggMaterialCollection::MaterialReplacement treplace;
  tc.collapse_equivalent_materials(EggMaterial::E_filename, treplace);
  //tc.sort_by_basename();

  EggMaterialCollection::iterator ti;
  for (ti = tc.begin(); ti != tc.end(); ++ti) {
    Filename fullpath = (*ti)->get_fullpath();

    PT(Material) mat = Material::load(fullpath);
    if (mat == nullptr) {
      continue;
    }

    for (size_t i = 0; i < mat->get_num_textures(); i++) {
      Material::ScriptTexture *tex = mat->get_texture(i);
      if (tex->_texture_type == Material::ScriptTexture::T_filename) {
        PNMImageHeader header;
        if (header.read_header(tex->_fullpath)) {
          std::cout << tex->_filename.get_basename() << " : "
              << header.get_x_size() << " " << header.get_y_size() << "\n";
        } else {
          std::cout << tex->_filename.get_basename() << " : unknown\n";
        }
      }
    }
  }
}


int main(int argc, char *argv[]) {
  EggListTextures prog;
  prog.parse_command_line(argc, argv);
  prog.run();
  return 0;
}

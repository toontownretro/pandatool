/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file ptexToTxo.cxx
 * @author lachbr
 * @date 2021-02-05
 */

#include "ptexToTxo.h"
#include "pTexture.h"
#include "texture.h"
#include "texturePool.h"

/**
 *
 */
PTexToTxo::
PTexToTxo() :
  ProgramBase("ptex2txo"),
  WithOutputFile(true, false, true) {

  set_program_brief("convert a .ptex file to a .txo file");
  set_program_description
    ("This program compiles a source .ptex texture into a binary .txo texture.  "
     "The .txo file contains the texture properties specified in the .ptex file "
     "and the image data from the source image(s).  "
     "A .txo file is like a .bam file, but contains texture data.  Like .bam "
     "files, .txo files are tied to a specific version of Panda.");

  add_runline("-o output.txo input.ptex");
  add_runline("input.ptex output.txo");

  add_option
    ("o", "filename", 0,
     "Specify the filename to which the resulting .txo file will be written.  "
     "If this option is omitted, the last parameter name is taken to be the "
     "name of the output file.",
     &PTexToTxo::dispatch_filename, &_got_output_filename, &_output_filename);
}

/**
 *
 */
bool PTexToTxo::
run() {
  DSearchPath path = get_model_path().get_value();
  path.append_directory(".");

  PT(PTexture) ptex = PTexture::load(_input_filename, path);
  if (ptex == nullptr) {
    std::cerr << "Could not load the input texture!\n";
    return false;
  }

  bool needs_rewrite = true;

  // Only rewrite if necessary.
  Filename ptex_filename = ptex->get_fullpath();
  if (_output_filename.compare_timestamps(ptex_filename) > 0) {
    // Up-to-date with the .ptex, check the images.
    PTexture::TextureType type = ptex->get_texture_type();
    if (type == PTexture::TT_unspecified ||
        type == PTexture::TT_1d_texture ||
        type == PTexture::TT_2d_texture) {
      // Single-faced texture, check the image file and (if we have one) the
      // alpha file.

      if (_output_filename.compare_timestamps(ptex->get_image_fullpath()) > 0) {
        if (!ptex->has_alpha_image_filename()) {
          // Everything up-to-date.
          needs_rewrite = false;
        } else if (_output_filename.compare_timestamps(ptex->get_alpha_image_fullpath()) > 0) {
          // Everything up-to-date.
          needs_rewrite = false;
        }
      }
    } else {
      // This is a texture that has multiple slices.  We need to compare the
      // timestamps of each slice image file to the output.

      Filename pattern = ptex->get_image_filename();
      pattern.set_pattern(true);

      Filename alpha_pattern = ptex->get_alpha_image_filename();
      alpha_pattern.set_pattern(true);
      bool has_alpha = ptex->has_alpha_image_filename();

      path.append_directory(ptex->get_fullpath().get_dirname());

      bool all_ok = true;
      for (int i = 0; i < ptex->get_num_pages(); i++) {
        Filename page_filename = pattern.get_filename_index(i);
        if (page_filename.resolve_filename(path)) {
          if (_output_filename.compare_timestamps(page_filename) <= 0) {
            // This one is out of date.
            all_ok = false;
            break;
          }
        }

        if (has_alpha) {
          Filename page_alpha_filename = alpha_pattern.get_filename_index(i);
          if (page_alpha_filename.resolve_filename(path)) {
            if (_output_filename.compare_timestamps(page_alpha_filename) <= 0) {
              // This one is out of date.
              all_ok = false;
              break;
            }
          }
        }
      }

      needs_rewrite = !all_ok;
    }
  }

  if (!needs_rewrite) {
    nout << "Output texture is up-to-date.\n";
    return true;
  }

  PT(Texture) tex = new Texture(_input_filename.get_basename_wo_extension());
  if (!tex->read_ptex(ptex)) {
    std::cerr << "Could not read the input texture into the texture object!\n";
    return false;
  }

  std::ostream &out = get_output();
  if (!tex->write_txo(out, get_output_filename())) {
    std::cerr << "Could not write the texture object!\n";
    return false;
  }
  close_output();

  return true;
}

/**
 *
 */
bool PTexToTxo::
handle_args(Args &args) {
  if (args.empty()) {
    nout << "You didn't specify an input texture!\n";
    return false;
  }

  _input_filename = args[0];

  return true;
}

/**
 *
 */
int
main(int argc, char *argv[]) {
  PTexToTxo prog;
  prog.parse_command_line(argc, argv);
  return prog.run() ? 0 : 1;
}

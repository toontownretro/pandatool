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
#include "pdxValue.h"
#include "virtualFileSystem.h"
#include "config_gobj.h"

/**
 *
 */
PTexToTxo::
PTexToTxo() :
  ProgramBase("ptex2txo"),
  WithOutputFile(true, false, true) {

  // Need actual image data here.
  textures_header_only = false;

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

  VirtualFileSystem *vfs = VirtualFileSystem::get_global_ptr();
  Filename fullpath = _input_filename;
  if (!vfs->resolve_filename(fullpath, path)) {
    std::cout << "Could not find input texture\n";
    return false;
  }

  PDXValue val;
  if (!val.read(fullpath, path)) {
    std::cerr << "Could not load the input texture!\n";
    return false;
  }

  PDXElement *elem = val.get_element();
  if (elem == nullptr) {
    std::cerr << "Expected PDXElement at root of texture\n";
    return false;
  }

  // Append the dirname of the loaded ptex file to the search path.
  path.append_directory(fullpath.get_dirname());

  PTexture ptex;
  ptex.local_object();
  if (!ptex.load(elem, path)) {
    std::cerr << "Failed to read file into ptex structure\n";
    return false;
  }

  PT(Texture) tex = new Texture(_input_filename.get_basename_wo_extension());
  if (!tex->read_ptex(elem, fullpath)) {
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

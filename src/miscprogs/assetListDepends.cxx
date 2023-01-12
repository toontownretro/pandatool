/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file assetListDepends.cxx
 * @author brian
 * @date 2021-03-05
 */

#include "assetListDepends.h"
#include "assetRegistry.h"
#include "assetBase.h"
#include "executionEnvironment.h"
#include "config_gobj.h"
#include "config_egg2pg.h"

/**
 *
 */
AssetListDepends::
AssetListDepends() :
  ProgramBase("asset-list-depends")
{
  set_program_brief("lists asset dependencies");
  set_program_description
    ("This is a small program that loads an asset file and outputs the "
     "asset's list of dependencies to stdout.  The dependencies are printed "
     "relative to the current working directory.");

  add_runline("input");
}

/**
 *
 */
void AssetListDepends::
run() {
  AssetRegistry *reg = AssetRegistry::get_global_ptr();

  DSearchPath search = get_model_path();
  search.append_directory(".");

  PT(AssetBase) asset = reg->load(_input_filename, search);
  if (asset == nullptr) {
    nout << "Failed to load asset file " << _input_filename << "\n";
    exit(1);
  }

  vector_string dependencies;
  asset->get_dependencies(dependencies);

  for (const std::string &dep : dependencies) {
    Filename fdep = dep;
    fdep.make_canonical();
    fdep.make_relative_to(ExecutionEnvironment::get_cwd());
    std::cout << fdep.get_fullpath() << " ";
  }
}

/**
 *
 */
bool AssetListDepends::
handle_args(Args &args) {
  if (args.size() != 1) {
    nout << "You must specify exactly one input file to read on the command line.\n";
    return false;
  }

  _input_filename = Filename::text_filename(args[0]);
  return true;
}

int
main(int argc, char *argv[]) {
  init_libgobj();
  init_libegg2pg();

  AssetListDepends prog;
  prog.parse_command_line(argc, argv);
  prog.run();

  return 0;
}

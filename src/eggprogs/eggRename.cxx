// Filename: eggRename.cxx
// Created by:  masad (22Apr05)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#include "eggRename.h"

////////////////////////////////////////////////////////////////////
//     Function: EggRename::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
EggRename::
EggRename() {
  set_program_description
    ("egg-rename reads one or more egg files and writes back with modified"
     "node names. ie. suppressing prefix from all the nodes' names. ");

  add_option
    ("strip_prefix", "name", 0,
     "strips out the prefix that is put on all nodes, by maya ext. ref",
     &EggRename::dispatch_vector_string, NULL, &_strip_prefix);
}

////////////////////////////////////////////////////////////////////
//     Function: EggRename::run
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void EggRename::
run() {
  if (!_strip_prefix.empty()) {
    nout << "Stripping prefix from nodes.\n";
    int num_renamed = 0;
    int num_egg_files = 0;
    Eggs::iterator ei;
    for (ei = _eggs.begin(); ei != _eggs.end(); ++ei) {
      num_renamed += (*ei)->rename_nodes(_strip_prefix, true);
      ++num_egg_files;
    }
    nout << "  (" << num_renamed << " renamed.)\n";
  }

  write_eggs();
}


int main(int argc, char *argv[]) {
  EggRename prog;
  prog.parse_command_line(argc, argv);
  prog.run();
  return 0;
}
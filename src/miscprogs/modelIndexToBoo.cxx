/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file modelIndexToBoo.cxx
 * @author lachbr
 * @date 2021-03-05
 */

#include "modelIndexToBoo.h"
#include "modelIndex.h"

/**
 *
 */
ModelIndexToBoo::
ModelIndexToBoo() :
  ProgramBase("modindex2boo"),
  WithOutputFile(true, false, true)
{
  set_program_brief("compile a model tree index file into a .boo file");
  set_program_description
    ("This is a tiny little program that compiles a model tree index file, "
     "generated by the ppremake build system, into a binary format that is "
     "quick for tools to load up."
     "\n\n"
     "The reasoning behind this is that when a model tree is very large with "
     "possibly thousands of assets, it takes quite a bit of time for tools "
     "like pmat2rso or egg2bam to load up the text version of the index file, "
     "and it severely slows down the build process.");

  add_runline("input output.boo");
  add_runline("-o output.boo input");

  add_option
    ("o", "filename", 0,
     "Specify the filename that the output .boo file should be written to.",
     &ModelIndexToBoo::dispatch_filename, &_got_output_filename, &_output_filename);
}

/**
 *
 */
void ModelIndexToBoo::
run() {
  ModelIndex *index = ModelIndex::get_global_ptr();

  if (!index->read_index(_input_filename)) {
    nout << "Failed to read input index file.\n";
    exit(1);
  }

  nout << "Writing binary model index " << _output_filename << "\n";

  if (!index->write_boo_index(index->get_num_trees() - 1, _output_filename)) {
    nout << "Failed to write binary index file.\n";
    exit(1);
  }
}

/**
 *
 */
bool ModelIndexToBoo::
handle_args(Args &args) {
  if (args.size() == 2 && !_got_output_filename) {
    // The second argument, if present, is implicitly the output file.
    _got_output_filename = true;
    _output_filename = args[1];
    args.pop_back();
  }

  if (args.size() != 1) {
    nout << "You must specify exactly one input file to read on the command line.\n";
    return false;
  }

  _input_filename = Filename::text_filename(args[0]);
  return true;
}

int
main(int argc, char *argv[]) {
  ModelIndexToBoo prog;
  prog.parse_command_line(argc, argv);
  prog.run();

  return 0;
}

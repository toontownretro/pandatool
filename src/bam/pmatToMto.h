/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file pmatToMto.h
 * @author brian
 * @date 2021-02-05
 */

#ifndef PMATTOMTO_H
#define PMATTOMTO_H

#include "programBase.h"
#include "withOutputFile.h"

/**
 * Program that compiles a source material file into a binary material object
 * file.
 */
class PMatToMto : public ProgramBase, public WithOutputFile {
public:
  PMatToMto();

  bool run();

protected:
  virtual bool handle_args(Args &args);

private:
  Filename _input_filename;

  bool _got_index_filename;
  Filename _index_filename;
};

#endif // PMATTOMTO_H

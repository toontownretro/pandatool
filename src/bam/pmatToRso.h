/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file pmatToRso.h
 * @author lachbr
 * @date 2021-02-05
 */

#ifndef PMATTORSO_H
#define PMATTORSO_H

#include "programBase.h"
#include "withOutputFile.h"

/**
 * Program that compiles a source material file into a binary render state
 * file.
 */
class PMatToRso : public ProgramBase, public WithOutputFile {
public:
  PMatToRso();

  bool run();

protected:
  virtual bool handle_args(Args &args);

private:
  Filename _input_filename;
};

#endif // PMATTORSO_H

/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file ptexToTxo.h
 * @author brian
 * @date 2021-02-05
 */

#ifndef PTEXTOTXO_H
#define PTEXTOTXO_H

#include "programBase.h"
#include "withOutputFile.h"

/**
 * Program that converts a .ptex file to a .txo file.
 */
class PTexToTxo : public ProgramBase, public WithOutputFile {
public:
  PTexToTxo();

  bool run();

protected:
  virtual bool handle_args(Args &args);

private:
  bool _tex_ctex;
  Filename _input_filename;
};

#endif // PTEXTOTXO_H

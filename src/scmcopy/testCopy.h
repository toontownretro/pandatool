/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file testCopy.h
 * @author drose
 * @date 2000-10-31
 */

#ifndef TESTCOPY_H
#define TESTCOPY_H

#include "pandatoolbase.h"

#include "scmCopy.h"

/**
 * A program to copy ordinary files into a SCM hierarchy.  Mainly to test
 * SCMCopy.
 */
class TestCopy : public SCMCopy {
public:
  TestCopy();

  void run();

protected:
  virtual bool copy_file(const Filename &source, const Filename &dest,
                         SCMSourceDirectory *dir, void *extra_data,
                         bool new_file);
};

#endif

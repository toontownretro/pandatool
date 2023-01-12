/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file assetListDepends.h
 * @author brian
 * @date 2021-03-05
 */

#ifndef ASSETLISTDEPENDS_H
#define ASSETLISTDEPENDS_H

#include "programBase.h"

/**
 * Program that loads up a given asset file and outputs the set of filenames
 * the asset depends on.
 */
class AssetListDepends : public ProgramBase {
public:
  AssetListDepends();

  void run();

protected:
  virtual bool handle_args(Args &args);

  Filename _input_filename;
};

#endif // ASSETLISTDEPENDS_H

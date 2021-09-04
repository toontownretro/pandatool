/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file eggStripMotion.h
 * @author brian
 * @date 2021-04-28
 */

#ifndef EGGSTRIPMOTION_H
#define EGGSTRIPMOTION_H

#include "eggFilter.h"

/**
 * Program that strips part or all animation from a particular joint.  Can be
 * used to make a movement animation become stationary for use in-game, for
 * example.
 */
class EggStripMotion : public EggFilter {
public:
  EggStripMotion();

  bool run();

  std::string _joint;
  std::string _components;
};

#endif // EGGSTRIPMOTION_H

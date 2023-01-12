/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file pmdlToBam.h
 * @author brian
 * @date 2021-09-13
 */

#ifndef PMDLTOBAM_H
#define PMDLTOBAM_H

#include "programBase.h"
#include "withOutputFile.h"
#include "texture.h"
#include "material.h"
#include "pset.h"

class PandaNode;

/**
 * Program that converts a .pmdl model file to a .bam file.
 */
class PmdlToBam : public ProgramBase, public WithOutputFile {
public:
  PmdlToBam();

  bool run();

protected:
  virtual bool handle_args(Args &args);

private:
  void collect_materials(PandaNode *node);

private:
  // This is the set of all materials referenced by the egg file that need to
  // remapped to their installed counterpart.
  typedef pset<Material *> Materials;
  Materials _materials;

  // We might also have textures applied directly onto the RenderState, and we
  // also need to remap those.
  typedef pset<Texture *> Textures;
  Textures _textures;

  Filename _input_filename;

  bool _got_index_filename;
  Filename _index_filename;
};

#endif // PMDLTOBAM_H

/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file materialFile.h
 * @author lachbr
 * @date 2020-01-14
 */

#ifndef MATERIALFILE_H
#define MATERIALFILE_H

#include "config_palettizer.h"
#include "typedWritable.h"
#include "namable.h"
#include "material.h"
#include "textureReference.h"

/**
 * This represents a single material file known to the palettizer.  It may
 * be referenced by one or more egg files, or listed directly by the
 * textures.txa file.  If the material was listed directly in the textures.txa
 * file, all of the textures referenced by the material are omitted.
 */
class MaterialFile : public TypedWritable, public Namable {

private:
  PT(Material) _material;

  typedef pvector<TextureReference *> Textures;
  Textures _textures;
};

#endif // MATERIALFILE_H

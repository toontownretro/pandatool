/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file eggTextureCards.h
 * @author drose
 * @date 2001-02-21
 */

#ifndef EGGTEXTURECARDS_H
#define EGGTEXTURECARDS_H

#include "pandatoolbase.h"

#include "eggWriter.h"
#include "luse.h"
#include "vector_string.h"

class EggVertexPool;
class EggVertex;

/**
 * Generates an egg file featuring a number of polygons, one for each named
 * texture.  This is a support program for getting textures through egg-
 * palettize.
 */
class EggTextureCards : public EggWriter {
public:
  EggTextureCards();

protected:
  virtual bool handle_args(Args &args);

private:
  bool scan_texture(const Filename &filename, LVecBase4d &geometry);
  void make_vertices(const LPoint4d &geometry, EggVertexPool *vpool,
                     EggVertex *&v1, EggVertex *&v2, EggVertex *&v3, EggVertex *&v4);

public:
  void run();

  LVecBase4d _polygon_geometry;
  LVecBase2d _pixel_scale;
  bool _got_pixel_scale;
  vector_string _suffixes;
  LColor _polygon_color;
  vector_string _texture_names;
  double _frame_rate;
  bool _noexist;
};

#endif

// Filename: eggTextureCards.h
// Created by:  drose (21Feb01)
// 
////////////////////////////////////////////////////////////////////

#ifndef EGGTEXTURECARDS_H
#define EGGTEXTURECARDS_H

#include <pandatoolbase.h>

#include <eggWriter.h>
#include <luse.h>

////////////////////////////////////////////////////////////////////
// 	 Class : EggTextureCards
// Description : Generates an egg file featuring a number of polygons,
//               one for each named texture.  This is a support
//               program for getting textures through egg-palettize.
////////////////////////////////////////////////////////////////////
class EggTextureCards : public EggWriter {
public:
  EggTextureCards();

protected:
  virtual bool handle_args(Args &args);

public:
  void run();

  LVecBase4d _polygon_geometry;
  Colorf _polygon_color;
  vector_string _texture_names;
};

#endif

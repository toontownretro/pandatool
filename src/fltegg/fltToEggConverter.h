// Filename: fltToEggConverter.h
// Created by:  drose (17Apr01)
// 
////////////////////////////////////////////////////////////////////

#ifndef FLTTOEGGCONVERTER_H
#define FLTTOEGGCONVERTER_H

#include <pandatoolbase.h>

#include "fltToEggLevelState.h"

#include <somethingToEggConverter.h>
#include <fltHeader.h>
#include <eggVertex.h>
#include <eggVertexPool.h>
#include <eggTexture.h>
#include <pointerTo.h>

class FltRecord;
class FltLOD;
class FltGroup;
class FltObject;
class FltBeadID;
class FltBead;
class FltVertex;
class FltGeometry;
class FltFace;
class FltExternalReference;
class FltTexture;
class EggGroupNode;
class EggPrimitive;

////////////////////////////////////////////////////////////////////
// 	 Class : FltToEggConverter
// Description : This class supervises the construction of an EggData
//               structure from the data represented by the FltHeader.
//               Reading and writing the egg and flt structures is
//               left to the user.
////////////////////////////////////////////////////////////////////
class FltToEggConverter : public SomethingToEggConverter {
public:
  FltToEggConverter();
  FltToEggConverter(const FltToEggConverter &copy);
  ~FltToEggConverter();

  virtual SomethingToEggConverter *make_copy();

  virtual string get_name() const;
  virtual string get_extension() const;

  virtual bool convert_file(const Filename &filename);
  bool convert_flt(const FltHeader *flt_header);

private:
  void cleanup();

  typedef vector< PT(EggVertex) > EggVertices;

  void convert_record(const FltRecord *flt_record, FltToEggLevelState &state);
  void dispatch_record(const FltRecord *flt_record, FltToEggLevelState &state);
  void convert_lod(const FltLOD *flt_lod, FltToEggLevelState &state);
  void convert_group(const FltGroup *flt_group, FltToEggLevelState &state);
  void convert_object(const FltObject *flt_object, FltToEggLevelState &state);
  void convert_bead_id(const FltBeadID *flt_bead, FltToEggLevelState &state);
  void convert_bead(const FltBead *flt_bead, FltToEggLevelState &state);
  void convert_face(const FltFace *flt_face, FltToEggLevelState &state);
  void convert_ext_ref(const FltExternalReference *flt_ext, FltToEggLevelState &state);
		    
  void setup_geometry(const FltGeometry *flt_geom, FltToEggLevelState &state,
		      EggPrimitive *egg_prim, EggVertexPool *egg_vpool,
		      const EggVertices &vertices);

  void convert_subfaces(const FltRecord *flt_record, FltToEggLevelState &state);

  void set_transform(const FltBead *flt_bead, EggGroup *egg_group);
  bool parse_comment(const FltBeadID *flt_bead, EggNode *egg_node);
  bool parse_comment(const FltBead *flt_bead, EggNode *egg_node);
  bool parse_comment(const FltTexture *flt_texture, EggNode *egg_node);
  bool parse_comment(const string &comment, const string &name,
		     EggNode *egg_node);

  PT(EggVertex) make_egg_vertex(const FltVertex *flt_vertex);
  PT(EggTexture) make_egg_texture(const FltTexture *flt_texture);

  CPT(FltHeader) _flt_header;

  PT(EggVertexPool) _main_egg_vpool;

  typedef map<const FltTexture *, PT(EggTexture) > Textures;
  Textures _textures;
};

#include "fltToEggConverter.I"

#endif


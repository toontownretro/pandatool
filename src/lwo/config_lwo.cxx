// Filename: config_lwo.cxx
// Created by:  drose (23Apr01)
// 
////////////////////////////////////////////////////////////////////

#include "config_lwo.h"
#include "iffChunk.h"
#include "iffGenericChunk.h"
#include "iffInputFile.h"
#include "lwoBoundingBox.h"
#include "lwoChunk.h"
#include "lwoClip.h"
#include "lwoDiscontinuousVertexMap.h"
#include "lwoGroupChunk.h"
#include "lwoHeader.h"
#include "lwoInputFile.h"
#include "lwoLayer.h"
#include "lwoPoints.h"
#include "lwoPolygons.h"
#include "lwoPolygonTags.h"
#include "lwoStillImage.h"
#include "lwoSurface.h"
#include "lwoSurfaceColor.h"
#include "lwoSurfaceParameter.h"
#include "lwoTags.h"
#include "lwoVertexMap.h"

#include <dconfig.h>

Configure(config_lwo);

ConfigureFn(config_lwo) {
  IffChunk::init_type();
  IffGenericChunk::init_type();
  IffInputFile::init_type();
  LwoBoundingBox::init_type();
  LwoChunk::init_type();
  LwoClip::init_type();
  LwoDiscontinuousVertexMap::init_type();
  LwoGroupChunk::init_type();
  LwoHeader::init_type();
  LwoInputFile::init_type();
  LwoLayer::init_type();
  LwoPoints::init_type();
  LwoPolygons::init_type();
  LwoPolygonTags::init_type();
  LwoTags::init_type();
  LwoStillImage::init_type();
  LwoSurface::init_type();
  LwoSurfaceColor::init_type();
  LwoSurfaceParameter::init_type();
  LwoVertexMap::init_type();
}


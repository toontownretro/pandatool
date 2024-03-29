#define LOCAL_LIBS pandatoolbase
#define OTHER_LIBS \
  mathutil:c linmath:c putil:c pipeline:c event:c \
  panda:m \
   express:c pandaexpress:m \
  interrogatedb prc  \
  dtoolutil:c dtoolbase:c dtool:m
#define UNIX_SYS_LIBS m

#begin ss_lib_target
  #define TARGET lwo

  #define SOURCES \
     config_lwo.h iffChunk.I iffChunk.h iffGenericChunk.I  \
     iffGenericChunk.h iffId.I iffId.h iffInputFile.I  \
     iffInputFile.h lwoBoundingBox.h lwoChunk.h lwoClip.h  \
     lwoDiscontinuousVertexMap.h lwoGroupChunk.h lwoHeader.I  \
     lwoHeader.h lwoInputFile.I lwoInputFile.h lwoLayer.h  \
     lwoPoints.h lwoPolygons.h lwoPolygonTags.h lwoTags.h  \
     lwoStillImage.h lwoSurface.h lwoSurfaceBlock.h  \
     lwoSurfaceBlockAxis.h lwoSurfaceBlockChannel.h  \
     lwoSurfaceBlockCoordSys.h lwoSurfaceBlockEnabled.h  \
     lwoSurfaceBlockImage.h lwoSurfaceBlockOpacity.h  \
     lwoSurfaceBlockProjection.h lwoSurfaceBlockHeader.h  \
     lwoSurfaceBlockRefObj.h lwoSurfaceBlockRepeat.h  \
     lwoSurfaceBlockTMap.h lwoSurfaceBlockTransform.h  \
     lwoSurfaceBlockVMapName.h lwoSurfaceBlockWrap.h  \
     lwoSurfaceColor.h lwoSurfaceParameter.h  \
     lwoSurfaceSidedness.h lwoSurfaceSmoothingAngle.h  \
     lwoVertexMap.h

  #define COMPOSITE_SOURCES \
     config_lwo.cxx iffChunk.cxx iffGenericChunk.cxx iffId.cxx  \
     iffInputFile.cxx lwoBoundingBox.cxx lwoChunk.cxx lwoClip.cxx  \
     lwoDiscontinuousVertexMap.cxx lwoGroupChunk.cxx  \
     lwoHeader.cxx lwoInputFile.cxx lwoLayer.cxx lwoPoints.cxx  \
     lwoPolygons.cxx lwoPolygonTags.cxx lwoTags.cxx  \
     lwoStillImage.cxx lwoSurface.cxx lwoSurfaceBlock.cxx  \
     lwoSurfaceBlockAxis.cxx lwoSurfaceBlockChannel.cxx  \
     lwoSurfaceBlockCoordSys.cxx lwoSurfaceBlockEnabled.cxx  \
     lwoSurfaceBlockImage.cxx lwoSurfaceBlockOpacity.cxx  \
     lwoSurfaceBlockProjection.cxx lwoSurfaceBlockHeader.cxx  \
     lwoSurfaceBlockRefObj.cxx lwoSurfaceBlockRepeat.cxx  \
     lwoSurfaceBlockTMap.cxx lwoSurfaceBlockTransform.cxx  \
     lwoSurfaceBlockVMapName.cxx lwoSurfaceBlockWrap.cxx  \
     lwoSurfaceColor.cxx lwoSurfaceParameter.cxx  \
     lwoSurfaceSidedness.cxx lwoSurfaceSmoothingAngle.cxx  \
     lwoVertexMap.cxx

  #define INSTALL_HEADERS \
    iffChunk.I iffChunk.h iffGenericChunk.I iffGenericChunk.h iffId.I \
    iffId.h iffInputFile.I iffInputFile.h \
    lwoBoundingBox.h \
    lwoChunk.h \
    lwoClip.h \
    lwoDiscontinuousVertexMap.h \
    lwoGroupChunk.h \
    lwoHeader.I lwoHeader.h \
    lwoInputFile.I lwoInputFile.h \
    lwoLayer.h \
    lwoPoints.h \
    lwoPolygons.h \
    lwoPolygonTags.h \
    lwoTags.h \
    lwoStillImage.h \
    lwoSurface.h \
    lwoSurfaceBlock.h \
    lwoSurfaceBlockAxis.h \
    lwoSurfaceBlockChannel.h \
    lwoSurfaceBlockCoordSys.h \
    lwoSurfaceBlockEnabled.h \
    lwoSurfaceBlockImage.h \
    lwoSurfaceBlockOpacity.h \
    lwoSurfaceBlockProjection.h \
    lwoSurfaceBlockHeader.h \
    lwoSurfaceBlockRefObj.h \
    lwoSurfaceBlockRepeat.h \
    lwoSurfaceBlockTMap.h \
    lwoSurfaceBlockTransform.h \
    lwoSurfaceBlockVMapName.h \
    lwoSurfaceBlockWrap.h \
    lwoSurfaceColor.h \
    lwoSurfaceParameter.h \
    lwoSurfaceSidedness.h \
    lwoSurfaceSmoothingAngle.h \
    lwoVertexMap.h

#end ss_lib_target

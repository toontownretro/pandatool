#begin ss_lib_target
  #define TARGET lwoegg
  #define LOCAL_LIBS converter lwo pandatoolbase
  #define OTHER_LIBS \
    egg:c pandaegg:m \
    mathutil:c linmath:c putil:c pipeline:c event:c \
    panda:m \
    pandabase:c express:c pandaexpress:m \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m
  #define UNIX_SYS_LIBS m

  #define SOURCES \
     cLwoClip.I cLwoClip.h cLwoLayer.I cLwoLayer.h cLwoPoints.I  \
     cLwoPoints.h cLwoPolygons.I cLwoPolygons.h cLwoSurface.I  \
     cLwoSurface.h cLwoSurfaceBlock.I cLwoSurfaceBlock.h  \
     cLwoSurfaceBlockTMap.I cLwoSurfaceBlockTMap.h  \
     lwoToEggConverter.I lwoToEggConverter.h

  #define COMPOSITE_SOURCES \
     cLwoClip.cxx cLwoLayer.cxx cLwoPoints.cxx cLwoPolygons.cxx  \
     cLwoSurface.cxx cLwoSurfaceBlock.cxx  \
     cLwoSurfaceBlockTMap.cxx lwoToEggConverter.cxx  \
     lwoToEggConverter.h

  #define INSTALL_HEADERS \
    lwoToEggConverter.I lwoToEggConverter.h

#end ss_lib_target

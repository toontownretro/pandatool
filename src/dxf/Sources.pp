#begin ss_lib_target
  #define TARGET dxf
  #define LOCAL_LIBS pandatoolbase

  #define OTHER_LIBS \
    pipeline:c event:c putil:c mathutil:c linmath:c \
    panda:m \
     express:c pandaexpress:m \
    dtoolutil:c prc interrogatedb dtoolbase:c dtool:m

  #define SOURCES \
    dxfFile.cxx dxfFile.h dxfLayer.h dxfLayer.cxx \
    dxfLayerMap.cxx dxfLayerMap.h \
    dxfVertex.cxx dxfVertex.h

  #define INSTALL_HEADERS \
    dxfFile.h dxfLayer.h dxfLayerMap.h dxfVertex.h

#end ss_lib_target

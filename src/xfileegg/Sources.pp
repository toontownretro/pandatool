#begin ss_lib_target
  #define TARGET xfileegg
  #define LOCAL_LIBS xfile eggbase progbase pandatoolbase
  #define OTHER_LIBS \
    egg:c pandaegg:m \
    mathutil:c linmath:c putil:c pipeline:c event:c \
    pnmimage:c \
    panda:m \
     express:c pandaexpress:m \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m

  #define SOURCES \
     xFileAnimationSet.h xFileAnimationSet.I \
     xFileFace.h xFileMaker.h xFileMaterial.h \
     xFileMesh.h xFileNormal.h \
     xFileToEggConverter.h xFileVertex.h

  #define COMPOSITE_SOURCES \
     xFileAnimationSet.cxx \
     xFileFace.cxx xFileMaker.cxx xFileMaterial.cxx \
     xFileMesh.cxx xFileNormal.cxx \
     xFileToEggConverter.cxx xFileVertex.cxx

#end ss_lib_target

#define BUILD_DIRECTORY $[HAVE_MAYA]

#begin ss_lib_target
  #define USE_PACKAGES maya
  #define TARGET mayaegg
  #define LOCAL_LIBS \
    mayabase converter pandatoolbase
  #define OTHER_LIBS \
    egg:c pandaegg:m \
    linmath:c putil:c panda:m \
    express:c pandaexpress:m \
    dtoolutil:c dtoolbase:c prc  dtool:m \
    pipeline:c interrogatedb gobj:c

  #define UNIX_SYS_LIBS \
    m

  #define SOURCES \
    config_mayaegg.cxx config_mayaegg.h \
    mayaEggGroupUserData.cxx mayaEggGroupUserData.I mayaEggGroupUserData.h \
    mayaEggLoader.cxx mayaEggLoader.h \
    mayaBlendDesc.cxx mayaBlendDesc.h \
    mayaNodeDesc.cxx mayaNodeDesc.h \
    mayaNodeTree.cxx mayaNodeTree.h \
    mayaToEggConverter.cxx mayaToEggConverter.h

#end ss_lib_target

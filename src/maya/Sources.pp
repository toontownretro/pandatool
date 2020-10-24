#define BUILD_DIRECTORY $[HAVE_MAYA]

#begin ss_lib_target
  #define USE_PACKAGES maya
  #define TARGET mayabase
  #define LOCAL_LIBS \
    converter pandatoolbase
  #define OTHER_LIBS \
    putil:c panda:m \
    express:c pandaexpress:m \
    dtoolutil:c dtoolbase:c prc  dtool:m \
    pipeline:c interrogatedb

  #define SOURCES \
    config_maya.h \
    mayaApi.h \
    mayaShader.h \
    mayaShaderColorDef.h \
    mayaShaders.h \
    maya_funcs.I maya_funcs.h \
    post_maya_include.h pre_maya_include.h

  #define COMPOSITE_SOURCES \
    config_maya.cxx \
    mayaApi.cxx \
    mayaShader.cxx \
    mayaShaderColorDef.cxx \
    mayaShaders.cxx \
    maya_funcs.cxx

#end ss_lib_target

#define BUILD_DIRECTORY $[HAVE_GLSLANG]

#define USE_PACKAGES glslang spirv_tools

#define LOCAL_LIBS pandatoolbase progbase
#define OTHER_LIBS shader:c gobj:c putil:c pgraph:c panda:m

#begin bin_target
  #define TARGET shadercompile

  #define SOURCES \
    shaderCompile.h

  #define COMPOSITE_SOURCES \
    shaderCompile.cxx

#end bin_target

#define OTHER_LIBS \
   p3pipeline:c panda:m \
   p3parametrics:c p3collide:c p3chan:c \
   p3char:c p3gsgbase:c p3gobj:c p3pgraphnodes:c \
   p3tform:c p3dgraph:c p3text:c p3movies:c \
   p3audio:c p3cull:c p3device:c \
   p3pnmtext:c \
   p3express:c p3putil:c p3pandabase:c pandaexpress:m \
   p3interrogatedb p3prc  \
   p3dtoolutil:c p3dtoolbase:c p3dtool:m

#define USE_PACKAGES threads

#begin ss_lib_target
  #define TARGET p3pandatoolbase

  #define SOURCES \
    animationConvert.cxx animationConvert.h \
    config_pandatoolbase.cxx config_pandatoolbase.h \
    distanceUnit.cxx distanceUnit.h \
    pandatoolbase.cxx pandatoolbase.h pandatoolsymbols.h \
    pathReplace.cxx pathReplace.I pathReplace.h \
    pathStore.cxx pathStore.h

  #define INSTALL_HEADERS \
    animationConvert.h \
    config_pandatoolbase.h \
    distanceUnit.h \
    pandatoolbase.h pandatoolsymbols.h \
    pathReplace.I pathReplace.h \
    pathStore.h

#end ss_lib_target

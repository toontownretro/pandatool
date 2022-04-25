#define OTHER_LIBS \
   pipeline:c panda:m \
   parametrics:c collide:c anim:c \
   gsgbase:c gobj:c pgraphnodes:c \
   tform:c dgraph:c text:c movies:c \
   audio:c cull:c device:c \
   pnmtext:c \
   express:c putil:c  pandaexpress:m \
   interrogatedb prc  \
   dtoolutil:c dtoolbase:c dtool:m

#define USE_PACKAGES threads eigen sleef

#begin ss_lib_target
  #define TARGET pandatoolbase

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

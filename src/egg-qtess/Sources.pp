#begin bin_target
  #define TARGET egg-qtess
  #define LOCAL_LIBS \
    eggbase progbase
  #define OTHER_LIBS \
    egg2pg:c egg:c pandaegg:m \
    anim:c downloader:c event:c \
    tform:c grutil:c text:c dgraph:c display:c gsgbase:c \
    collide:c gobj:c cull:c device:c \
    parametrics:c pgraph:c pgraphnodes:c pipeline:c pstatclient:c \
    pnmimagetypes:c pnmimage:c mathutil:c linmath:c putil:c \
    movies:c \
    $[if $[HAVE_FREETYPE],pnmtext:c] \
    $[if $[HAVE_NET],net:c] $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[HAVE_AUDIO],audio:c] \
    panda:m \
     express:c pandaexpress:m \
    interrogatedb dtoolutil:c dtoolbase:c prc  dtool:m
  #define UNIX_SYS_LIBS m

  #define SOURCES \
     config_egg_qtess.h \
     eggQtess.h \
     isoPlacer.I isoPlacer.h \
     qtessGlobals.h \
     qtessInputEntry.I qtessInputEntry.h \
     qtessInputFile.I qtessInputFile.h \
     qtessSurface.I qtessSurface.h \
     subdivSegment.I subdivSegment.h

  #define COMPOSITE_SOURCES \
     config_egg_qtess.cxx \
     eggQtess.cxx \
     isoPlacer.cxx \
     qtessGlobals.cxx \
     qtessInputEntry.cxx \
     qtessInputFile.cxx \
     qtessSurface.cxx \
     subdivSegment.cxx

#end bin_target

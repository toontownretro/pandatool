#define BUILD_DIRECTORY $[HAVE_FREETYPE]

#define USE_PACKAGES freetype png

#define LOCAL_LIBS \
  palettizer eggbase progbase

#define OTHER_LIBS \
    egg:c pandaegg:m \
    pipeline:c event:c pstatclient:c panda:m \
    parametrics:c gsgbase:c gobj:c \
     pnmimage:c pnmimagetypes:c pnmtext:c \
    mathutil:c linmath:c putil:c express:c \
    pandaexpress:m \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

#begin bin_target
  #define TARGET egg-mkfont

  #defer SOURCES \
    eggMakeFont.h \
    rangeDescription.h rangeDescription.I \
    rangeIterator.h rangeIterator.I

  #define COMPOSITE_SOURCES \
    eggMakeFont.cxx \
    rangeDescription.cxx \
    rangeIterator.cxx

#end bin_target

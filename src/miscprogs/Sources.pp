#define LOCAL_LIBS \
  progbase

#define OTHER_LIBS \
    pipeline:c event:c pstatclient:c panda:m \
    pandabase:c pnmimage:c mathutil:c linmath:c putil:c express:c \
    pandaexpress:m \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

#define UNIX_SYS_LIBS m

#begin bin_target
  #define TARGET bin2c

  #define SOURCES \
    binToC.cxx binToC.h

#end bin_target

#begin bin_target
  #define TARGET modindex2boo

  #define SOURCES \
    modelIndexToBoo.cxx modelIndexToBoo.h
#end bin_target

#begin bin_target
  #define TARGET asset-list-depends
  #define OTHER_LIBS \
    $[OTHER_LIBS] egg2pg:c gobj:c

  #define SOURCES \
    assetListDepends.cxx assetListDepends.h

#end bin_target

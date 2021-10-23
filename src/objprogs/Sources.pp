#define UNIX_SYS_LIBS m

#define OTHER_LIBS \
    egg:c egg2pg:c pandaegg:m \
    pipeline:c event:c pstatclient:c panda:m \
     pnmimage:c mathutil:c linmath:c putil:c express:c \
    grutil:c text:c movies:c audio:c cull:c device:c \
    pnmtext:c \
    pandaexpress:m \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

#begin bin_target
  #define TARGET obj2egg
  #define LOCAL_LIBS objegg eggbase progbase

  #define SOURCES \
    objToEgg.cxx objToEgg.h

#end bin_target

#begin bin_target
  #define TARGET egg2obj
  #define LOCAL_LIBS objegg eggbase progbase

  #define SOURCES \
    eggToObj.cxx eggToObj.h

#end bin_target

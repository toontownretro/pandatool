#begin ss_lib_target
  #define TARGET objegg
  #define LOCAL_LIBS converter pandatoolbase
  #define OTHER_LIBS \
    egg:c pandaegg:m \
    pipeline:c event:c pstatclient:c panda:m \
    pandabase:c pnmimage:c mathutil:c linmath:c putil:c express:c \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

  #define UNIX_SYS_LIBS \
    m

  #define SOURCES \
    config_objegg.cxx config_objegg.h \
    objToEggConverter.cxx objToEggConverter.h objToEggConverter.I \
    eggToObjConverter.cxx eggToObjConverter.h

  #define INSTALL_HEADERS \
    objToEggConverter.h objToEggConverter.I \
    eggToObjConverter.h

#end ss_lib_target

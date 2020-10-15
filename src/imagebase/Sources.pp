#begin ss_lib_target
  #define TARGET imagebase
  #define LOCAL_LIBS \
    progbase

  #define OTHER_LIBS \
    pipeline:c event:c pstatclient:c panda:m \
    pandabase:c pnmimage:c mathutil:c linmath:c putil:c express:c \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

  #define SOURCES \
    imageReader.h imageWriter.I imageWriter.h \
    imageBase.h imageFilter.h

  #define COMPOSITE_SOURCES \
    imageBase.cxx imageFilter.cxx \
    imageReader.cxx imageWriter.cxx

  #define INSTALL_HEADERS \
    imageBase.h imageFilter.h imageReader.h imageWriter.I imageWriter.h

#end ss_lib_target

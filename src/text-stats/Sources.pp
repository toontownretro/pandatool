#define BUILD_DIRECTORY $[HAVE_NET]

#begin bin_target
  #define TARGET text-stats
  #define LOCAL_LIBS \
    progbase pstatserver
  #define OTHER_LIBS \
    pstatclient:c linmath:c putil:c pipeline:c event:c \
    pnmimage:c mathutil:c \
    downloader:c $[if $[HAVE_NET],net:c] $[if $[WANT_NATIVE_NET],nativenet:c] \
    panda:m \
    pandabase:c express:c pandaexpress:m \
    interrogatedb dtoolutil:c dtoolbase:c prc  dtool:m

  #define SOURCES \
    textMonitor.cxx textMonitor.h textMonitor.I \
    textStats.cxx textStats.h

  #define INSTALL_HEADERS

#end bin_target

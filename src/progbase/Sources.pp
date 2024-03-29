#define USE_PACKAGES zlib

#begin ss_lib_target
  #define TARGET progbase
  #define LOCAL_LIBS \
    pandatoolbase
  #define OTHER_LIBS \
    pipeline:c event:c pstatclient:c panda:m \
     pnmimage:c mathutil:c linmath:c putil:c express:c \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

  #define SOURCES \
    programBase.I programBase.h \
    withOutputFile.I withOutputFile.h \
    wordWrapStream.h wordWrapStreamBuf.I \
    wordWrapStreamBuf.h

  #define COMPOSITE_SOURCES \
    programBase.cxx withOutputFile.cxx wordWrapStream.cxx \
    wordWrapStreamBuf.cxx

  #define INSTALL_HEADERS \
    programBase.I programBase.h \
    withOutputFile.I withOutputFile.h \
    wordWrapStream.h wordWrapStreamBuf.I \
    wordWrapStreamBuf.h

#end ss_lib_target

#begin test_bin_target
  #define TARGET test_prog
  #define LOCAL_LIBS \
    progbase

  #define SOURCES \
    test_prog.cxx

#end test_bin_target

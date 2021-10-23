#begin ss_lib_target
  #define TARGET scmcopy
  #define LOCAL_LIBS \
    progbase pandatoolbase

  #define OTHER_LIBS \
    pipeline:c event:c pstatclient:c panda:m \
     pnmimage:c mathutil:c linmath:c putil:c express:c \
    interrogatedb prc \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

  #define SOURCES \
    scmCopy.h scmSourceDirectory.h scmSourceTree.h

  #define COMPOSITE_SOURCES \
    scmCopy.cxx scmSourceDirectory.cxx scmSourceTree.cxx

  #define INSTALL_HEADERS \
    scmCopy.h

#end ss_lib_target

#begin test_bin_target
  #define TARGET testcopy
  #define LOCAL_LIBS scmcopy

  #define SOURCES \
    testCopy.cxx testCopy.h

#end test_bin_target

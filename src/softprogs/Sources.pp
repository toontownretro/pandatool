#define BUILD_DIRECTORY $[HAVE_SOFTIMAGE]

#begin bin_target
  #define TARGET softcvs
  #define LOCAL_LIBS progbase
  #define USE_PACKAGES openssl

  #define OTHER_LIBS \ 
    egg:c pandaegg:m \
    pipeline:c event:c pstatclient:c panda:m \
    dtoolbase:c pnmimage:c mathutil:c linmath:c putil:c express:c \
    pandaexpress:m \
    interrogatedb prc \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

  #define SOURCES \
    softCVS.cxx softCVS.h softFilename.cxx softFilename.h

#end bin_target

#begin ss_lib_target
  #define TARGET eggbase
  #define LOCAL_LIBS \
    progbase converter
  #define OTHER_LIBS \
    pipeline:c event:c pstatclient:c panda:m \
     pnmimage:c mathutil:c linmath:c putil:c express:c \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m \
    $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],net:c downloader:c]

  #define SOURCES \
     eggBase.h eggConverter.h eggFilter.h \
     eggMakeSomething.h \
     eggMultiBase.h eggMultiFilter.h \
     eggReader.h eggSingleBase.h \
     eggToSomething.h eggWriter.h \
     somethingToEgg.h

  #define COMPOSITE_SOURCES \
     eggBase.cxx eggConverter.cxx eggFilter.cxx \
     eggMakeSomething.cxx \
     eggMultiBase.cxx \
     eggMultiFilter.cxx eggReader.cxx eggSingleBase.cxx \
     eggToSomething.cxx \
     eggWriter.cxx somethingToEgg.cxx

  #define INSTALL_HEADERS \
    eggBase.h eggConverter.h eggFilter.h \
    eggMakeSomething.h \
    eggMultiBase.h eggMultiFilter.h \
    eggReader.h eggSingleBase.h eggToSomething.h eggWriter.h somethingToEgg.h

#end ss_lib_target

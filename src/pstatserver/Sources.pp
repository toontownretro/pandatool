#define BUILD_DIRECTORY $[HAVE_NET]

#begin ss_lib_target
  #define TARGET pstatserver
  #define LOCAL_LIBS pandatoolbase
  #define OTHER_LIBS \
    pstatclient:c downloader:c net:c putil:c pipeline:c \
    panda:m \
     express:c linmath:c pandaexpress:m \
    $[if $[HAVE_NET],net:c] $[if $[WANT_NATIVE_NET],nativenet:c] \
    interrogatedb dtoolutil:c dtoolbase:c prc  dtool:m

  #define HEADERS \
    pStatClientData.h \
    pStatFlameGraph.h pStatFlameGraph.I \
    pStatGraph.h pStatGraph.I \
    pStatListener.h \
    pStatMonitor.h pStatMonitor.I \
    pStatPianoRoll.h pStatPianoRoll.I \
    pStatReader.h \
    pStatServer.h \
    pStatStripChart.h pStatStripChart.I \
    pStatThreadData.h pStatThreadData.I \
    pStatTimeline.h pStatTimeline.I \
    pStatView.h pStatView.I \
    pStatViewLevel.h pStatViewLevel.I

  #define SOURCES \
    $[HEADERS] \
    pStatClientData.cxx \
    pStatFlameGraph.cxx \
    pStatGraph.cxx \
    pStatListener.cxx \
    pStatMonitor.cxx \
    pStatPianoRoll.cxx \
    pStatReader.cxx \
    pStatServer.cxx \
    pStatStripChart.cxx \
    pStatThreadData.cxx \
    pStatTimeline.cxx \
    pStatView.cxx \
    pStatViewLevel.cxx

  #define INSTALL_HEADERS \
    $[HEADERS]

#end ss_lib_target

#define BUILD_DIRECTORY $[and $[HAVE_GTK],$[HAVE_NET]]
#define USE_PACKAGES net gtk

#begin bin_target
  // We suspect gtk will not be built universal on OSX.  Don't try.
  #define UNIVERSAL_BINARIES

  // We rename TARGET to pstats-gtk on Windows, so it won't compete
  // with Windows-native pstats.
  #define TARGET $[if $[or $[WINDOWS_PLATFORM],$[OSX_PLATFORM]],pstats-gtk,pstats]
  #define LOCAL_LIBS \
    progbase pstatserver
  #define OTHER_LIBS \
    $[if $[HAVE_NET],net:c] $[if $[WANT_NATIVE_NET],nativenet:c] \
     pnmimage:c event:c pstatclient:c \
    linmath:c putil:c pipeline:c express:c pandaexpress:m panda:m \
    interrogatedb dtoolutil:c dtoolbase:c prc  dtool:m

  #define HEADERS \
    gtkStatsChartMenu.h \
    gtkStatsFlameGraph.h \
    gtkStatsGraph.h \
    gtkStatsLabel.h \
    gtkStatsLabelStack.h \
    gtkStatsMonitor.h gtkStatsMonitor.I \
    gtkStatsPianoRoll.h \
    gtkStatsServer.h \
    gtkStatsStripChart.h

  #define SOURCES \
    $[HEADERS] \
    gtkStats.cxx \
    gtkStatsChartMenu.cxx \
    gtkStatsFlameGraph.cxx \
    gtkStatsGraph.cxx \
    gtkStatsLabel.cxx \
    gtkStatsLabelStack.cxx \
    gtkStatsMonitor.cxx \
    gtkStatsPianoRoll.cxx \
    gtkStatsServer.cxx \
    gtkStatsStripChart.cxx \
    gtkStatsTimeline.cxx

  #if $[DEVELOP_GTKSTATS]
    #define EXTRA_CDEFS $[EXTRA_CDEFS] DEVELOP_GTKSTATS
  #endif

#end bin_target

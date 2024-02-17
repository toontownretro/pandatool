#define BUILD_DIRECTORY $[and $[OSX_PLATFORM],$[HAVE_NET]]

#begin bin_target
  #define TARGET pstats

  #define OSX_SYS_FRAMEWORKS Foundation Cocoa Carbon Quartz

  #define LOCAL_LIBS \
    progbase pstatserver

  #define OTHER_LIBS \
    $[if $[HAVE_NET],net:c] $[if $[WANT_NATIVE_NET],nativenet:c] \
     pnmimage:c event:c pstatclient:c \
    linmath:c putil:c pipeline:c express:c pandaexpress:m panda:m \
    interrogatedb dtoolutil:c dtoolbase:c prc  dtool:m

  #define HEADERS \
    macStats.h \
    macStatsAppDelegate.h \
    macStatsChartMenu.h \
    macStatsChartMenuDelegate.h \
    macStatsFlameGraph.h \
    macStatsGraph.h \
    macStatsGraphView.h \
    macStatsGraphViewController.h \
    macStatsLabel.h \
    macStatsLabelStack.h \
    macStatsMonitor.h \
    macStatsPianoRoll.h \
    macStatsScaleArea.h \
    macStatsServer.h \
    macStatsStripChart.h \
    macStatsTimeline.h

  #define SOURCES \
    $[HEADERS] \
    macStats.mm \
    macStatsAppDelegate.mm \
    macStatsChartMenu.mm \
    macStatsChartMenuDelegate.mm \
    macStatsFlameGraph.mm \
    macStatsGraph.mm \
    macStatsGraphView.mm \
    macStatsGraphViewController.mm \
    macStatsLabel.mm \
    macStatsLabelStack.mm \
    macStatsMonitor.mm \
    macStatsPianoRoll.mm \
    macStatsScaleArea.mm \
    macStatsServer.mm \
    macStatsStripChart.mm \
    macStatsTimeline.mm

#end bin_target

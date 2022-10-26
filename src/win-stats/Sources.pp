#define BUILD_DIRECTORY $[and $[WINDOWS_PLATFORM],$[HAVE_NET]]
#define USE_PACKAGES net

#begin bin_target
  #define TARGET pstats
  #define LOCAL_LIBS \
    progbase pstatserver
  #define OTHER_LIBS \
    pstatclient:c linmath:c putil:c net:c express:c pandaexpress:m panda:m \
    dtoolutil:c dtoolbase:c prc  dtool:m

  #define HEADERS \
    winStatsChartMenu.h \
    winStatsFlameGraph.h \
    winStatsGraph.h \
    winStats.h \
    winStatsLabel.h winStatsLabel.I \
    winStatsLabelStack.h \
    winStatsMenuId.h \
    winStatsMonitor.h winStatsMonitor.I \
    winStatsPianoRoll.h \
    winStatsServer.h \
    winStatsStripChart.h \
    winStatsTimeline.h

  #define SOURCES \
    $[HEADERS] \
    winStatsChartMenu.cxx \
    winStats.cxx \
    winStatsFlameGraph.cxx \
    winStatsGraph.cxx \
    winStatsLabel.cxx \
    winStatsLabelStack.cxx \
    winStatsMonitor.cxx \
    winStatsPianoRoll.cxx \
    winStatsServer.cxx \
    winStatsStripChart.cxx \
    winStatsTimeline.cxx

  #if $[DEVELOP_WINSTATS]
    #define EXTRA_CDEFS $[EXTRA_CDEFS] DEVELOP_WINSTATS
  #endif
  #define EXTRA_CDEFS $[EXTRA_CDEFS] WIN32_LEAN_AND_MEAN

  #define WIN_SYS_LIBS Imm32.lib winmm.lib kernel32.lib oldnames.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib UxTheme.lib

#end bin_target

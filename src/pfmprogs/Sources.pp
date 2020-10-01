#define OTHER_LIBS \
    p3egg:c pandaegg:m \
    p3pipeline:c p3event:c p3display:c p3gobj:c p3pgraph:c p3pstatclient:c p3grutil:c panda:m \
    p3pandabase:c p3pnmimage:c p3pnmimagetypes:c p3mathutil:c p3linmath:c p3putil:c p3express:c \
    pandaexpress:m \
    p3interrogatedb p3prc  \
    p3dtoolutil:c p3dtoolbase:c p3dtool:m \
    $[if $[WANT_NATIVE_NET],p3nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],p3net:c p3downloader:c]

#define USE_PACKAGES threads

#begin bin_target
  #define TARGET pfm-trans
  #define LOCAL_LIBS p3progbase

  #define SOURCES \
    config_pfmprogs.cxx config_pfmprogs.h \
    pfmTrans.cxx pfmTrans.h

#end bin_target

#begin bin_target
  #define TARGET pfm-bba
  #define LOCAL_LIBS p3progbase

  #define SOURCES \
    config_pfmprogs.cxx config_pfmprogs.h \
    pfmBba.cxx pfmBba.h

#end bin_target

//#begin bin_target
//  #define TARGET pfm-wallpaper
//  #define LOCAL_LIBS p3progbase
//
//  #define SOURCES \
//    config_pfm.cxx config_pfm.h \
//    pfmWallpaper.cxx pfmWallpaper.h
//
//#end bin_target

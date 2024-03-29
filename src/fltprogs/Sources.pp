#define UNIX_SYS_LIBS m

#define OTHER_LIBS \
  egg:c pandaegg:m \
  linmath:c pnmimagetypes:c pnmimage:c event:c \
  putil:c mathutil:c pipeline:c pstatclient:c downloader:c net:c nativenet:c \
  panda:m \
   express:c pandaexpress:m \
  interrogatedb dtoolutil:c dtoolbase:c prc  dtool:m

#define USE_PACKAGES png

#begin bin_target
  #define TARGET fltcopy
  #define LOCAL_LIBS scmcopy flt

  #define SOURCES \
    fltCopy.cxx fltCopy.h

#end bin_target

#begin bin_target
  #define TARGET flt-trans
  #define LOCAL_LIBS \
    progbase flt

  #define SOURCES \
    fltTrans.cxx fltTrans.h

#end bin_target

#begin bin_target
  #define TARGET flt-info
  #define LOCAL_LIBS \
    progbase flt

  #define SOURCES \
    fltInfo.cxx fltInfo.h

#end bin_target

#begin bin_target
  #define TARGET flt2egg
  #define LOCAL_LIBS flt fltegg eggbase progbase

  #define SOURCES \
    fltToEgg.cxx fltToEgg.h

#end bin_target

#begin bin_target
  #define TARGET egg2flt
  #define LOCAL_LIBS flt eggbase progbase

  #define SOURCES \
    eggToFlt.cxx eggToFlt.h

#end bin_target

#define LOCAL_LIBS pandatoolbase progbase
#define OTHER_LIBS panda:m linmath:c gobj:c \
  pnmimage:c pnmimagetypes:c \
  dtool:m dtoolbase:c dtoolutil:c prc

#begin bin_target
  #define TARGET make-txo

  #define SOURCES \
    makeTxo.h \
    makeTxo.cxx

#end bin_target

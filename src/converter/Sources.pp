#define OTHER_LIBS \
  egg:c pandaegg:m \
  pipeline:c event:c display:c pgraph:c panda:m \
  mathutil:c linmath:c putil:c express:c  \
  interrogatedb prc \
  dtoolutil:c dtoolbase:c dtool:m

#begin ss_lib_target
  #define TARGET converter
  #define LOCAL_LIBS pandatoolbase
  #define UNIX_SYS_LIBS \
    m

  #define SOURCES \
    somethingToEggConverter.I somethingToEggConverter.cxx \
    somethingToEggConverter.h \
    eggToSomethingConverter.I eggToSomethingConverter.cxx \
    eggToSomethingConverter.h

  #define INSTALL_HEADERS \
    somethingToEggConverter.I somethingToEggConverter.h \
    eggToSomethingConverter.I eggToSomethingConverter.h

#end ss_lib_target

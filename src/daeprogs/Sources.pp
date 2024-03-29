#define BUILD_DIRECTORY $[HAVE_FCOLLADA]

#define OTHER_LIBS \
  egg:c pandaegg:m \
  pipeline:c pnmimage:c putil:c event:c mathutil:c linmath:c panda:m \
   express:c pandaexpress:m \
  interrogatedb dtoolutil:c dtoolbase:c prc dtool:m

#begin bin_target
  #define TARGET dae2egg
  #define LOCAL_LIBS daeegg eggbase progbase

  #define SOURCES \
    daeToEgg.cxx daeToEgg.h

#end bin_target

//#begin bin_target
//  #define TARGET egg2dae
//  #define LOCAL_LIBS daeegg eggbase progbase
//
//  #define SOURCES \
//    eggToDAE.cxx eggToDAE.h
//
//#end bin_target

#define USE_PACKAGES fftw png
#define OTHER_LIBS \
  egg2pg:c egg:c pandaegg:m \
  pipeline:c recorder:c parametrics:c collide:c anim:c \
  dgraph:c downloader:c recorder:c \
  pnmimagetypes:c pnmimage:c pgraph:c display:c \
  pgraphnodes:c gobj:c putil:c \
  mathutil:c linmath:c event:c pstatclient:c \
  gsgbase:c grutil:c text:c cull:c \
  tform:c device:c movies:c material:c pdx:c \
  $[if $[HAVE_FREETYPE],pnmtext:c] \
  $[if $[HAVE_NET],net:c] $[if $[WANT_NATIVE_NET],nativenet:c] \
  $[if $[HAVE_AUDIO],audio:c] \
  panda:m \
   express:c pandaexpress:m \
  interrogatedb dtoolutil:c dtoolbase:c prc \
  dtool:m

#begin bin_target
  #define TARGET bam-info
  #define LOCAL_LIBS \
    progbase palettizer

  #define SOURCES \
    bamInfo.cxx bamInfo.h

  #define INSTALL_HEADERS
#end bin_target

#if $[HAVE_EGG]

#begin bin_target
  #define TARGET egg2bam
  #define LOCAL_LIBS \
    eggbase progbase

  #define SOURCES \
    eggToBam.cxx eggToBam.h
#end bin_target


#begin bin_target
  #define BUILD_TARGET
  #define TARGET bam2egg
  #define LOCAL_LIBS \
    converter eggbase progbase

  #define SOURCES \
    bamToEgg.cxx bamToEgg.h
#end bin_target

#begin bin_target
  #define TARGET pts2bam
  #define LOCAL_LIBS \
   progbase

  #define SOURCES \
    ptsToBam.cxx ptsToBam.h
#end bin_target

#begin bin_target
  #define TARGET make-txo
  #define LOCAL_LIBS progbase

  #define SOURCES \
    makeTxo.cxx makeTxo.h
#end bin_target

#begin bin_target
  #define TARGET ptex2txo
  #define LOCAL_LIBS progbase

  #define SOURCES \
    ptexToTxo.cxx ptexToTxo.h
#end bin_target

#begin bin_target
  #define TARGET pmat2mto
  #define LOCAL_LIBS progbase

  #define SOURCES \
    pmatToMto.cxx pmatToMto.h
#end bin_target

#begin bin_target
  #define TARGET pmdl2bam
  #define LOCAL_LIBS progbase
  #define SOURCES \
    pmdlToBam.cxx pmdlToBam.h
#end bin_target

#endif // HAVE_EGG

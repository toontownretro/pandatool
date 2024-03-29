#define LOCAL_LIBS \
  converter eggbase progbase
#define OTHER_LIBS \
  egg:c pandaegg:m \
  event:c pipeline:c pstatclient:c downloader:c net:c nativenet:c \
  pnmimagetypes:c pnmimage:c mathutil:c linmath:c putil:c \
  panda:m \
   express:c pandaexpress:m \
  interrogatedb dtoolutil:c dtoolbase:c prc  dtool:m
#define UNIX_SYS_LIBS m
#define USE_PACKAGES png

#begin bin_target
  #define TARGET egg-rename

  #define SOURCES \
    eggRename.cxx eggRename.h

#end bin_target

#begin bin_target
  #define TARGET egg-trans

  #define SOURCES \
    eggTrans.cxx eggTrans.h

#end bin_target

#begin bin_target
  #define TARGET egg-crop

  #define SOURCES \
    eggCrop.cxx eggCrop.h

#end bin_target

#begin bin_target
  #define TARGET egg-texture-cards

  #define SOURCES \
    eggTextureCards.cxx eggTextureCards.h

#end bin_target

#begin bin_target
  #define TARGET egg-make-tube

  #define SOURCES \
    eggMakeTube.cxx eggMakeTube.h

#end bin_target

#begin bin_target
  #define LOCAL_LIBS eggcharbase $[LOCAL_LIBS]
  #define TARGET egg-topstrip

  #define SOURCES \
    eggTopstrip.cxx eggTopstrip.h

#end bin_target

#begin bin_target
  #define LOCAL_LIBS eggcharbase $[LOCAL_LIBS]
  #define TARGET egg-retarget-anim

  #define SOURCES \
    eggRetargetAnim.cxx eggRetargetAnim.h

#end bin_target

#begin bin_target
  #define TARGET egg2c

  #define SOURCES \
    eggToC.cxx eggToC.h

#end bin_target

#begin bin_target
  #define TARGET egg-list-textures

  #define SOURCES \
    eggListTextures.cxx eggListTextures.h

#end bin_target

#begin bin_target
  #define TARGET egg-strip-motion

  #define SOURCES \
    eggStripMotion.cxx eggStripMotion.h

#end bin_target

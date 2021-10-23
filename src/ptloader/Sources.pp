#begin lib_target
  #define TARGET ptloader
  #define BUILDING_DLL BUILDING_PTLOADER
  #define LOCAL_LIBS \
    fltegg flt lwoegg lwo dxfegg dxf vrmlegg vrml xfileegg xfile \
    objegg \
    converter pandatoolbase $[if $[HAVE_FCOLLADA],daeegg]
  #define OTHER_LIBS \
    egg2pg:c egg:c pandaegg:m \
    pstatclient:c mathutil:c linmath:c putil:c \
    gobj:c parametrics:c pgraph:c pgraphnodes:c \
    pnmimage:c grutil:c collide:c tform:c text:c \
    anim:c dgraph:c display:c device:c cull:c \
    downloader:c pipeline:c \
    event:c gsgbase:c movies:c \
    $[if $[HAVE_FREETYPE],pnmtext:c] \
    $[if $[HAVE_NET],net:c] $[if $[WANT_NATIVE_NET],nativenet:c] \
    $[if $[HAVE_AUDIO],audio:c] \
    panda:m \
     express:c pandaexpress:m \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m
  #define UNIX_SYS_LIBS \
    m

  #define SOURCES \
    config_ptloader.cxx config_ptloader.h \
    loaderFileTypePandatool.cxx loaderFileTypePandatool.h

  #define INSTALL_HEADERS \
    config_ptloader.h loaderFileTypePandatool.h

#end lib_target

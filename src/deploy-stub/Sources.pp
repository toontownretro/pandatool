#define BUILD_DIRECTORY $[HAVE_PYTHON]

#define USE_PACKAGES python

#define WIN_SYS_LIBS $[WIN_SYS_LIBS] Shell32.lib

#begin bin_target
  #define TARGET deploy-stub

  #define SOURCES \
    deploy-stub.c \
    $[if $[WINDOWS_PLATFORM],frozen_dllmain.c]

  #if $[OSX_PLATFORM]
    #define LFLAGS $[LFLAGS] -sectcreate __PANDA __panda /dev/null
  #elif $[UNIX_PLATFORM]
    #define LFLAGS $[LFLAGS] -Wl,-z,origin -rdynamic
  #endif

#end bin_target

#begin bin_target
  #define BUILD_TARGET $[or $[WINDOWS_PLATFORM],$[OSX_PLATFORM]]

  #define TARGET deploy-stubw

  #define SOURCES \
    deploy-stub.c \
    $[if $[WINDOWS_PLATFORM],frozen_dllmain.c]

  #if $[OSX_PLATFORM]
    #define LFLAGS $[LFLAGS] -sectcreate __PANDA __panda /dev/null
    #define CFLAGS $[CFLAGS] -DMACOS_APP_BUNDLE=1
  #endif

#end bin_target

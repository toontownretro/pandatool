add_library(mayaeggimport SHARED mayaEggImport.cxx)
target_link_libraries(mayaeggimport panda p3maya p3mayaegg)
#install(TARGETS mayaeggimport EXPORT Tools COMPONENT Tools DESTINATION ${CMAKE_INSTALL_BINDIR})

add_library(p3mayaloader SHARED config_mayaloader.cxx)
target_link_libraries(p3mayaloader panda p3maya p3mayaegg
  p3fltegg
  p3flt
  p3lwoegg
  p3lwo
  p3dxfegg
  p3dxf
  p3objegg
  p3vrmlegg
  p3xfileegg
  p3xfile
  p3eggbase
  p3progbase
  p3converter
  p3pandatoolbase
  p3ptloader)

add_library(mayapview SHARED mayaPview.cxx)
target_link_libraries(mayapview p3mayaegg p3maya p3framework
  p3pandatoolbase
  p3converter
  p3progbase)

add_executable(maya2egg mayaToEgg.cxx)
target_link_libraries(maya2egg p3mayaegg p3maya
  p3converter
  p3progbase)

install(TARGETS maya2egg EXPORT Tools COMPONENT Tools DESTINATION ${CMAKE_INSTALL_BINDIR})

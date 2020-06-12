set(P3MAYAEGG_SOURCES
  config_mayaegg.cxx
  mayaBlendDesc.cxx
  mayaEggGroupUserData.cxx
  mayaEggLoader.cxx
  mayaNodeDesc.cxx
  mayaNodeTree.cxx
  mayaToEggConverter.cxx
)

set(P3MAYAEGG_HEADERS
  config_mayaegg.h
  mayaBlendDesc.h
  mayaEggGroupUserData.h
  mayaEggLoader.h
  mayaNodeDesc.h
  mayaNodeTree.h
  mayaToEggConverter.h
)

composite_sources(p3mayaegg P3MAYAEGG_SOURCES)
add_library(p3mayaegg STATIC ${P3MAYAEGG_SOURCES} ${P3MAYAEGG_HEADERS})
target_link_libraries(p3mayaegg p3maya pandaegg p3eggbase)
#set_target_properties(p3mayaegg PROPERTIES POSITION_INDEPENDENT_CODE ON)

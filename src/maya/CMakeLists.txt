set(P3MAYA_SOURCES
  config_maya.cxx
  mayaApi.cxx
  mayaShader.cxx
  mayaShaderColorDef.cxx
  mayaShaders.cxx
  maya_funcs.cxx
)

set(P3MAYA_HEADERS
  config_maya.h
  mayaApi.h
  maya_funcs.h
  maya_funcs.I
  maya_funcs.T
  mayaShader.h
  mayaShaderColorDef.h
  mayaShaders.h
  post_maya_include.h
  pre_maya_include.h
)

composite_sources(p3maya P3MAYA_SOURCES)
add_library(p3maya STATIC ${P3MAYA_HEADERS} ${P3MAYA_SOURCES})
target_link_libraries(p3maya p3pandatoolbase PKG::MAYA)
#set_target_properties(p3maya PROPERTIES POSITION_INDEPENDENT_CODE ON)

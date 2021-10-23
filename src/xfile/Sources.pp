#define YACC_PREFIX xyy
#define FLEXFLAGS -i

#define USE_PACKAGES zlib

#begin ss_lib_target
  #define TARGET xfile
  #define LOCAL_LIBS pandatoolbase
  #define OTHER_LIBS \
    event:c putil:c pipeline:c mathutil:c linmath:c panda:m \
     express:c pandaexpress:m \
    interrogatedb prc  \
    dtoolutil:c dtoolbase:c dtool:m

  #define USE_PACKAGES zlib

  #define SOURCES \
     config_xfile.h \
     standard_templates.h \
     windowsGuid.h \
     xFile.I xFile.h \
     xFileArrayDef.I xFileArrayDef.h \
     xFileDataDef.I xFileDataDef.h \
     xFileDataNode.I xFileDataNode.h \
     xFileDataNodeReference.I xFileDataNodeReference.h \
     xFileDataNodeTemplate.I xFileDataNodeTemplate.h \
     xFileDataObject.I xFileDataObject.h \
     xFileDataObjectArray.I xFileDataObjectArray.h \
     xFileDataObjectDouble.I xFileDataObjectDouble.h \
     xFileDataObjectInteger.I xFileDataObjectInteger.h \
     xFileDataObjectString.I xFileDataObjectString.h \
     xFileNode.I xFileNode.h \
     xFileParseData.I xFileParseData.h \
     xFileTemplate.I xFileTemplate.h \
     xLexer.lxx xLexerDefs.h \
     xParser.yxx xParserDefs.h

  #define COMPOSITE_SOURCES \
     config_xfile.cxx \
     standard_templates.cxx \
     windowsGuid.cxx\
     xFile.cxx \
     xFileArrayDef.cxx \
     xFileDataDef.cxx \
     xFileDataNode.cxx \
     xFileDataNodeReference.cxx \
     xFileDataNodeTemplate.cxx \
     xFileDataObject.cxx \
     xFileDataObjectArray.cxx \
     xFileDataObjectDouble.cxx \
     xFileDataObjectInteger.cxx \
     xFileDataObjectString.cxx \
     xFileNode.cxx \
     xFileParseData.cxx \
     xFileTemplate.cxx


#end ss_lib_target

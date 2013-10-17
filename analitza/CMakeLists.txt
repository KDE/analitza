project(analitza)

add_subdirectory(tests)

set(analitza_SRCS
    expressionstream.cpp
	polynomial.cpp
	importqobjectmetatype.cpp
	builtinmethods.cpp
	providederivative.cpp
	abstractexpressiontransformer.cpp
	expression.cpp
	expressionwriter.cpp
	stringexpressionwriter.cpp
	mathmlexpressionwriter.cpp
	mathmlpresentationexpressionwriter.cpp
	htmlexpressionwriter.cpp
	analyzer.cpp
	container.cpp
	operator.cpp
	operations.cpp
	variables.cpp
	object.cpp
	value.cpp
	variable.cpp
	vector.cpp
	list.cpp
	apply.cpp
	customobject.cpp
	matrix.cpp
	
	expressiontype.cpp
	expressiontypechecker.cpp
	
	expressiontable.cpp	#generated
	expressionparser.cpp	#   ''
	explexer.cpp
	abstractlexer.cpp
	mathmlpresentationlexer.cpp
	analitzautils.cpp
	substituteexpression.cpp
	transformation.cpp
)

add_library(analitza ${analitza_SRCS})
target_link_libraries(analitza Qt5::Core Qt5::Xml )

set_target_properties(analitza PROPERTIES VERSION ${ANALITZA_LIB_VERSION} SOVERSION ${ANALITZA_LIB_SOVERSION} )

install(TARGETS analitza EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES 
	abstractlexer.h
	analitzaexport.h
	analyzer.h
	analitzautils.h
	expression.h
	object.h
	operations.h
	operator.h
	variable.h
	value.h
	variables.h
	vector.h
	expressiontype.h
	builtinmethods.h
	importqobjectmetatype.h
	expressionstream.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/analitza/ COMPONENT Devel)
	

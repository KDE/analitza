add_subdirectory(tests)

set(analitza_SRCS

    expressionstream.cpp
	polynomial.cpp
	importqobjectmetatype.cpp
	builtinmethods.cpp
	providederivative.cpp
	abstractexpressiontransformer.cpp
	expression.cpp
	abstractexpressionvisitor.cpp
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
	
	commands/realpower.cpp
	commands/listcommands.cpp
	commands/vectorcommands.cpp
	commands/matrixcommands.cpp
	commands/blockmatrixcommands.cpp
	commands/matrixqueries.cpp
	commands/combinatronics.cpp
)

ecm_create_qm_loader(analitza_SRCS analitza_qt)

add_library(Analitza ${analitza_SRCS})
target_link_libraries(Analitza PUBLIC Qt5::Core PRIVATE Qt5::Xml)
target_include_directories(Analitza INTERFACE "$<INSTALL_INTERFACE:${ANALITZA_INCLUDE_INSTALL_DIR}>")

set_target_properties(Analitza PROPERTIES VERSION ${ANALITZA_VERSION_STRING} SOVERSION ${ANALITZA_SOVERSION} )

if(EIGEN3_FOUND)
    target_sources(Analitza PRIVATE commands/eigencommands.cpp)
    target_link_libraries(Analitza PRIVATE Eigen3::Eigen)
endif()

generate_export_header(Analitza EXPORT_FILE_NAME analitzaexport.h)

install(TARGETS Analitza EXPORT AnalitzaTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES 
	${CMAKE_CURRENT_BINARY_DIR}/analitzaexport.h
	abstractlexer.h
	analyzer.h
	analitzautils.h
	expression.h
	abstractexpressionvisitor.h
	object.h
	operations.h
	operator.h
	variable.h
	value.h
	variables.h
	vector.h
	apply.h
	list.h
	customobject.h
	container.h
	matrix.h
	expressiontype.h
	builtinmethods.h
	importqobjectmetatype.h
	expressionstream.h
	DESTINATION ${ANALITZA_INCLUDE_INSTALL_DIR}/analitza/ COMPONENT Devel)
	

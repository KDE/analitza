project(AnalitzaJIT)

add_definitions(${LLVM_CFLAGS})
link_directories(${LLVM_LIBRARY_DIR})

include_directories(${LLVM_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)

set ( analitzajit_SRCS
    typecompiler.cpp
    operationscompiler.cpp
    expressioncompiler.cpp
    jitanalyzer.cpp
)

add_library( AnalitzaJIT ${analitzajit_SRCS} )
target_link_libraries ( AnalitzaJIT
  Qt5::Core
  Qt5::Gui

  Analitza
  ${LLVM_MODULE_LIBS} ${LLVM_LFLAGS}
)

set_target_properties(AnalitzaJIT PROPERTIES VERSION ${ANALITZA_VERSION_STRING} SOVERSION ${ANALITZA_SOVERSION} )

install(TARGETS AnalitzaJIT EXPORT AnalitzaTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    analitzajitexport.h
    typecompiler.h
    operationscompiler.h
    expressioncompiler.h
    jitanalyzer.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/analitzajit/ COMPONENT Devel
)

project(analitza)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Qt5 5.2 REQUIRED NO_MODULE COMPONENTS Widgets Xml Svg OpenGL Test)
find_package(KF5 5.0.0 REQUIRED MODULE COMPONENTS CMake Compiler InstallDirs)

include(FeatureSummary)
include(ECMSetupVersion)
include(ECMAddTests)

ecm_setup_version(5.0.0 VARIABLE_PREFIX ANALITZA
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/analitza_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/AnalitzaConfigVersion.cmake")

#Checking whether we can compile calgebra
# include(CheckIncludeFiles)
# macro_optional_find_package(Readline)
# macro_bool_to_01(READLINE_FOUND HAVE_READLINE)
# macro_log_feature(READLINE_FOUND "GNU Readline" "Allows KAlgebra to provide a console interface." "http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html" FALSE "" "")
# 
find_package(OpenGL)
set(HAVE_OPENGL ${OPENGL_FOUND})
# macro_log_feature(OPENGL_FOUND "OpenGL" "Support for 3D graphs in Analitza" "http://opengl.org" FALSE "" "")

# add_definitions (-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_NO_KEYWORDS)

add_subdirectory(analitza)
add_subdirectory(analitzaplot)
add_subdirectory(analitzagui)
if(HAVE_READLINE)
	add_subdirectory(calgebra)
endif(HAVE_READLINE)

# macro_display_feature_log()

set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/Analitza")
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/AnalitzaConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AnalitzaConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS  INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/AnalitzaConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/AnalitzaConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT AnalitzaTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE AnalitzaTargets.cmake NAMESPACE KF5:: )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/analitza_version.h
  DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)

export(TARGETS analitza analitzaplot analitzagui FILE "${PROJECT_BINARY_DIR}/AnalitzaTargets.cmake")

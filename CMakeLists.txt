project(analitza)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
set(CMAKE_MODULE_PATH ${analitza_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

#Checking whether we can compile calgebra
include(CheckIncludeFiles)
macro_optional_find_package(Readline)
macro_bool_to_01(READLINE_FOUND HAVE_READLINE)
macro_log_feature(READLINE_FOUND "GNU Readline" "Allows KAlgebra to provide a console interface." "http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html" FALSE "" "")

macro_optional_find_package(OpenGL)
macro_bool_to_01(OPENGL_FOUND HAVE_OPENGL)
macro_log_feature(OPENGL_FOUND "OpenGL" "Support for 3D graphs in Analitza" "http://opengl.org" FALSE "" "")

include_directories(.)
add_definitions (-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

set(ANALITZA_LIB_VERSION "5.0.0")
set(ANALITZA_LIB_SOVERSION "5")

add_subdirectory(analitza)
add_subdirectory(analitzaplot)
add_subdirectory(analitzagui)
if(HAVE_READLINE)
	add_subdirectory(calgebra)
endif(HAVE_READLINE)

macro_display_feature_log()

# # # # # # # # # # #  Installation of the CMake config file
if(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
    set(_analitza_CONFIG_DEST "${LIB_INSTALL_DIR}/cmake/analitza")
else(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
    set(_analitza_CONFIG_DEST "${LIB_INSTALL_DIR}/analitza")
endif(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AnalitzaConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/AnalitzaConfig.cmake" @ONLY )
export(TARGETS analitza analitzaplot analitzagui
  FILE "${PROJECT_BINARY_DIR}/AnalitzaTargets.cmake")

install( FILES
        "${CMAKE_CURRENT_BINARY_DIR}/AnalitzaConfig.cmake"
        DESTINATION "${_analitza_CONFIG_DEST}" )
install( EXPORT AnalitzaTargets
         DESTINATION "${_analitza_CONFIG_DEST}"
         NAMESPACE AnalitzaImport__
         FILE AnalitzaTargets.cmake )

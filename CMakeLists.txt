project(kalgebra)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
set(CMAKE_MODULE_PATH ${kalgebra_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

#Checking whether we can compile calgebra
include(CheckIncludeFiles)
macro_optional_find_package(Readline)
macro_bool_to_01(READLINE_FOUND HAVE_READLINE)
macro_log_feature(READLINE_FOUND "GNU Readline" "Allows KAlgebra to provide a console interface." "http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html" FALSE "" "")

include_directories(.)
add_definitions (-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

add_subdirectory(analitza)
add_subdirectory(analitzagui)
if(HAVE_READLINE)
	add_subdirectory(calgebra)
endif(HAVE_READLINE)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
   macro_display_feature_log()
endif (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})


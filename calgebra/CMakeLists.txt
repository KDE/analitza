include(CheckFunctionExists)
include(CMakePushCheckState)

include_directories(${READLINE_INCLUDE_DIR} ${CURSES_INCLUDE_DIR})

set(calgebra_SRCS
	main.cpp
)

# check if we have recent version of Readline
cmake_push_check_state(RESET)
set(CMAKE_REQUIRED_LIBRARIES ${READLINE_LIBRARY} ${CURSES_LIBRARIES})
check_function_exists(free_history_entry HAVE_FREE_HISTORY_ENTRY)
cmake_pop_check_state()
if(HAVE_FREE_HISTORY_ENTRY)
	add_definitions(-DHAVE_FREE_HISTORY_ENTRY)
endif()

kde4_add_executable(calgebra ${calgebra_SRCS})
target_link_libraries(calgebra ${QT_QTCORE_LIBRARY} ${READLINE_LIBRARY} ${CURSES_LIBRARIES} analitza)

install(TARGETS calgebra ${INSTALL_TARGETS_DEFAULT_ARGS})


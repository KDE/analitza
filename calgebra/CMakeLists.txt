include_directories(${READLINE_INCLUDE_DIR})

set(calgebra_SRCS
	main.cpp
)

add_executable(calgebra ${calgebra_SRCS})
target_link_libraries(calgebra Qt5::Core ${READLINE_LIBRARY} analitza)

install(TARGETS calgebra ${INSTALL_TARGETS_DEFAULT_ARGS})


set(testLibs Qt5::Core Analitza AnalitzaJIT)

add_executable(benchmarking benchmarking.cpp)
target_link_libraries(benchmarking ${testLibs})

add_executable(bench bench.cpp)
target_link_libraries(bench TRIE)

include_directories("../external/SuRF/include")

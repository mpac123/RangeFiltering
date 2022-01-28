add_executable(bench bench.cpp)
add_executable(prefixBF_bench prefixBF_bench.cpp)
add_executable(bench_A bench_A.cpp)

target_link_libraries(bench TRIE)
target_link_libraries(prefixBF_bench TRIE)
target_link_libraries(bench_A TRIE)

include_directories("../external/SuRF/include")

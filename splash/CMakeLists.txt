include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../baseline/include")
enable_testing()

add_subdirectory(test)

add_library(splash include/splash.hpp)
target_link_libraries(splash TRIE)
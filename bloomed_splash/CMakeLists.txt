include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

enable_testing()

add_subdirectory(test)

add_library(bloomed_splash include/bloomed_splash.hpp)
target_link_libraries(bloomed_splash TRIE)
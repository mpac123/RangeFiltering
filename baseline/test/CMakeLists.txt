find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

function (add_unit_test file_name)
    add_executable(${file_name} ${file_name}.cpp)
    target_link_libraries(${file_name} gtest TRIE FILTER mmh qf)
    add_test(NAME ${file_name}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

add_unit_test(test_trie)
add_unit_test(test_compactTrie)
add_unit_test(test_blindTrieWithBloomFilter)
add_unit_test(test_prefixBloomFilter)
add_unit_test(test_prefixQuotientFilter)
add_unit_test(test_multiPrefixBloomFilter)
add_unit_test(test_multiPrefixQuotientFilter)
add_unit_test(test_surfingTrie)
add_unit_test(test_restrainedSurfingTrie)
add_unit_test(test_bloomFiltersEnsemble)
add_unit_test(test_bloomedSurfingTrie)
add_unit_test(test_splashyTrie)
add_unit_test(test_restrainedSplashyTrie)
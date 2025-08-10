cmake_minimum_required(VERSION 3.12)
project(data_structures)

set(CMAKE_CXX_STANDARD 17)

add_library(data_structures STATIC
        search_tree/avltree.cpp search_tree/BTree.cpp search_tree/rbtree.cpp
        trie/stringmap.cpp
        misc/pst.cpp)
 target_include_directories(data_structures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/search_tree
         ${CMAKE_CURRENT_SOURCE_DIR}/trie
         ${CMAKE_CURRENT_SOURCE_DIR}/common
         ${CMAKE_CURRENT_SOURCE_DIR}/misc
         ${CMAKE_CURRENT_SOURCE_DIR}/sort
         ${CMAKE_CURRENT_SOURCE_DIR}/priority_queue
)

add_executable(test_speed tests/test.cpp)
target_link_libraries(test_speed data_structures)

# set output directory to bin
set_target_properties(test_speed PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

enable_testing()
add_subdirectory(tests/unit)

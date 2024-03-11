# Data-Struct-Eval

Data-Struct-Eval is a C++ project that includes different implementations
and speed tests of different search trees and sorting algorithms.

## Project Structure

- **common/**: Common utility files.
    - `alloc.h`: Memory allocation utility for data structures.

- **search_tree/**: Search tree implementations.
    - `commontree.h`: Common functions for trees.
    - `rbtree.cpp`, `rbtree.h`: Red-Black tree implementation similar to std::map/set.
    - `avltree.cpp`, `avltree.h`: AVL tree implementation.
    - `23Tree.h`: 2-3 tree implementation.
    - `BTree.cpp`, `Btree.h`: B-tree implementation where B can be setup at runtime.
    - `BtreeT.h` : B-tree implementation where B is a template parameter.

- **misc/**: Miscellaneous files.
    - `pst.h`, `pst.cpp`: Implementation of a Priority Search Tree. Can be used to solve 3-sided range queries.
    - `structs.h`: File with some data structures; some of them are probably duplicates of the ones in the other folders;
       shoule be refactored in the future and split into different files.

- **priority_queue/**: Priority queue implementation.
    - `priority_queue.h`: simple and efficient priority queue data structure for limited number of priorities.

- **sort/**: Sorting algorithm implementations.
    - `sort.h`: Sorting algorithms.

- **tests/**: Test suite.
    - `test.cpp`: Test cases for evaluating the implemented structures.

- **trie/**: Trie implementation.
    - `stringmap.cpp`, `stringmap.h`: String map using a trie-like structure.

- **CMakeLists.txt**: CMake configuration for building the project.


## Building the Project

The project can be built using CMake. The following commands can be used to build the project:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release 
```

## Running Speed Tests

The speed tests can be run using the following command:

```bash
bin/test_speed N
```

Where `N` is the number of elements to be used in the speed tests 
(size of the data structures and arrays).

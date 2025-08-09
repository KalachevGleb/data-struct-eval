# Try to find Catch2; if not found, fetch it automatically
find_package(Catch2 3 QUIET)
if (NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0
    )
    FetchContent_MakeAvailable(Catch2)
    # Ensure CMake can find the test discovery module provided by Catch2
    if (EXISTS "${Catch2_SOURCE_DIR}/extras")
        list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
    endif()
endif()

# Include test discovery module (supports both Catch2 v3 and v2)
include(Catch2 OPTIONAL)
include(Catch OPTIONAL)

add_executable(unit_tests test_structures.cpp)
target_link_libraries(unit_tests PRIVATE data_structures Catch2::Catch2WithMain)
catch_discover_tests(unit_tests)

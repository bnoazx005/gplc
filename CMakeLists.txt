cmake_minimum_required (VERSION 3.8)

project (gplc)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<CONFIGURATION>")

# Global variables are declared here
set(GPL_LIBRARY_NAME "gpllib")
set(GPL_TESTS_NAME "tests")

# Global options are declared here
option(IS_TESTING_ENABLED "The option turns on/off tests" ON)

if (IS_TESTING_ENABLED)
	enable_testing()
endif ()

add_subdirectory(gpllib)
add_subdirectory(gplc)

if (IS_TESTING_ENABLED)
	add_subdirectory(tests)
endif ()
cmake_minimum_required (VERSION 3.8)

project (gplc_utility CXX)

set(GPLC_UTILITY_NAME "gplc")

find_package(LLVM REQUIRED CONFIG)

option(IS_TESTING_ENABLED "The option turns on/off tests" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIGURATION>")

if (NOT DEFINED ${GPL_LIBRARY_NAME})
	set(GPL_LIBRARY_NAME "gpllib")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# attach GPL
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../gpllib/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../bin")

set(HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/argparse/argparse.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/gplcCompilerDriver.h")

set(SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/gplcCompilerDriver.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/argparse/argparse.c")

source_group("includes" FILES ${HEADERS})
source_group("sources" FILES ${SOURCES})

include_directories ("${PROJECT_SOURCE_DIR}/include/")

if (MSVC) 	#cl.exe compiler's options

	#Debug compiler's options
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /std:c++17 /MDd /W3 /GS /Zc:inline /Od /ZI  /Zc:wchar_t")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /RTC1 /Gd /Oy- /EHsc /nologo /diagnostics:classic /errorReport:prompt /sdl- /permissive- /analyze-")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D _DEBUG")

	#Release compiler's options
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /std:c++17 /permissive- /GS /GL /analyze- /W3 /Gy /Zc:wchar_t /Zi /Gm- /O2 /sdl- /Zc:inline")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /fp:precise /D _WINDLL /D _MBCS /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /Oi /MD /EHsc /nologo /diagnostics:classic")

endif(MSVC)

# include argparse library
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/deps/argparse/")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(${GPLC_UTILITY_NAME} ${SOURCES} ${HEADERS})

llvm_map_components_to_libnames(llvm_libs support core irreader bitwriter)

target_link_libraries(${GPLC_UTILITY_NAME} PUBLIC ${GPL_LIBRARY_NAME} ${llvm_libs})
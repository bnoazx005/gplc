cmake_minimum_required (VERSION 3.8)

project (tests CXX)

find_package(LLVM REQUIRED CONFIG)

option(IS_TESTING_ENABLED "The option turns on/off tests" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIGURATION>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIGURATION>")

if (NOT DEFINED ${GPL_LIBRARY_NAME})
	set(GPL_LIBRARY_NAME "gpllib")
endif ()

if (NOT DEFINED ${GPL_TESTS_NAME})
	set(GPL_TESTS_NAME "tests")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (IS_TESTING_ENABLED)
	enable_testing()
endif ()

# attach GPL
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../gpllib/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../bin")

# include Catch2
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/lib/Catch2/contrib/")
add_subdirectory(lib/Catch2)

set(HEADERS )

set(SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/codegen/llvmCodeGeneratorTests.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/codegen/llvmLiteralVisitorTests.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/codegen/llvmTypeVisitorTests.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/codegen/ctplr/llvmCCodeGeneratorTests.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/common/symTable.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/common/typeSystemTests.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/common/constExprInterpreter.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/lexer/lexer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/lexer/stubInputStream.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/lexer/stubInputStream.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/parser/semanticAnalyser.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/parser/stubLexer.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/parser/stubLexer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/utils/ASTLispyPrinterTests.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp")

source_group("includes" FILES ${HEADERS})
source_group("sources" FILES ${SOURCES})

if (MSVC) 	#cl.exe compiler's options

	#Debug compiler's options
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /std:c++17 /MDd /W3 /GS /Zc:inline /Od /ZI  /Zc:wchar_t")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /RTC1 /Gd /Oy- /EHsc /nologo /diagnostics:classic /errorReport:prompt /sdl- /permissive- /analyze-")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D _DEBUG")

	#Release compiler's options
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /std:c++17 /permissive- /GS /GL /analyze- /W3 /Gy /Zc:wchar_t /Zi /Gm- /O2 /sdl- /Zc:inline")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /fp:precise /D _WINDLL /D _MBCS /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /Oi /MD /EHsc /nologo /diagnostics:classic")

endif(MSVC)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(${GPL_TESTS_NAME} ${SOURCES} ${HEADERS})

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(${GPL_TESTS_NAME} PUBLIC ${GPL_LIBRARY_NAME} Catch2::Catch2 ${llvm_libs})

include(CTest)
include(Catch)

catch_discover_tests(${GPL_TESTS_NAME})
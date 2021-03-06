cmake_minimum_required (VERSION 3.8)

project (GPL CXX)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/$<CONFIGURATION>/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/$<CONFIGURATION>/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/$<CONFIGURATION>/")

if (NOT DEFINED ${GPL_LIBRARY_NAME})
	set(GPL_LIBRARY_NAME "gpllib")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/deps/cmake-modules/")

set(GPL_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/gplc.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcCodegen.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcLinker.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcNativePrimitives.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcLLVMNativePrimitives.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcLLVMCodegen.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcLLVMLinker.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcLLVMLiteralVisitor.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/gplcLLVMTypeVisitor.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/ctplr/gplcCCodegen.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/ctplr/gplcCTypeVisitor.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/codegen/ctplr/gplcCLiteralVisitor.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcConstants.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcValues.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcSymTable.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcTypes.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcTypeSystem.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcModuleResolver.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcVisitor.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcConstExprInterpreter.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/common/gplcTypesFactory.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/lexer/gplcLexer.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/lexer/gplcTokens.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/lexer/gplcInputStream.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/parser/gplcParser.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/parser/gplcASTNodes.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/parser/gplcASTNodesFactory.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/parser/gplcSemanticAnalyser.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/utils/Callback.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/utils/CASTLispyPrinter.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/utils/CResult.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/utils/Delegate.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/utils/Utils.h"
	)

set(GPL_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/gplcLLVMCodegen.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/gplcLLVMLiteralVisitor.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/gplcLLVMTypeVisitor.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/gplcLLVMLinker.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/gplcLLVMNativePrimitives.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/ctplr/gplcCCodegen.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/ctplr/gplcCTypeVisitor.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/codegen/ctplr/gplcCLiteralVisitor.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/common/gplcValues.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/common/gplcSymTable.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/common/gplcTypeSystem.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/common/gplcConstExprInterpreter.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/common/gplcModuleResolver.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/common/gplcTypesFactory.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/lexer/gplcLexer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/lexer/gplcTokens.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/lexer/gplcInputStream.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/parser/gplcParser.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/parser/gplcASTNodes.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/parser/gplcASTNodesFactory.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/parser/gplcSemanticAnalyser.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/utils/CASTLispyPrinter.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/utils/Utils.cpp"
	)

source_group("includes" FILES ${GPL_HEADERS})
source_group("sources" FILES ${GPL_SOURCES})


if (MSVC) 	#cl.exe compiler's options
	message(STATUS "Windows system has detected...")

	#Debug compiler's options
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /std:c++17 /MDd /W3 /GS /Zc:inline /Od /Zi  /Zc:wchar_t")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /RTC1 /Gd /Oy- /EHsc /nologo /diagnostics:classic /errorReport:prompt /sdl- /permissive- /analyze-")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D _DEBUG")

	#Release compiler's options
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /std:c++17 /permissive- /GS /GL /analyze- /W3 /Gy /Zc:wchar_t /Zi /Gm- /O2 /sdl- /Zc:inline")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /fp:precise /D _WINDLL /D _MBCS /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /Oi /MD /EHsc /nologo /diagnostics:classic")
endif()

include_directories ("${PROJECT_SOURCE_DIR}/include/")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


add_library(${GPL_LIBRARY_NAME} STATIC ${GPL_SOURCES} ${GPL_HEADERS})

llvm_map_components_to_libnames(llvm_libs all)

# Link against LLVM libraries
target_link_libraries(${GPL_LIBRARY_NAME} ${llvm_libs})

# disable the <libname>_EXPORTS
set_target_properties(${GPL_LIBRARY_NAME} PROPERTIES DEFINE_SYMBOL "")

# copy lib into /tests subdirectory for its testing
add_custom_command(TARGET ${GPL_LIBRARY_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/../${GPL_TESTS_NAME}/bin/$<CONFIGURATION>/"
	COMMAND ${CMAKE_COMMAND} -E copy
	"$<TARGET_FILE:${GPL_LIBRARY_NAME}>"
	"${PROJECT_SOURCE_DIR}/../${GPL_TESTS_NAME}/bin/$<CONFIGURATION>/")
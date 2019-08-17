/*!
	\author Ildar Kasimov
	\date   16.08.2019
	\copyright

	\brief The file contains definition of common stuff

	\todo
*/
#pragma once


#include <gplc.h>
#include <string>


namespace gplc
{
	static struct TVersion
	{
		const U32 mMajor = 0;
		const U32 mMinor = 1;
	} ToolVersion;


	constexpr const C8* Usage[] = 
	{
		"gplc input [options]",
		0
	};


	enum E_PRINT_FLAGS: U32
	{
		PF_SYMTABLE_DUMP    = 0x1,
		PF_COMPILER_TARGETS = 0x2,
	};


	enum class E_EMIT_FLAGS : U32
	{
		EF_LLVM_IR = 0x1,
		EF_LLVM_BC = 0x2,
		EF_ASM     = 0x4,
		EF_NONE    = 0x0
	};


	std::string EmitFlagsToExtensionString(E_EMIT_FLAGS flag);


	typedef struct TCompilerOptions
	{
		std::string  mInputFilename;

		std::string  mOutputFilename;

		U32          mPrintFlags        = 0x0;

		E_EMIT_FLAGS mEmitFlag          = E_EMIT_FLAGS::EF_NONE;

		U8           mOptimizationLevel = 0;
	} TCompilerOptions, *TCompilerOptionsPtr;


	TResult<TCompilerOptions> ParseOptions(int argc, const C8** argv);


	inline std::string GetRedConsoleText(const std::string& text)
	{
		return std::string("\x1B[91m").append(text).append("\033[0m");
	}


	Result InitLLVMInfrastructure();
}
#include "gplcCommon.h"
#include "argparse.h"
#include <iostream>
#include <filesystem>


namespace gplc
{
	TResult<TCompilerOptions> ParseOptions(int argc, const char** argv)
	{
		TCompilerOptions compilerOptions{};
		
		I32 showVersion = 0;

		const C8* pPrintArg    = nullptr;
		const C8* pOutFilename = nullptr;
		const C8* pEmitArg     = nullptr;

		struct argparse_option options[] = {
			OPT_HELP(),
			OPT_GROUP("Basic options"),
			OPT_BOOLEAN('V', "version", &showVersion, "Print version info and exit"),
			OPT_STRING('p', "print", &pPrintArg, "[symtable-dump|targets] Print additional compiler information"),
			OPT_STRING('o', "out", &pOutFilename, "Write output into specified <filename>"),
			OPT_STRING('E', "emit", &pEmitArg, "[llvm-ir|llvm-bc|asm] Emit intermediate representation in one of specified type"),
			OPT_INTEGER('O', "opt-level", &compilerOptions.mOptimizationLevel, "Specify optimization level"),
			OPT_END(),
		};

		struct argparse argparse;
		argparse_init(&argparse, options, Usage, 0);
		argparse_describe(&argparse, "\nThe utility is a compiler of GPL language", "\n");
		argc = argparse_parse(&argparse, argc, argv);

		if (showVersion)
		{
			std::cout << "gplc " << ToolVersion.mMajor << "." << ToolVersion.mMinor << std::endl;

			exit(0);
		}

		// \note parse input files before any option, because argparse library will remove all argv's values after it processes that
		compilerOptions.mInputFilename = argc < 1 ? compilerOptions.mInputFilename : argparse.out[0];

		if (compilerOptions.mInputFilename.empty())
		{
			Panic(GetRedConsoleText("Error:").append(" no input file's found")); // causes termination of the utility
		}

		compilerOptions.mOutputFilename = pOutFilename ? std::filesystem::path(pOutFilename).replace_extension().string() : ""; // store output filename without extension
		compilerOptions.mPrintFlags     = pPrintArg ? ((strcmp(pPrintArg, "symtable-dump") == 0) ? PF_SYMTABLE_DUMP : 
																	((strcmp(pPrintArg, "targets") == 0) ? 
																		PF_COMPILER_TARGETS: 0x0)) : 0x0;
		compilerOptions.mEmitFlag       = pEmitArg ? ((strcmp(pEmitArg, "llvm-ir") == 0) ? 
																	E_EMIT_FLAGS::EF_LLVM_IR : 
																	((strcmp(pEmitArg, "llvm-bc") == 0) ? 
																			E_EMIT_FLAGS::EF_LLVM_BC: 
																			((strcmp(pEmitArg, "asm") == 0) ? 
																						E_EMIT_FLAGS::EF_ASM : 
																						E_EMIT_FLAGS::EF_NONE))) : E_EMIT_FLAGS::EF_NONE;

		compilerOptions.mOptimizationLevel = std::min<U8>(3, std::max<U8>(0, compilerOptions.mOptimizationLevel)); // in range of [0; 3]

		return TOkValue<TCompilerOptions>(compilerOptions);
	}

	std::string EmitFlagsToExtensionString(E_EMIT_FLAGS flag)
	{
		switch (flag)
		{
			case E_EMIT_FLAGS::EF_LLVM_IR:
				return "ll";
			case E_EMIT_FLAGS::EF_LLVM_BC:
				return "bc";
			case E_EMIT_FLAGS::EF_ASM:
				return "s";
		}

		return "";
	}
}
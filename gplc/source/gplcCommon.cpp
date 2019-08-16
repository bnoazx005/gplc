#include "gplcCommon.h"
#include "argparse.h"
#include <iostream>


namespace gplc
{
	TResult<TCompilerOptions> ParseOptions(int argc, const char** argv)
	{
		TCompilerOptions compilerOptions{};

		// \note parse input files before any option, because argparse library will remove all argv's values after it processes that
		if (argc > 1)
		{
			compilerOptions.mInputFilename = argc < 2 ? compilerOptions.mInputFilename : argv[1];

			if (compilerOptions.mInputFilename.empty())
			{
				Panic(GetRedConsoleText("Error:").append(" no input file's found")); // causes termination of the utility
			}
		}
		
		I32 showVersion = 0;

		const C8* pPrintArg = nullptr;

		struct argparse_option options[] = {
			OPT_HELP(),
			OPT_GROUP("Basic options"),
			OPT_BOOLEAN('V', "version", &showVersion, "Print version info and exit"),
			OPT_STRING('p', "print", &pPrintArg, "[symtable-dump|] Print additional compiler information"),
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

		compilerOptions.mPrintFlags = pPrintArg ? ((strcmp(pPrintArg, "symtable-dump") == 0) ? PF_SYMTABLE_DUMP : 0x0) : 0x0;

		return TOkValue<TCompilerOptions>(compilerOptions);
	}

}
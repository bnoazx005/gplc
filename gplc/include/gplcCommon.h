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
		PF_SYMTABLE_DUMP = 0x1,
	};


	typedef struct TCompilerOptions
	{
		std::string mInputFilename;

		U32         mPrintFlags     = 0x0;
	} TCompilerOptions, *TCompilerOptionsPtr;


	TResult<TCompilerOptions> ParseOptions(int argc, const C8** argv);


	inline std::string GetRedConsoleText(const std::string& text)
	{
		return std::string("\x1B[91m").append(text).append("\033[0m");
	}
}
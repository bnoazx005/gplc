#include "gplcCompilerDriver.h"
#include "gplcCommon.h"


using namespace gplc;


int main(int argc, const char** argv)
{
	TCompilerOptions parserOptions = ParseOptions(argc, argv).Get();

	ICompilerDriver* pCompilerDriver = CreateCompilerDriver(parserOptions).Get(); // this will raise panic if fails

	pCompilerDriver->Run({ parserOptions.mInputFilename });
	
	return pCompilerDriver->Free();
}
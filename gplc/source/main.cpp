#include "gplcCompilerDriver.h"


using namespace gplc;


int main(int argc, const char** argv)
{
	// \todo parse options
	ICompilerDriver* pCompilerDriver = CreateCompilerDriver().Get(); // this will raise panic if fails

	pCompilerDriver->Run({ "test.gpls" });

	pCompilerDriver->Free();

	return system("pause");
}
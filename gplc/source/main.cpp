#include "gplcCompilerDriver.h"
#include "argparse.h"


using namespace gplc;

//
//static const char *const usage[] = {
//	"test_argparse [options] [[--] args]",
//	"test_argparse [options]",
//	NULL,
//};

int main(int argc, const char** argv)
{
	/*int force = 0;
	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Basic options"),
		OPT_BOOLEAN('f', "force", &force, "force to do"),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usage, 0);
	argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, argv);*/

	// \todo parse options
	ICompilerDriver* pCompilerDriver = CreateCompilerDriver().Get(); // this will raise panic if fails

	pCompilerDriver->Run({ "Test.gpls" });

	pCompilerDriver->Free();

	return system("pause");
}
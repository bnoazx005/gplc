#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("CLLVMCodeGenerator tests")
{
	ICodeGenerator* pCodeGenerator = new CLLVMCodeGenerator();

	SECTION("TestGenerate_PassEmptyProgram_ReturnsBytecode")
	{
		pCodeGenerator->Generate(new CASTSourceUnitNode());
	}

	delete pCodeGenerator;
}
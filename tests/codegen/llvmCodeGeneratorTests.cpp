#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("CLLVMCodeGenerator tests")
{
	ICodeGenerator* pCodeGenerator = new CLLVMCodeGenerator();

	SECTION("TestGenerate_PassEmptyProgram_ReturnsBytecode")
	{
		pCodeGenerator->Generate(new CASTSourceUnitNode(), new CSymTable());
	}

	SECTION("TestGenerate_PassBinaryExpression_ReturnsBytecode")
	{
		CASTSourceUnitNode* pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTBinaryExpressionNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntLiteral(2))),
														   TT_PLUS,
														   new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntLiteral(3)))));

		pCodeGenerator->Generate(pProgram, new CSymTable());
	}

	delete pCodeGenerator;
}
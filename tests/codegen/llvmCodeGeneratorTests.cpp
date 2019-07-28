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
/*
	SECTION("TestGenerate_PassBinaryExpression_ReturnsBytecode")
	{
		CASTSourceUnitNode* pProgram = new CASTSourceUnitNode();

		ISymTable* pSymTable = new CSymTable();

		pSymTable->CreateScope();
		pSymTable->AddVariable({ "x", nullptr, new CType(CT_INT32, BTS_INT32, 0x0) });
		pSymTable->LeaveScope();

		auto pBlock = new CASTBlockNode();

		pBlock->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
													new CASTBinaryExpressionNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(2))),
														TT_PLUS,
														new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(3))))));

		pProgram->AttachChild(pBlock);

		pCodeGenerator->Generate(pProgram, pSymTable);

		delete pSymTable;
	}
*/
	delete pCodeGenerator;
}
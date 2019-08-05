#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("ConstExprInterpreter Tests")
{
	IConstExprInterpreter* pInterpreter = new CConstExprInterpreter();

	SECTION("TestEval_PassSingleIntegralLiteral_ReturnsItsValue")
	{
		REQUIRE(pInterpreter->Eval(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42))), new CSymTable()).Get() == 42);
		REQUIRE(pInterpreter->Eval(new CASTUnaryExpressionNode(TT_MINUS, new CASTLiteralNode(new CIntValue(42))), new CSymTable()).Get() == -42);
	}

	SECTION("TestEval_PassSingleIdentifier_ReturnsValue")
	{
		ISymTable* pSymTable = new CSymTable();
		pSymTable->AddVariable({ "a", new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42))), new CType(CT_INT32, BTS_INT32, 0x0) });

		REQUIRE(pInterpreter->Eval(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("a", 0x0)), pSymTable).Get() == 42);

		delete pSymTable;
	}

	delete pInterpreter;
}
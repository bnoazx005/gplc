#include <catch2/catch.hpp>
#include <gplc.h>


TEST_CASE("CASTLispyPrinter tests")
{
	gplc::CASTLispyPrinter* pASTPrinter = new gplc::CASTLispyPrinter();

	REQUIRE(pASTPrinter != nullptr);

	SECTION("TestPrint_PassNullptr_ReturnsEmptyString")
	{
		REQUIRE(pASTPrinter->Print(nullptr) == "");
	}

	SECTION("TestPrint_PassEmptyASTTree_Returns()")
	{
		REQUIRE(pASTPrinter->Print(new gplc::CASTSourceUnitNode()) == "()");
	}

	SECTION("TestPrint_PassIdentifier_Returns(define <id> <default_value>)")
	{
		REQUIRE(pASTPrinter->Print(new gplc::CASTIdentifierNode("x")) == "x");
	}

	SECTION("TestPrint_PassSimpleBinaryExpressionAndAssignment_Returns(set! z (+ x (* 2.0 y)))")
	{
		REQUIRE(pASTPrinter->Print(new gplc::CASTAssignmentNode(
										new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("z")),
										new gplc::CASTBinaryExpressionNode(
											new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("x")),
											gplc::TT_PLUS,
											new gplc::CASTBinaryExpressionNode(
												new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTLiteralNode(new gplc::CFloatLiteral(2.0f))),
												gplc::TT_STAR,
												new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("y")))))) == "(set! z (+ x (* 2.000000 y)))");
	}
}
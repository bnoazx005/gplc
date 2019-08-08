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
		REQUIRE(pASTPrinter->Print(new gplc::CASTSourceUnitNode("")) == "()");
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
												new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTLiteralNode(new gplc::CFloatValue(2.0f))),
												gplc::TT_STAR,
												new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("y")))))) == "(set! z (+ x (* 2.000000 y)))");
	}

	SECTION("TestPrint_PassIfStatement_Returns(if (<condition>) (<then-branch>) (<else-branch>))")
	{
		gplc::CASTBlockNode* pThenBlock = new gplc::CASTBlockNode();
		pThenBlock->AttachChild(new gplc::CASTBinaryExpressionNode(
									new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("y")),
									gplc::TT_PLUS,
									new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTLiteralNode(new gplc::CIntValue(2)))));

		gplc::CASTBlockNode* pElseBlock = new gplc::CASTBlockNode();
		pElseBlock->AttachChild(new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTLiteralNode(new gplc::CIntValue(42))));

		REQUIRE(pASTPrinter->Print(new gplc::CASTIfStatementNode(new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("x")), 
																 pThenBlock, pElseBlock)) == "(if x ((+ y 2)) (42))");
	}

	SECTION("TestPrint_PassLoopStatements_Returns(loop (<block>))")
	{
		gplc::CASTBlockNode* pBlock = new gplc::CASTBlockNode();

		pBlock->AttachChild(new gplc::CASTBinaryExpressionNode(
			new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("y")),
			gplc::TT_PLUS,
			new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTLiteralNode(new gplc::CIntValue(2)))));

		REQUIRE(pASTPrinter->Print(new gplc::CASTLoopStatementNode(pBlock)) == "(loop ((+ y 2)))");
		REQUIRE(pASTPrinter->Print(new gplc::CASTWhileLoopStatementNode(new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("x")), pBlock)) == "(while x ((+ y 2)))");
	}

	SECTION("TestPrint_PassReturnStatement_Returns(return <expr>)")
	{
		REQUIRE(pASTPrinter->Print(new gplc::CASTReturnStatementNode(new gplc::CASTBinaryExpressionNode(
											new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTIdentifierNode("y")),
											gplc::TT_PLUS,
											new gplc::CASTUnaryExpressionNode(gplc::TT_DEFAULT, new gplc::CASTLiteralNode(new gplc::CIntValue(2)))))) == "(return (+ y 2))");
	}
}
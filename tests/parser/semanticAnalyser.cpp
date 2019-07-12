#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("CSemanticAnalyser's tests")
{
	ISemanticAnalyser* pSemanticAnalyser = new CSemanticAnalyser();

	ITypeResolver* pTypeResolver = new CTypeResolver();

	REQUIRE(pSemanticAnalyser != nullptr);

	SECTION("TestAnalyze_PassNullptr_ReturnsFalse")
	{
		REQUIRE(!pSemanticAnalyser->Analyze(nullptr, nullptr, new CSymTable()));
		REQUIRE(!pSemanticAnalyser->Analyze(new CASTBlockNode(), nullptr, nullptr));
		REQUIRE(!pSemanticAnalyser->Analyze(nullptr, new CTypeResolver(), nullptr));
	}

	SECTION("TestAnalyze_PassDoubledInitializedVariable_ReturnsFalse")
	{
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));
		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));

		REQUIRE(!pSemanticAnalyser->Analyze(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_INT32)), pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyse_UsageOfUndeclaredVariable_ReturnsFalse")
	{
		auto pExprNode = new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(-0.5))));

		REQUIRE(!pSemanticAnalyser->Analyze(pExprNode, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_CorrectTypesOfLValueAndRValueInSimpleStatement_ReturnsTrue")
	{
		/*
			x : double;

			x = -0.5;
		*/
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));

		auto pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(-0.5)))));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_IncompatibleTypesOfLValueAndRValueInSimpleStatement_ReturnsFalse")
	{
		/* 
			x : double;

			x = -0.5;
		*/
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));

		auto pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
													 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntLiteral(42)))));

		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassComplexExpressionWithCorrectTypes_ReturnTrue")
	{
		/*
			x, y: double;

			y = 2.0;

			x = -0.5 * y + 1.0;
		*/
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));
		pIdentifiersList->AttachChild(new CASTIdentifierNode("y"));

		auto pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("y")),
													 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(1.0f)))));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
													 new CASTBinaryExpressionNode(
														 new CASTBinaryExpressionNode(
																new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(-0.5))),
															    TT_STAR,
																new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("y"))),
														 TT_PLUS,
														 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(1.0f))))));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassComplexExpressionWithIncompatibleTypes_ReturnsFalse")
	{
		/*
			x, y: double;

			y = 2.0;

			x = -0.5 * y + 1.0;
		*/
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));
		pIdentifiersList->AttachChild(new CASTIdentifierNode("y"));

		auto pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("y")),
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(1.0f)))));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
			new CASTBinaryExpressionNode(
				new CASTBinaryExpressionNode(
					new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(-0.5))),
					TT_STAR,
					new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("y"))),
				TT_PLUS,
				new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntLiteral(1.0f))))));

		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassDefinitionOfBuiltinTypeWithCorrectValue_ReturnsTrue")
	{
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));

		/*
			x : double = -0.5;
		*/
		REQUIRE(pSemanticAnalyser->Analyze(new CASTDefinitionNode(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_DOUBLE)),
																  new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleLiteral(-0.5)))), 
										   pTypeResolver, new CSymTable()));
	}

	delete pTypeResolver;
	delete pSemanticAnalyser;
}
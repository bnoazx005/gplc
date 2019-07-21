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
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(-0.5))));

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
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(-0.5)))));

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
													 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42)))));

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
													 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(1.0f)))));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
													 new CASTBinaryExpressionNode(
														 new CASTBinaryExpressionNode(
																new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(-0.5))),
															    TT_STAR,
																new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("y"))),
														 TT_PLUS,
														 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(1.0f))))));

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
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(1.0f)))));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
			new CASTBinaryExpressionNode(
				new CASTBinaryExpressionNode(
					new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(-0.5))),
					TT_STAR,
					new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("y"))),
				TT_PLUS,
				new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(1))))));

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
																  new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(-0.5)))), 
										   pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassIfStatementWithIncorrectCondition_ReturnsFalse")
	{
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));

		/*
			x : double = -0.5;

			if x {
				x = 1.0;
			}
		*/
		auto pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTDefinitionNode(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_DOUBLE)),
													 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(-0.5)))));

		auto pThenBlock = new CASTBlockNode();

		pThenBlock->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
													   new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(1.0f)))));

		pProgram->AttachChild(new CASTIfStatementNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")), pThenBlock, nullptr));

		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassIfStatementWithBooleanCondition_ReturnsTrue")
	{
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));

		/*
			x : double = -0.5;

			if x < 0 {
				x = 1.0;
			}
		*/
		auto pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTDefinitionNode(new CASTDeclarationNode(pIdentifiersList, new CASTTypeNode(NT_DOUBLE)),
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(-0.5)))));

		auto pThenBlock = new CASTBlockNode();

		pThenBlock->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(1.0f)))));

		pProgram->AttachChild(new CASTIfStatementNode(new CASTBinaryExpressionNode(
																	new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")), 
																	TT_LE,
																	new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(0.0)))),
													  pThenBlock, nullptr));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassCorectFunctionDeclaration_ReturnsTrue")
	{
		/*
			f: (x: int32) -> int32;
		*/

		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier   = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("f"));

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32)));

		REQUIRE(pSemanticAnalyser->Analyze(new CASTDeclarationNode(pIdentifiersList, new CASTFunctionDeclNode(
																								nullptr,
																								pFuncArgs,
																								new CASTTypeNode(NT_INT32))), 
										   pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassCorectFunctionArgs_ReturnsTrue")
	{
		/*
			(x: int32);
		*/
		auto pArgIdentifier   = new CASTNode(NT_IDENTIFIERS_LIST);

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32)));

		REQUIRE(pSemanticAnalyser->Analyze(pFuncArgs, pTypeResolver, new CSymTable()));
	}

	//SECTION("TestAnalyze_PassFunctionArgsWithUndefinedType_ReturnsFalse")
	//{
	//	/*
	//		(x: Type);
	//	*/
	//	auto pArgIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

	//	CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

	//	pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

	//	pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTIdentifierNode("TType")));

	//	REQUIRE(!pSemanticAnalyser->Analyze(pFuncArgs, pTypeResolver, new CSymTable()));
	//}

	SECTION("TestAnalyze_PassCorrectFunctionDefinition_ReturnsTrue")
	{
		/*
			f: (x: int32) -> int32 = (x: int32) -> int32 {
				return 42;
			};
		*/

		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("f"));

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32)));

		auto pFuncDeclaration = new CASTFunctionDeclNode(nullptr, pFuncArgs, new CASTTypeNode(NT_INT32));;

		auto pFuncBody = new CASTBlockNode();

		pFuncBody->AttachChild(new CASTReturnStatementNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42)))));

		REQUIRE(pSemanticAnalyser->Analyze(new CASTFuncDefinitionNode(new CASTDeclarationNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody),
										   pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassIncompatibleFunctionDefinition_ReturnsFalse")
	{
		/*
			f: (x: int32) -> int32 = () -> int32 {
				return 42;
			};
		*/

		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("f"));

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32)));

		auto pFuncDeclaration = new CASTFunctionDeclNode(nullptr, pFuncArgs, new CASTTypeNode(NT_INT32));;

		auto pFuncBody = new CASTBlockNode();

		pFuncBody->AttachChild(new CASTReturnStatementNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42)))));

		REQUIRE(!pSemanticAnalyser->Analyze(new CASTFuncDefinitionNode(new CASTDeclarationNode(pIdentifiersList, pFuncDeclaration), 
																	   new CASTFunctionDeclNode(nullptr, new CASTFunctionArgsNode(), new CASTTypeNode(NT_INT32)),
																	   pFuncBody),
										    pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassFunctionCallIncompatibleWithExpression_ReturnsFalse")
	{
		/*
			f: (x: int32) -> int32 = (x: int32) -> int32 {
				return 42;
			};

			x : double = 0.0;

			x = x + f(0);
		*/

		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("f"));

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32)));

		auto pFuncDeclaration = new CASTFunctionDeclNode(nullptr, pFuncArgs, new CASTTypeNode(NT_INT32));;

		auto pFuncBody = new CASTBlockNode();

		pFuncBody->AttachChild(new CASTReturnStatementNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42)))));

		auto pXIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);
		pXIdentifier->AttachChild(new CASTIdentifierNode("x"));

		auto pProgram = new CASTSourceUnitNode();

		auto pCallArgs = new CASTNode(NT_FUNC_ARGS);
		pCallArgs->AttachChild(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(0))));

		pProgram->AttachChild(new CASTFuncDefinitionNode(new CASTDeclarationNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody));
		pProgram->AttachChild(new CASTDefinitionNode(new CASTDeclarationNode(pXIdentifier, new CASTTypeNode(NT_DOUBLE)), 
													 new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CDoubleValue(0.0)))));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
													 new CASTBinaryExpressionNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
																				  TT_PLUS,
																				  new CASTUnaryExpressionNode(TT_DEFAULT, 
																						new CASTFunctionCallNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("f")),
																										         pCallArgs)))));

		

		auto pSymTable = new CSymTable();
		
		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable));

		delete pSymTable;
	}

	SECTION("TestAnalyze_PassFunctionCallWithExpression_ReturnsTrue")
	{
		/*
			f: (x: int32) -> int32 = (x: int32) -> int32 {
				return 42;
			};

			x : int32 = 0;

			x = x + f(0);
		*/

		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("f"));

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32), AV_FUNC_ARG_DECL));

		auto pFuncDeclaration = new CASTFunctionDeclNode(nullptr, pFuncArgs, new CASTTypeNode(NT_INT32));;

		auto pFuncBody = new CASTBlockNode();

		pFuncBody->AttachChild(new CASTReturnStatementNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42)))));

		auto pXIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);
		pXIdentifier->AttachChild(new CASTIdentifierNode("x"));

		auto pProgram = new CASTSourceUnitNode();

		auto pCallArgs = new CASTNode(NT_FUNC_ARGS);
		pCallArgs->AttachChild(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(0))));

		pProgram->AttachChild(new CASTFuncDefinitionNode(new CASTDeclarationNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody));
		pProgram->AttachChild(new CASTDefinitionNode(new CASTDeclarationNode(pXIdentifier, new CASTTypeNode(NT_INT32)),
			new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(0)))));
		pProgram->AttachChild(new CASTAssignmentNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
													new CASTBinaryExpressionNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("x")),
														TT_PLUS,
														new CASTUnaryExpressionNode(TT_DEFAULT,
															new CASTFunctionCallNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTIdentifierNode("f")),
																pCallArgs)))));
		
		pSemanticAnalyser->OnErrorOutput += [](E_SEMANTIC_ANALYSER_ERRORS err)
		{
			REQUIRE(false);
		};

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_DefineTwoVariablesWithSameFunctionDefinition_ReturnsFalse")
	{
		/*
			f, g: (x: int32) -> int32 = (x: int32) -> int32 {
				return 42;
			};
		*/

		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("f"));
		pIdentifiersList->AttachChild(new CASTIdentifierNode("g"));

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32)));

		auto pFuncDeclaration = new CASTFunctionDeclNode(nullptr, pFuncArgs, new CASTTypeNode(NT_INT32));;

		auto pFuncBody = new CASTBlockNode();

		pFuncBody->AttachChild(new CASTReturnStatementNode(new CASTUnaryExpressionNode(TT_DEFAULT, new CASTLiteralNode(new CIntValue(42)))));

		REQUIRE(!pSemanticAnalyser->Analyze(new CASTFuncDefinitionNode(new CASTDeclarationNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody),
											pTypeResolver, new CSymTable()));
	}

	SECTION("TestAnalyze_PassInvalidFunctionDeclaration_ReturnsFalse")
	{
		/*
			f: (x, y: int32) -> int32;
		*/

		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("f"));

		CASTFunctionArgsNode* pFuncArgs = new CASTFunctionArgsNode();

		pArgIdentifier->AttachChild(new CASTIdentifierNode("x"));
		pArgIdentifier->AttachChild(new CASTIdentifierNode("y"));

		pFuncArgs->AttachChild(new CASTDeclarationNode(pArgIdentifier, new CASTTypeNode(NT_INT32), AV_FUNC_ARG_DECL));

		REQUIRE(!pSemanticAnalyser->Analyze(new CASTDeclarationNode(pIdentifiersList, new CASTFunctionDeclNode(
											nullptr,
											pFuncArgs,
											new CASTTypeNode(NT_INT32))),
											pTypeResolver, new CSymTable()));
	}

	delete pTypeResolver;
	delete pSemanticAnalyser;
}
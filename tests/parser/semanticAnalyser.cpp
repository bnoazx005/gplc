#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("CSemanticAnalyser's tests")
{
	ISemanticAnalyser* pSemanticAnalyser = new CSemanticAnalyser();

	gplc::IASTNodesFactory* pNodesFactory = new gplc::CASTNodesFactory();

	ITypeResolver* pTypeResolver = new CTypeResolver();

	IConstExprInterpreter* pInterpreter = new CConstExprInterpreter();

	ISymTable* pSymTable = new CSymTable();

	ITypesFactory* pTypesFactory = new CTypesFactory();

	pTypeResolver->Init(pSymTable, pInterpreter, pTypesFactory);
		
	REQUIRE(pSemanticAnalyser != nullptr);

	SECTION("TestAnalyze_PassNullptr_ReturnsFalse")
	{
		REQUIRE(!pSemanticAnalyser->Analyze(nullptr, nullptr, pSymTable, pNodesFactory));
		REQUIRE(!pSemanticAnalyser->Analyze(pNodesFactory->CreateBlockNode(), nullptr, nullptr, pNodesFactory));
		REQUIRE(!pSemanticAnalyser->Analyze(nullptr, pTypeResolver, nullptr, pNodesFactory));
	}

	SECTION("TestAnalyze_PassDoubledInitializedVariable_ReturnsFalse")
	{
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));
		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));

		REQUIRE(!pSemanticAnalyser->Analyze(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_INT32)), pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyse_UsageOfUndeclaredVariable_ReturnsFalse")
	{
		auto pExprNode = pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
														 pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(-0.5))));

		REQUIRE(!pSemanticAnalyser->Analyze(pExprNode, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_CorrectTypesOfLValueAndRValueInSimpleStatement_ReturnsTrue")
	{
		/*
			x : double;

			x = -0.5;
		*/
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		pProgram->AttachChild(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
															  pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(-0.5)))));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_IncompatibleTypesOfLValueAndRValueInSimpleStatement_ReturnsFalse")
	{
		/* 
			x : double;

			x = 42;
		*/

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		pProgram->AttachChild(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
															  pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(42)))));

		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassComplexExpressionWithCorrectTypes_ReturnTrue")
	{
		/*
			x, y: double;

			y = 2.0;

			x = -0.5 * y + 1.0;
		*/
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));
		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("y"));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		pProgram->AttachChild(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("y")),
															  pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(1.0f)))));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
															  pNodesFactory->CreateBinaryExpr(
																				pNodesFactory->CreateBinaryExpr(
																						pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(-0.5))),
																						TT_STAR,
																						pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("y"))),
																				TT_PLUS,
																				pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(1.0f))))));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassComplexExpressionWithIncompatibleTypes_ReturnsFalse")
	{
		/*
			x, y: double;

			y = 2.0;

			x = -0.5 * y + 1;
		*/
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));
		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("y"));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		pProgram->AttachChild(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_DOUBLE)));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("y")),
															  pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(1.0f)))));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
															  pNodesFactory->CreateBinaryExpr(
																	pNodesFactory->CreateBinaryExpr(
																		pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(-0.5))),
																		TT_STAR,
																		pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("y"))),
																	TT_PLUS,
																	pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(1))))));

		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassDefinitionOfBuiltinTypeWithCorrectValue_ReturnsTrue")
	{
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));

		/*
			x : double = -0.5;
		*/
		REQUIRE(pSemanticAnalyser->Analyze(pNodesFactory->CreateDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_DOUBLE)),
																		pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(-0.5)))), 
										   pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassIfStatementWithIncorrectCondition_ReturnsFalse")
	{
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));

		/*
			x : double = -0.5;

			if x {
				x = 1.0;
			}
		*/
		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		pProgram->AttachChild(pNodesFactory->CreateDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_DOUBLE)),
														   pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(-0.5)))));

		auto pThenBlock = pNodesFactory->CreateBlockNode();

		pThenBlock->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
																pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(1.0f)))));

		pProgram->AttachChild(pNodesFactory->CreateIfStmtNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")), pThenBlock, nullptr));

		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassIfStatementWithBooleanCondition_ReturnsTrue")
	{
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));

		/*
			x : double = -0.5;

			if x < 0 {
				x = 1.0;
			}
		*/
		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		pProgram->AttachChild(pNodesFactory->CreateDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateTypeNode(NT_DOUBLE)),
			pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(-0.5)))));

		auto pThenBlock = pNodesFactory->CreateBlockNode();

		pThenBlock->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
			pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(1.0f)))));

		pProgram->AttachChild(pNodesFactory->CreateIfStmtNode(pNodesFactory->CreateBinaryExpr(
																	pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")), 
																	TT_LE,
																	pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(0.0)))),
													  pThenBlock, nullptr));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassCorectFunctionDeclaration_ReturnsTrue")
	{
		/*
			f: (x: int32) -> int32;
		*/

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier   = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("f"));

		auto pFuncArgs = pNodesFactory->CreateFuncArgsNode();

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32)));

		REQUIRE(pSemanticAnalyser->Analyze(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateFuncDeclNode(
																								nullptr,
																								pFuncArgs,
																								pNodesFactory->CreateTypeNode(NT_INT32))), 
										   pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassCorectFunctionArgs_ReturnsTrue")
	{
		/*
			(x: int32);
		*/
		auto pArgIdentifier = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		auto pFuncArgs = pNodesFactory->CreateFuncArgsNode();
		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32)));

		REQUIRE(pSemanticAnalyser->Analyze(pFuncArgs, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassFunctionArgsWithUndefinedType_ReturnsFalse")
	{
		/*
			(x: Type);
		*/
		auto pArgIdentifier = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		CASTFunctionArgsNode* pFuncArgs = pNodesFactory->CreateFuncArgsNode();

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateIdNode("TType")));

		REQUIRE(!pSemanticAnalyser->Analyze(pFuncArgs, pTypeResolver, new CSymTable(), pNodesFactory));
	}

	SECTION("TestAnalyze_PassCorrectFunctionDefinition_ReturnsTrue")
	{
		/*
			f: (x: int32) -> int32 = (x: int32) -> int32 {
				return 42;
			};
		*/

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("f"));

		auto pFuncArgs = pNodesFactory->CreateFuncArgsNode();

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32)));

		auto pFuncDeclaration = pNodesFactory->CreateFuncDeclNode(nullptr, pFuncArgs, pNodesFactory->CreateTypeNode(NT_INT32));;

		auto pFuncBody = pNodesFactory->CreateBlockNode();

		pFuncBody->AttachChild(pNodesFactory->CreateReturnStmtNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(42)))));

		REQUIRE(pSemanticAnalyser->Analyze(pNodesFactory->CreateFuncDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody),
										   pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassIncompatibleFunctionDefinition_ReturnsFalse")
	{
		/*
			f: (x: int32) -> int32 = () -> int32 {
				return 42;
			};
		*/

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("f"));

		auto pFuncArgs = pNodesFactory->CreateFuncArgsNode();

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32)));

		auto pFuncDeclaration = pNodesFactory->CreateFuncDeclNode(nullptr, pFuncArgs, pNodesFactory->CreateTypeNode(NT_INT32));;

		auto pFuncBody = pNodesFactory->CreateBlockNode();

		pFuncBody->AttachChild(pNodesFactory->CreateReturnStmtNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(42)))));

		REQUIRE(!pSemanticAnalyser->Analyze(pNodesFactory->CreateFuncDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pFuncDeclaration), 
																	   pNodesFactory->CreateFuncDeclNode(nullptr, pNodesFactory->CreateFuncArgsNode(), pNodesFactory->CreateTypeNode(NT_INT32)),
																	   pFuncBody),
										    pTypeResolver, pSymTable, pNodesFactory));
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

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("f"));

		auto pFuncArgs = pNodesFactory->CreateFuncArgsNode();

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32)));

		auto pFuncDeclaration = pNodesFactory->CreateFuncDeclNode(nullptr, pFuncArgs, pNodesFactory->CreateTypeNode(NT_INT32));;

		auto pFuncBody = pNodesFactory->CreateBlockNode();

		pFuncBody->AttachChild(pNodesFactory->CreateReturnStmtNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(42)))));

		auto pXIdentifier = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		pXIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		auto pCallArgs = pNodesFactory->CreateNode(NT_FUNC_ARGS);
		pCallArgs->AttachChild(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0))));

		pProgram->AttachChild(pNodesFactory->CreateFuncDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody));
		pProgram->AttachChild(pNodesFactory->CreateDefNode(pNodesFactory->CreateDeclNode(pXIdentifier, pNodesFactory->CreateTypeNode(NT_DOUBLE)), 
													 pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CDoubleValue(0.0)))));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
													 pNodesFactory->CreateBinaryExpr(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
																				  TT_PLUS,
																				  pNodesFactory->CreateUnaryExpr(TT_DEFAULT, 
																						pNodesFactory->CreateFuncCallNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("f")),
																										         pCallArgs)))));
		
		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
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

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier   = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("f"));

		auto pFuncArgs = pNodesFactory->CreateFuncArgsNode();

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32), AV_FUNC_ARG_DECL));

		auto pFuncDeclaration = pNodesFactory->CreateFuncDeclNode(nullptr, pFuncArgs, pNodesFactory->CreateTypeNode(NT_INT32));;

		auto pFuncBody = pNodesFactory->CreateBlockNode();

		pFuncBody->AttachChild(pNodesFactory->CreateReturnStmtNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(42)))));

		auto pXIdentifier = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		pXIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();

		auto pCallArgs = pNodesFactory->CreateNode(NT_FUNC_ARGS);
		pCallArgs->AttachChild(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0))));

		pProgram->AttachChild(pNodesFactory->CreateFuncDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody));
		pProgram->AttachChild(pNodesFactory->CreateDefNode(pNodesFactory->CreateDeclNode(pXIdentifier, pNodesFactory->CreateTypeNode(NT_INT32)),
			pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0)))));
		pProgram->AttachChild(pNodesFactory->CreateAssignNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
													pNodesFactory->CreateBinaryExpr(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("x")),
														TT_PLUS,
														pNodesFactory->CreateUnaryExpr(TT_DEFAULT,
															pNodesFactory->CreateFuncCallNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateIdNode("f")),
																pCallArgs)))));
		
		pSemanticAnalyser->OnErrorOutput += [](auto err)
		{
			REQUIRE(false);
		};

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_DefineTwoVariablesWithSameFunctionDefinition_ReturnsFalse")
	{
		/*
			f, g: (x: int32) -> int32 = (x: int32) -> int32 {
				return 42;
			};
		*/

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier   = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("f"));
		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("g"));

		auto pFuncArgs = pNodesFactory->CreateFuncArgsNode();

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));

		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32)));

		auto pFuncDeclaration = pNodesFactory->CreateFuncDeclNode(nullptr, pFuncArgs, pNodesFactory->CreateTypeNode(NT_INT32));;

		auto pFuncBody = pNodesFactory->CreateBlockNode();
		pFuncBody->AttachChild(pNodesFactory->CreateReturnStmtNode(pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(42)))));

		REQUIRE(!pSemanticAnalyser->Analyze(pNodesFactory->CreateFuncDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList, pFuncDeclaration), pFuncDeclaration, pFuncBody),
											pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassInvalidFunctionDeclaration_ReturnsFalse")
	{
		/*
			f: (x, y: int32) -> int32;
		*/

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		auto pArgIdentifier   = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("f"));

		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("x"));
		pArgIdentifier->AttachChild(pNodesFactory->CreateIdNode("y"));

		CASTFunctionArgsNode* pFuncArgs = pNodesFactory->CreateFuncArgsNode();
		pFuncArgs->AttachChild(pNodesFactory->CreateDeclNode(pArgIdentifier, pNodesFactory->CreateTypeNode(NT_INT32), AV_FUNC_ARG_DECL));

		REQUIRE(!pSemanticAnalyser->Analyze(pNodesFactory->CreateDeclNode(pIdentifiersList, pNodesFactory->CreateFuncDeclNode(
											nullptr,
											pFuncArgs,
											pNodesFactory->CreateTypeNode(NT_INT32))),
											pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassCorrectShortEnumDeclaration_ReturnsTrue")
	{
		/*!
			enum NewEnum {
				first, second
			}
		*/
		pSymTable->CreateNamedScope("NewEnum");
		pSymTable->AddVariable({ "first", nullptr, nullptr });
		pSymTable->AddVariable({ "second", nullptr, nullptr });
		pSymTable->LeaveScope();

		REQUIRE(pSemanticAnalyser->Analyze(pNodesFactory->CreateEnumDeclNode(pNodesFactory->CreateIdNode("NewEnum")), pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_CorrectPointerDeclaration_ReturnsTrue")
	{
		/*
			x : double*;
		*/
		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("x"));
		
		auto pPointerType = pNodesFactory->CreatePointerTypeNode(pNodesFactory->CreateTypeNode(NT_DOUBLE));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();
		pProgram->AttachChild(pNodesFactory->CreateDeclNode(pIdentifiersList, pPointerType));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassBreakOperatorOutsideOfLoop_ReturnsFalse")
	{
		/*
			break;
		*/

		REQUIRE(!pSemanticAnalyser->Analyze(pNodesFactory->CreateBreakNode(), pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassBreakOperatorWithinOfLoop_ReturnsTrue")
	{
		/*
			loop {
				break;
			}
		*/
		
		auto pLoopBody = pNodesFactory->CreateBlockNode();
		pLoopBody->AttachChild(pNodesFactory->CreateBreakNode());

		auto pProgram = pNodesFactory->CreateSourceUnitNode();
		pProgram->AttachChild(pNodesFactory->CreateLoopStmtNode(pLoopBody));

		REQUIRE(pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	SECTION("TestAnalyze_PassDereferencingOfInvalidPointer_ReturnsFalse")
	{
		/*
			pPtr : @uninit int32*;
			val := *pPtr;
		*/

		auto pIdentifiersList = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		pIdentifiersList->AttachChild(pNodesFactory->CreateIdNode("pPtr", AV_KEEP_UNINITIALIZED));

		auto pPointerType = pNodesFactory->CreatePointerTypeNode(pNodesFactory->CreateTypeNode(NT_INT32));

		auto pProgram = pNodesFactory->CreateSourceUnitNode();
		pProgram->AttachChild(pNodesFactory->CreateDeclNode(pIdentifiersList, pPointerType));

		auto pIdentifiersList2 = pNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);
		pIdentifiersList2->AttachChild(pNodesFactory->CreateIdNode("val"));

		pProgram->AttachChild(pNodesFactory->CreateDefNode(pNodesFactory->CreateDeclNode(pIdentifiersList2, pNodesFactory->CreateTypeNode(NT_INT32)), 
							  pNodesFactory->CreateUnaryExpr(TT_STAR, pNodesFactory->CreateIdNode("pPtr"))));

		REQUIRE(!pSemanticAnalyser->Analyze(pProgram, pTypeResolver, pSymTable, pNodesFactory));
	}

	delete pTypesFactory;
	delete pSymTable;
	delete pNodesFactory;
	delete pTypeResolver;
	delete pInterpreter;
	delete pSemanticAnalyser;
}
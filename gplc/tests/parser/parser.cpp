#include <catch.hpp>
#include <gplc.h>
#include "stubLexer.h"


void OnError(const gplc::TParserErrorInfo& errorInfo)
{
	REQUIRE(false);
}


TEST_CASE("Parser's tests")
{
	gplc::TParserErrorInfo* pErrorInfo = nullptr;
	
	gplc::CParser* pParser = new gplc::CParser();

	pParser->OnErrorOutput += OnError;

	SECTION("TestParse_PassCorrectSequence_ReturnsCorrectAST")
	{
		gplc::CASTNode* pMain = pParser->Parse(new CStubLexer(
			{
			//fill in the tokens' vector
			new gplc::CIdentifierToken("x", 0),
			new gplc::CToken(gplc::TT_COMMA, 1),
			new gplc::CIdentifierToken("y", 2),
			new gplc::CToken(gplc::TT_COLON, 3),
			new gplc::CToken(gplc::TT_INT32_TYPE, 4),
			new gplc::CToken(gplc::TT_SEMICOLON, 5),
			}), new gplc::CSymTable());

		REQUIRE(pMain != nullptr);

		//delete pMain;
	}

	SECTION("TestParse_PassCorrectExprSequence_ReturnsCorrectAST")
	{
		gplc::CASTNode* pMain = pParser->Parse(new CStubLexer(
			{
				//fill in the tokens' vector
				new gplc::CIdentifierToken("z", 0),
				new gplc::CToken(gplc::TT_ASSIGN_OP, 1),
				new gplc::CIdentifierToken("x", 2),
				new gplc::CToken(gplc::TT_PLUS, 3),
				new gplc::CLiteralToken(new gplc::CFloatLiteral(2.0f), 4),
				new gplc::CToken(gplc::TT_STAR, 5),
				new gplc::CIdentifierToken("y", 6),
				new gplc::CToken(gplc::TT_SEMICOLON, 7),
			}), new gplc::CSymTable());

		REQUIRE(pMain != nullptr);
	}

	SECTION("TestParse_PassCodeWithBlock_ReturnsCorrectAST")
	{
		gplc::CASTNode* pMain = pParser->Parse(new CStubLexer(
			{
				//fill in the tokens' vector
				new gplc::CIdentifierToken("z", 0),
				new gplc::CToken(gplc::TT_ASSIGN_OP, 1),
				new gplc::CIdentifierToken("x", 2),
				new gplc::CToken(gplc::TT_PLUS, 3),
				new gplc::CLiteralToken(new gplc::CFloatLiteral(2.0f), 4),
				new gplc::CToken(gplc::TT_STAR, 5),
				new gplc::CIdentifierToken("y", 6),
				new gplc::CToken(gplc::TT_SEMICOLON, 7),
				new gplc::CToken(gplc::TT_OPEN_BRACE, 8),
				new gplc::CIdentifierToken("x", 9),
				new gplc::CToken(gplc::TT_COLON, 10),
				new gplc::CToken(gplc::TT_INT32_TYPE, 11),
				new gplc::CToken(gplc::TT_SEMICOLON, 12),	
				new gplc::CToken(gplc::TT_CLOSE_BRACE, 13)
			}), new gplc::CSymTable());

		REQUIRE(pMain != nullptr);
		REQUIRE(pMain->GetChildrenCount() == 2);
	}

	SECTION("TestParse_PassSimpleIfStatement_ReturnsCorrectAST")
	{
		gplc::CASTNode* pMain = pParser->Parse(new CStubLexer(
			{
				//fill in the tokens' vector
				new gplc::CToken(gplc::TT_IF_KEYWORD, 0),
				new gplc::CIdentifierToken("x", 1),
				new gplc::CToken(gplc::TT_OPEN_BRACE, 2),
				new gplc::CIdentifierToken("y", 3),
				new gplc::CToken(gplc::TT_ASSIGN_OP, 4),
				new gplc::CLiteralToken(new gplc::CIntLiteral(42), 5),
				new gplc::CToken(gplc::TT_SEMICOLON, 6),
				new gplc::CToken(gplc::TT_CLOSE_BRACE, 7),
			}), new gplc::CSymTable());

		REQUIRE(pMain != nullptr);
	}

	SECTION("TestParse_PassSimpleIfElseStatement_ReturnsCorrectAST")
	{
		gplc::CASTNode* pMain = pParser->Parse(new CStubLexer(
			{
				//fill in the tokens' vector
				new gplc::CToken(gplc::TT_IF_KEYWORD, 0),
				new gplc::CIdentifierToken("x", 1),
				new gplc::CToken(gplc::TT_OPEN_BRACE, 2),
				new gplc::CIdentifierToken("y", 3),
				new gplc::CToken(gplc::TT_ASSIGN_OP, 4),
				new gplc::CLiteralToken(new gplc::CIntLiteral(42), 5),
				new gplc::CToken(gplc::TT_SEMICOLON, 6),
				new gplc::CToken(gplc::TT_CLOSE_BRACE, 7),
				new gplc::CToken(gplc::TT_ELSE_KEYWORD, 8),
				new gplc::CToken(gplc::TT_OPEN_BRACE, 9),
				new gplc::CIdentifierToken("y", 10),
				new gplc::CToken(gplc::TT_ASSIGN_OP, 11),
				new gplc::CLiteralToken(new gplc::CIntLiteral(24), 12),
				new gplc::CToken(gplc::TT_SEMICOLON, 13),
				new gplc::CToken(gplc::TT_CLOSE_BRACE, 14),
			}), new gplc::CSymTable());

		REQUIRE(pMain != nullptr);
	}

	SECTION("TestParse_PassFullFunctionDecl_ReturnsCorrectAST")
	{
		gplc::CASTNode* pMain = pParser->Parse(new CStubLexer(
			{
				//the sequence below specifies the following declaration of a function: f: [x, y](z: int32) -> int32;
				new gplc::CIdentifierToken("f", 0),
				new gplc::CToken(gplc::TT_COLON, 1),
				new gplc::CToken(gplc::TT_OPEN_SQR_BRACE, 2),
				new gplc::CIdentifierToken("x", 3),
				new gplc::CToken(gplc::TT_COMMA, 4),
				new gplc::CIdentifierToken("y", 5),
				new gplc::CToken(gplc::TT_CLOSE_SQR_BRACE, 6),
				new gplc::CToken(gplc::TT_OPEN_BRACKET, 7),
				new gplc::CIdentifierToken("z", 8),
				new gplc::CToken(gplc::TT_COLON, 9),
				new gplc::CToken(gplc::TT_INT32_TYPE, 10),
				new gplc::CToken(gplc::TT_CLOSE_BRACKET, 11),
				new gplc::CToken(gplc::TT_ARROW, 12),
				new gplc::CToken(gplc::TT_INT32_TYPE, 13),
				new gplc::CToken(gplc::TT_SEMICOLON, 14),
			}), new gplc::CSymTable());

		REQUIRE(pMain != nullptr);
	}

	SECTION("TestParse_PassFuncCall_ReturnsCorrectAST")
	{
		gplc::CASTNode* pMain = pParser->Parse(new CStubLexer(
			{
				//the sequence below specifies the following declaration of a function: f: [x, y](z: int32) -> int32;
				new gplc::CIdentifierToken("f", 0),
				new gplc::CToken(gplc::TT_OPEN_BRACKET, 7),
				new gplc::CIdentifierToken("x", 3),
				new gplc::CToken(gplc::TT_COMMA, 4),
				new gplc::CIdentifierToken("y", 5),
				new gplc::CToken(gplc::TT_COMMA, 4),
				new gplc::CIdentifierToken("z", 8),
				new gplc::CToken(gplc::TT_CLOSE_BRACKET, 11),
				new gplc::CToken(gplc::TT_SEMICOLON, 14),
			}), new gplc::CSymTable());

		REQUIRE(pMain != nullptr);
	}

	if (pErrorInfo != nullptr)
	{
		delete pErrorInfo;
	}
		
	delete pParser;
}
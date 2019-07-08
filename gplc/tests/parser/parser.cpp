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

	if (pErrorInfo != nullptr)
	{
		delete pErrorInfo;
	}
		
	delete pParser;
}
#include <catch.hpp>
#include <gplc.h>
#include "stubLexer.h"


TEST_CASE("Parser's tests")
{
	gplc::TParserErrorInfo* pErrorInfo = nullptr;

	CStubLexer* pStubLexerObject = new CStubLexer();
	pStubLexerObject->Init("identifier", ".tokens");

	gplc::CParser* pParser = new gplc::CParser();

	SECTION("Parsing")
	{
		gplc::CASTNode* pMain = pParser->Parse(pStubLexerObject);

		REQUIRE(pMain != nullptr);

		//delete pMain;
	}

	if (pErrorInfo != nullptr)
	{
		delete pErrorInfo;
	}
		
	delete pParser;
	delete pStubLexerObject;
}
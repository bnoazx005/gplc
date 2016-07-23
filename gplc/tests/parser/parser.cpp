#include <catch.hpp>
#include <gplc.h>
#include "stubLexer.h"


TEST_CASE("Parser's tests")
{
	gplc::TLexerErrorInfo error;
	gplc::TParserErrorInfo* errorInfo;

	CStubLexer* pStubLexerObject = new CStubLexer();
	pStubLexerObject->Init(L"identifier", L".tokens", &error);

	gplc::CParser* pParser = new gplc::CParser();

	SECTION("Parsing")
	{
		//gplc::CASTNode* pMain = pParser->Parse(pStubLexerObject, errorInfo);

		//REQUIRE(pMain != nullptr);
	}
	
	delete pParser;
	delete pStubLexerObject;
}
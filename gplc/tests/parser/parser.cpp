#include <catch.hpp>
#include <gplc.h>
#include "stubLexer.h"


TEST_CASE("Parser's tests")
{
	gplc::TLexerErrorInfo error;

	CStubLexer* pStubLexerObject = new CStubLexer();
	pStubLexerObject->Init(L"identifier", L".tokens", &error);

	gplc::CParser* pParser = new gplc::CParser();
	
	delete pParser;
	delete pStubLexerObject;
}
#include <catch.hpp>
#include <gplc.h>


TEST_CASE("lexer")
{
	gplc::CLexer* pLexer = new gplc::CLexer();

	REQUIRE(pLexer != nullptr);

	REQUIRE(pLexer->Init(L"42") == gplc::RV_SUCCESS);

	delete pLexer;
}
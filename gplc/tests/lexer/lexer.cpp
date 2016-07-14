#include <catch.hpp>
#include <gplc.h>


TEST_CASE("Lexer's tests")
{
	gplc::CLexer* pLexer = new gplc::CLexer();

	REQUIRE(pLexer != nullptr);
	REQUIRE(pLexer->Init(L"42") == gplc::RV_FAIL);
	
	SECTION("Correct identifier test")
	{
		REQUIRE(pLexer->Init(L"identifier") == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"identifier");
	}

	SECTION("Invalid \'digit at the begining of the identifier\' identifier test")
	{
		REQUIRE(pLexer->Init(L"9identifier") == gplc::RV_FAIL);
	}

	delete pLexer;
}
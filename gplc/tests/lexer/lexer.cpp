#include <catch.hpp>
#include <gplc.h>


TEST_CASE("Lexer's tests")
{
	gplc::TLexerErrorInfo error;
	gplc::CLexer* pLexer = new gplc::CLexer();

	REQUIRE(pLexer != nullptr);
	REQUIRE(pLexer->Init(L"42", &error) == gplc::RV_FAIL);
	
	SECTION("Correct identifier test")
	{
		REQUIRE(pLexer->Init(L"identifier", &error) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"identifier");
	}

	SECTION("Correct identifier with digits test")
	{
		REQUIRE(pLexer->Init(L"identifier42 identi42fier", &error) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"identifier42");

		pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"identi42fier");
	}

	SECTION("Correct identifier with underscope test")
	{
		REQUIRE(pLexer->Init(L"_identifier compound_identifier_test id_", &error) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"_identifier");

		pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"compound_identifier_test");

		pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"id_");
	}

	SECTION("Invalid \'digit at the begining of the identifier\' identifier test")
	{
		REQUIRE(pLexer->Init(L"42identifier", &error) == gplc::RV_FAIL);
	}

	SECTION("GetNextToken test")
	{
		REQUIRE(pLexer->Init(L"id0 id1 id2 id3 id4 id5", &error) == gplc::RV_SUCCESS);

		gplc::U32 numOfTokens = 0;

		while (pLexer->GetNextToken())
		{
			numOfTokens++;
		}

		REQUIRE(numOfTokens == 5); //5 is because of an access to the first token is provided with GetCurrToken; GetNextToken returns the other five
	}

	SECTION("PeekNextToken test")
	{
		REQUIRE(pLexer->Init(L"id0 id1 id2 id3 id4 id5", &error) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pCurrToken = nullptr;

		gplc::U32 i = 0;

		std::wstring currName;
		gplc::W16 charsBuf[4];

		while (pCurrToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->PeekNextToken(i)))
		{
			swprintf(charsBuf, L"id%d\0", i);

			currName.clear();
			currName.append(charsBuf);

			i++;

			REQUIRE(pCurrToken->GetName() == currName);
			REQUIRE(pCurrToken->GetType() == gplc::TT_IDENTIFIER);
		}

		REQUIRE(pLexer->PeekNextToken(6) == nullptr);
		REQUIRE(pLexer->PeekNextToken(42) == nullptr);
	}

	delete pLexer;
}
#include <catch.hpp>
#include <gplc.h>


TEST_CASE("Lexer's tests")
{
	gplc::TLexerErrorInfo error;
	gplc::CLexer* pLexer = new gplc::CLexer();

	const std::wstring pathToConfig          = L".\\lexer\\configs\\.tokens";
	const std::wstring pathToCorrectConfig   = L".\\lexer\\configs\\correct.tokens";
	const std::wstring pathToIncorrectConfig = L".\\lexer\\configs\\incorrect.tokens";

	REQUIRE(pLexer != nullptr);
	
	SECTION("Correct identifier test")
	{
		REQUIRE(pLexer->Init(L"identifier", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"identifier");
	}

	SECTION("Correct identifier with digits test")
	{
		REQUIRE(pLexer->Init(L"identifier42 identi42fier", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"identifier42");

		pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == L"identi42fier");
	}

	SECTION("Correct identifier with underscope test")
	{
		REQUIRE(pLexer->Init(L"_identifier compound_identifier_test id_", pathToConfig, &error) == gplc::RV_SUCCESS);

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

	//Now it works in other way. A lexer recognizes number and identifier, but not the incorrect identifier. 
	//Therefore this test should be removed.

	//SECTION("Invalid \'digit at the begining of the identifier\' identifier test")
	//{
	//	REQUIRE(pLexer->Init(L"42identifier", pathToConfig, &error) == gplc::RV_FAIL);
	//}

	SECTION("GetNextToken test")
	{
		REQUIRE(pLexer->Init(L"id0 id1 id2 id3 id4 id5", pathToConfig, &error) == gplc::RV_SUCCESS);

		gplc::U32 numOfTokens = 0;

		while (pLexer->GetNextToken())
		{
			numOfTokens++;
		}

		REQUIRE(numOfTokens == 5); //5 is because of an access to the first token is provided with GetCurrToken; GetNextToken returns the other five
	}

	SECTION("PeekNextToken test")
	{
		REQUIRE(pLexer->Init(L"id0 id1 id2 id3 id4 id5", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pCurrToken = nullptr;

		gplc::U32 i = 0;

		std::wstring currName;
		gplc::W16 charsBuf[4];

		while (pCurrToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->PeekNextToken(i)))
		{
			swprintf_s(charsBuf, 4, L"id%d\0", i);

			currName.clear();
			currName.append(charsBuf);

			i++;

			REQUIRE(pCurrToken->GetName() == currName);
			REQUIRE(pCurrToken->GetType() == gplc::TT_IDENTIFIER);
		}

		REQUIRE(pLexer->PeekNextToken(6) == nullptr);
		REQUIRE(pLexer->PeekNextToken(42) == nullptr);
	}

	SECTION("_readTokensMapFromFile test")
	{
		REQUIRE(pLexer->Init(L"", pathToIncorrectConfig, &error) == gplc::RV_INCORRECT_CONFIG);
		REQUIRE(pLexer->Init(L"", pathToCorrectConfig, &error) == gplc::RV_SUCCESS);
	}

	SECTION("Keywords test")
	{
		REQUIRE(pLexer->Init(L"identifier integer int character char floatValue float2int float", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CToken* pCurrToken = pLexer->GetCurrToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_IDENTIFIER);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_IDENTIFIER);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_INT);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_IDENTIFIER);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_CHAR);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_IDENTIFIER);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_IDENTIFIER);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_FLOAT);

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken == nullptr);
	}

	SECTION("Numbers' tokens test")
	{
		REQUIRE(pLexer->Init(L"42 4.2 0.42 .5", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CNumberToken<gplc::I32>* pCurrIntToken = dynamic_cast<const gplc::CNumberToken<gplc::I32>*>(pLexer->GetCurrToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 42);
		CAPTURE(0);
		const gplc::CNumberToken<gplc::F64>* pCurrFloatToken = dynamic_cast<const gplc::CNumberToken<gplc::F64>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_DOUBLE);
		REQUIRE(pCurrFloatToken->GetValue() == Approx(4.2));

		CAPTURE(1);
		pCurrFloatToken = dynamic_cast<const gplc::CNumberToken<gplc::F64>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_DOUBLE);
		REQUIRE(pCurrFloatToken->GetValue() == Approx(0.42));

		CAPTURE(2);
		pCurrFloatToken = dynamic_cast<const gplc::CNumberToken<gplc::F64>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_DOUBLE);
		REQUIRE(pCurrFloatToken->GetValue() == Approx(0.5));
		CAPTURE(3);
	}

	SECTION("numerical systems test")
	{
		REQUIRE(pLexer->Init(L"0x42 0b10", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CNumberToken<gplc::I32>* pCurrIntToken = dynamic_cast<const gplc::CNumberToken<gplc::I32>*>(pLexer->GetCurrToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 0x42);

		pCurrIntToken = dynamic_cast<const gplc::CNumberToken<gplc::I32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 2);
	}

	delete pLexer;
}
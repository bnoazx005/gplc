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
		REQUIRE(pLexer->Init(L"identifier integer int character char floatValue float2int float!====<>+-*[]{};,.:\\/", pathToConfig, &error) == gplc::RV_SUCCESS);
		const gplc::CToken* pCurrToken = pLexer->GetCurrToken();

		std::vector<gplc::E_TOKEN_TYPE> tokens;
		tokens.push_back(gplc::TT_IDENTIFIER);
		tokens.push_back(gplc::TT_IDENTIFIER);
		tokens.push_back(gplc::TT_INT);
		tokens.push_back(gplc::TT_IDENTIFIER);
		tokens.push_back(gplc::TT_CHAR);
		tokens.push_back(gplc::TT_IDENTIFIER);
		tokens.push_back(gplc::TT_IDENTIFIER);
		tokens.push_back(gplc::TT_FLOAT);
		tokens.push_back(gplc::TT_NE);
		tokens.push_back(gplc::TT_EQ);
		tokens.push_back(gplc::TT_ASSIGN_OP);
		tokens.push_back(gplc::TT_LT);
		tokens.push_back(gplc::TT_GT);
		tokens.push_back(gplc::TT_PLUS);
		tokens.push_back(gplc::TT_MINUS);
		tokens.push_back(gplc::TT_STAR);
		tokens.push_back(gplc::TT_OPEN_SQR_BRACE);
		tokens.push_back(gplc::TT_CLOSE_SQR_BRACE);
		tokens.push_back(gplc::TT_OPEN_BRACE);
		tokens.push_back(gplc::TT_CLOSE_BRACE);
		tokens.push_back(gplc::TT_SEMICOLON);
		tokens.push_back(gplc::TT_COMMA);
		tokens.push_back(gplc::TT_POINT);
		tokens.push_back(gplc::TT_COLON);
		tokens.push_back(gplc::TT_BACKSLASH);
		tokens.push_back(gplc::TT_SLASH);

		gplc::U32 tokensCount = tokens.size();

		for (gplc::U32 i = 0; i < tokensCount; i++)
		{
			pCurrToken = pLexer->GetCurrToken();

			REQUIRE(pCurrToken != nullptr);
			REQUIRE(pCurrToken->GetType() == tokens[i]);

			pCurrToken = pLexer->GetNextToken();
		}

		pCurrToken = pLexer->GetNextToken();

		REQUIRE(pCurrToken == nullptr);
	}

	SECTION("Numbers' tokens test")
	{
		REQUIRE(pLexer->Init(L"42 4.2 0.42 .5", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CTypedValueToken<gplc::I32>* pCurrIntToken = dynamic_cast<const gplc::CTypedValueToken<gplc::I32>*>(pLexer->GetCurrToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 42);

		const gplc::CTypedValueToken<gplc::F64>* pCurrFloatToken = dynamic_cast<const gplc::CTypedValueToken<gplc::F64>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_DOUBLE);
		REQUIRE(pCurrFloatToken->GetValue() == Approx(4.2));

		pCurrFloatToken = dynamic_cast<const gplc::CTypedValueToken<gplc::F64>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_DOUBLE);
		REQUIRE(pCurrFloatToken->GetValue() == Approx(0.42));

		pCurrFloatToken = dynamic_cast<const gplc::CTypedValueToken<gplc::F64>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_DOUBLE);
		REQUIRE(pCurrFloatToken->GetValue() == Approx(0.5));
	}

	SECTION("numerical systems test")
	{
		REQUIRE(pLexer->Init(L"0x42 0b10 042 42", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CTypedValueToken<gplc::I32>* pCurrIntToken = dynamic_cast<const gplc::CTypedValueToken<gplc::I32>*>(pLexer->GetCurrToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 0x42);

		pCurrIntToken = dynamic_cast<const gplc::CTypedValueToken<gplc::I32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 2);

		pCurrIntToken = dynamic_cast<const gplc::CTypedValueToken<gplc::I32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 042);

		pCurrIntToken = dynamic_cast<const gplc::CTypedValueToken<gplc::I32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken->GetValue() == 42);
	}

	SECTION("number's literals test")
	{
		REQUIRE(pLexer->Init(L"42l 42ul 42s 42uu 42f 42usf", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CTypedValueToken<gplc::IL32>* pCurrIntToken1 = dynamic_cast<const gplc::CTypedValueToken<gplc::IL32>*>(pLexer->GetCurrToken());
		
		REQUIRE(pCurrIntToken1 != nullptr);
		REQUIRE(pCurrIntToken1->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken1->GetValue() == 42);

		const gplc::CTypedValueToken<gplc::UL32>* pCurrIntToken2 = dynamic_cast<const gplc::CTypedValueToken<gplc::UL32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken2 != nullptr);
		REQUIRE(pCurrIntToken2->GetType() == gplc::TT_UINT);
		REQUIRE(pCurrIntToken2->GetValue() == 42);

		const gplc::CTypedValueToken<gplc::I32>* pCurrIntToken3 = dynamic_cast<const gplc::CTypedValueToken<gplc::I32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken3 != nullptr);
		REQUIRE(pCurrIntToken3->GetType() == gplc::TT_INT);
		REQUIRE(pCurrIntToken3->GetValue() == 42);

		const gplc::CTypedValueToken<gplc::U32>* pCurrIntToken4 = dynamic_cast<const gplc::CTypedValueToken<gplc::U32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken4 != nullptr);
		REQUIRE(pCurrIntToken4->GetType() == gplc::TT_UINT);
		REQUIRE(pCurrIntToken4->GetValue() == 42);

		const gplc::CTypedValueToken<gplc::F32>* pCurrFloatToken5 = dynamic_cast<const gplc::CTypedValueToken<gplc::F32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken5 != nullptr);
		REQUIRE(pCurrFloatToken5->GetType() == gplc::TT_FLOAT);
		REQUIRE(pCurrFloatToken5->GetValue() == Approx(42.0));

		const gplc::CTypedValueToken<gplc::F32>* pCurrFloatToken6 = dynamic_cast<const gplc::CTypedValueToken<gplc::F32>*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken6 != nullptr);
		REQUIRE(pCurrFloatToken6->GetType() == gplc::TT_FLOAT);
		REQUIRE(pCurrFloatToken6->GetValue() == Approx(42.0));
	}

	SECTION("single line comments test")
	{
		REQUIRE(pLexer->Init(L"//42l 42ul 42s 42uu 42f 42usf", pathToConfig, &error) == gplc::RV_SUCCESS);
		
		REQUIRE(pLexer->GetCurrToken() == nullptr);
	}

	SECTION("multi line comments test")
	{
		REQUIRE(pLexer->Init(L"/*42l 42ul 42s 42uu 42f 42usf*/\n/**/", pathToConfig, &error) == gplc::RV_SUCCESS);

		REQUIRE(pLexer->GetCurrToken() == nullptr);

		REQUIRE(pLexer->Init(L"/*42l 42ul*/\n    .4f .4", pathToConfig, &error) == gplc::RV_SUCCESS);

		const gplc::CTypedValueToken<gplc::F32>* pCurrFloatToken = dynamic_cast<const gplc::CTypedValueToken<gplc::F32>*>(pLexer->GetCurrToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_FLOAT);
		REQUIRE(pCurrFloatToken->GetValue() == Approx(0.4f));

		const gplc::CTypedValueToken<gplc::F64>* pCurrDoubleToken = dynamic_cast<const gplc::CTypedValueToken<gplc::F64>*>(pLexer->GetNextToken());

		REQUIRE(pCurrDoubleToken != nullptr);
		REQUIRE(pCurrDoubleToken->GetType() == gplc::TT_DOUBLE);
		REQUIRE(pCurrDoubleToken->GetValue() == Approx(0.4));
	}

	SECTION("strings test")
	{
		REQUIRE(pLexer->Init(LR"("Hello, world!")", pathToConfig, &error) == gplc::RV_SUCCESS);
	}
	
	delete pLexer;
}
#include <catch2/catch.hpp>
#include <gplc.h>


TEST_CASE("Lexer's tests")
{
	gplc::CLexer* pLexer = new gplc::CLexer();

	const std::string pathToConfig          = ".\\..\\..\\lexer\\configs\\.tokens";
	const std::string pathToCorrectConfig   = ".\\..\\..\\lexer\\configs\\correct.tokens";
	const std::string pathToIncorrectConfig = ".\\..\\..\\lexer\\configs\\incorrect.tokens";

	REQUIRE(pLexer != nullptr);
	
	SECTION("Correct identifier test")
	{
		REQUIRE(pLexer->Init("identifier", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == "identifier");
	}

	SECTION("Correct identifier with digits test")
	{
		REQUIRE(pLexer->Init("identifier42 identi42fier", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == "identifier42");

		pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == "identi42fier");
	}

	SECTION("Correct identifier with underscope test")
	{
		REQUIRE(pLexer->Init("_identifier compound_identifier_test id_", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetCurrToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == "_identifier");

		pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == "compound_identifier_test");

		pIdentifierToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken());

		REQUIRE(pIdentifierToken->GetType() == gplc::TT_IDENTIFIER);
		REQUIRE(pIdentifierToken->GetName() == "id_");
	}

	//Now it works in other way. A lexer recognizes number and identifier, but not the incorrect identifier. 
	//Therefore this test should be removed.

	//SECTION("Invalid \'digit at the begining of the identifier\' identifier test")
	//{
	//	REQUIRE(pLexer->Init("42identifier", pathToConfig, &error) == gplc::RV_FAIL);
	//}

	SECTION("GetNextToken test")
	{
		REQUIRE(pLexer->Init("id0 id1 id2 id3 id4 id5", pathToConfig) == gplc::RV_SUCCESS);

		gplc::U32 numOfTokens = 0;

		while (pLexer->GetNextToken())
		{
			numOfTokens++;
		}

		REQUIRE(numOfTokens == 5); //5 is because of an access to the first token is provided with GetCurrToken; GetNextToken returns the other five
	}

	SECTION("PeekNextToken test")
	{
		REQUIRE(pLexer->Init("id0 id1 id2 id3 id4 id5", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CIdentifierToken* pCurrToken = nullptr;

		gplc::U32 i = 0;

		std::string currName;
		gplc::C8 charsBuf[4];

		while (pCurrToken = dynamic_cast<const gplc::CIdentifierToken*>(pLexer->PeekNextToken(i)))
		{
			sprintf_s(charsBuf, 4, "id%d\0", i);

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
		REQUIRE(pLexer->Init("", pathToIncorrectConfig) == gplc::RV_INCORRECT_CONFIG);
		REQUIRE(pLexer->Init("", pathToCorrectConfig) == gplc::RV_SUCCESS);
	}

	SECTION("Keywords test")
	{
		REQUIRE(pLexer->Init("identifier integer int character char floatValue float2int float!====<>+-*[]{};,.:\\/", pathToConfig) == gplc::RV_SUCCESS);
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
		REQUIRE(pLexer->Init("42 4.2 0.42 .5", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CLiteralToken* pCurrIntToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CIntLiteral*>(pCurrIntToken->GetValue())->GetValue() == 42);

		const gplc::CLiteralToken* pCurrFloatToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CDoubleLiteral*>(pCurrFloatToken->GetValue())->GetValue() == Approx(4.2));

		pCurrFloatToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CDoubleLiteral*>(pCurrFloatToken->GetValue())->GetValue() == Approx(0.42));

		pCurrFloatToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CDoubleLiteral*>(pCurrFloatToken->GetValue())->GetValue() == Approx(0.5));
	}

	SECTION("numerical systems test")
	{
		REQUIRE(pLexer->Init("0x42 0b10 042 42", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CLiteralToken* pCurrIntToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CIntLiteral*>(pCurrIntToken->GetValue())->GetValue() == 0x42);

		pCurrIntToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CIntLiteral*>(pCurrIntToken->GetValue())->GetValue() == 2);

		pCurrIntToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CIntLiteral*>(pCurrIntToken->GetValue())->GetValue() == 042);

		pCurrIntToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken != nullptr);
		REQUIRE(pCurrIntToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CIntLiteral*>(pCurrIntToken->GetValue())->GetValue() == 42);
	}

	SECTION("number's literals test")
	{
		REQUIRE(pLexer->Init("42l 42ul 42s 42uu 42f 42usf", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CLiteralToken* pCurrIntToken1 = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken());
		
		REQUIRE(pCurrIntToken1 != nullptr);
		REQUIRE(pCurrIntToken1->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CIntLiteral*>(pCurrIntToken1->GetValue())->GetValue() == 42);

		const gplc::CLiteralToken* pCurrIntToken2 = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken2 != nullptr);
		REQUIRE(pCurrIntToken2->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CUIntLiteral*>(pCurrIntToken2->GetValue())->GetValue() == 42);

		const gplc::CLiteralToken* pCurrIntToken3 = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken3 != nullptr);
		REQUIRE(pCurrIntToken3->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CIntLiteral*>(pCurrIntToken3->GetValue())->GetValue() == 42);

		const gplc::CLiteralToken* pCurrIntToken4 = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrIntToken4 != nullptr);
		REQUIRE(pCurrIntToken4->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CUIntLiteral*>(pCurrIntToken4->GetValue())->GetValue() == 42);

		const gplc::CLiteralToken* pCurrFloatToken5 = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken5 != nullptr);
		REQUIRE(pCurrFloatToken5->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CFloatLiteral*>(pCurrFloatToken5->GetValue())->GetValue() == Approx(42.0));

		const gplc::CLiteralToken* pCurrFloatToken6 = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrFloatToken6 != nullptr);
		REQUIRE(pCurrFloatToken6->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CFloatLiteral*>(pCurrFloatToken6->GetValue())->GetValue() == Approx(42.0));
	}

	SECTION("single line comments test")
	{
		REQUIRE(pLexer->Init("//42l 42ul 42s 42uu 42f 42usf", pathToConfig) == gplc::RV_SUCCESS);
		
		REQUIRE(pLexer->GetCurrToken() == nullptr);
	}

	SECTION("multi line comments test")
	{
		REQUIRE(pLexer->Init("/*42l 42ul 42s 42uu 42f 42usf*/\n/**/", pathToConfig) == gplc::RV_SUCCESS);

		REQUIRE(pLexer->GetCurrToken() == nullptr);

		REQUIRE(pLexer->Init("/*42l 42ul*/\n    .4f .4", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CLiteralToken* pCurrFloatToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken());

		REQUIRE(pCurrFloatToken != nullptr);
		REQUIRE(pCurrFloatToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CFloatLiteral*>(pCurrFloatToken->GetValue())->GetValue() == Approx(0.4f));

		const gplc::CLiteralToken* pCurrDoubleToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrDoubleToken != nullptr);
		REQUIRE(pCurrDoubleToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CDoubleLiteral*>(pCurrDoubleToken->GetValue())->GetValue() == Approx(0.4));
	}

	SECTION("strings test")
	{
		REQUIRE(pLexer->Init(R"("Hello, world!")", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CLiteralToken* pCurrToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken());

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CStringLiteral*>(pCurrToken->GetValue())->GetValue() == "Hello, world!");
	}

	SECTION("chars test")
	{
		REQUIRE(pLexer->Init("'f'  'g'", pathToConfig) == gplc::RV_SUCCESS);

		const gplc::CLiteralToken* pCurrToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken());

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CCharLiteral*>(pCurrToken->GetValue())->GetValue() == "f");

		pCurrToken = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCurrToken != nullptr);
		REQUIRE(pCurrToken->GetType() == gplc::TT_LITERAL);
		REQUIRE(dynamic_cast<const gplc::CCharLiteral*>(pCurrToken->GetValue())->GetValue() == "g");
	}
	
	SECTION("Save- Restore- methods test")
	{
		REQUIRE(pLexer->Init("42l 42ul 42s 42uu 42f 42usf\n/**/", pathToConfig) == gplc::RV_SUCCESS);

		REQUIRE(pLexer->GetNextToken() != nullptr);

		pLexer->SavePosition();

		const gplc::CToken* pSavedToken = pLexer->GetCurrToken();

		for (gplc::I32 i = 0; i < 3; i++)
		{
			REQUIRE(pLexer->GetNextToken() != nullptr);
		}

		pLexer->RestorePosition();

		REQUIRE(pLexer->GetCurrToken() == pSavedToken);
	}

	SECTION("true false")
	{
		REQUIRE(pLexer->Init("true false", pathToConfig) == gplc::RV_SUCCESS);

		REQUIRE(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken()));
		REQUIRE(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()));
	}

	SECTION("null")
	{
		REQUIRE(pLexer->Init("null", pathToConfig) == gplc::RV_SUCCESS);

		REQUIRE(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken()));
	}

	SECTION("TestInit_Pass0_ReturnsIntLiteral")
	{
		REQUIRE(pLexer->Init("0", pathToConfig) == gplc::RV_SUCCESS);

		REQUIRE(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetCurrToken()));
	}


	delete pLexer;
}
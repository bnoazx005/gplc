#include <catch2/catch.hpp>
#include "stubInputStream.h"
#include <gplc.h>


using namespace gplc;


TEST_CASE("Lexer's tests")
{
	gplc::CLexer* pLexer = new gplc::CLexer();
		
	auto checkIdentifierToken = [](const CIdentifierToken* pIdentifierToken, const std::string& expectedIdentifier)
	{
		REQUIRE(pIdentifierToken);
		REQUIRE(pIdentifierToken->GetName() == expectedIdentifier);
	};

	SECTION("TestGetNextToken_ParseCorrectIdentifier_ReturnsIdentifierToken")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"identifier"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		checkIdentifierToken(dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken()), "identifier");
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}

	SECTION("TestGetNextToken_PassIdentifierWithDigits_ReturnsIdentifierToken")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"identifier42 identi42fier"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		checkIdentifierToken(dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken()), "identifier42");
		checkIdentifierToken(dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken()), "identi42fier");
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}

	SECTION("Correct identifier with underscope test")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"_identifier compound_identifier_test id_"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		checkIdentifierToken(dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken()), "_identifier");
		checkIdentifierToken(dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken()), "compound_identifier_test");
		checkIdentifierToken(dynamic_cast<const gplc::CIdentifierToken*>(pLexer->GetNextToken()), "id_");
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}

	SECTION("TestPeekNextToken_PassIdentifiers_ReturnsCorrectTokenByItsOffset")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"id0 id1 id2 id3 id4 id5"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		for (U8 i = 1; i < 7; ++i)
		{
			checkIdentifierToken(dynamic_cast<const gplc::CIdentifierToken*>(pLexer->PeekNextToken(i)), std::string("id").append(std::to_string(i - 1)));
		}

		REQUIRE(!pLexer->PeekNextToken(7));

		delete pInputStream;
	}

	SECTION("TestGetNextToken_PassStringWithKeywords_ReturnsCorrectTokensSequence")
	{
		std::vector<E_TOKEN_TYPE> tokens
		{
			TT_IDENTIFIER,
			TT_IDENTIFIER,
			TT_IDENTIFIER,
			TT_IDENTIFIER,
			TT_IDENTIFIER,
			TT_IDENTIFIER,
			TT_NE,
			TT_EQ,
			TT_ASSIGN_OP,
			TT_LT,
			TT_GT,
			TT_PLUS,
			TT_MINUS,
			TT_STAR,
			TT_OPEN_SQR_BRACE,
			TT_CLOSE_SQR_BRACE,
			TT_OPEN_BRACE,
			TT_CLOSE_BRACE,
			TT_SEMICOLON,
			TT_COMMA,
			TT_POINT,
			TT_COLON,
			TT_BACKSLASH,
			TT_SLASH,
			TT_MINUS,
			TT_ARROW,
			TT_MINUS,
			TT_LITERAL,
			TT_COMMA
		};
		
		IInputStream* pInputStream = new CStubInputStream(
			{
				"identifier integer  character  floatValue float2int integer!====<>+-*[]{};,.:\\/-->-2,"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);
		
		const CToken* pCurrToken = nullptr;

		for (U32 i = 0; i < tokens.size(); ++i)
		{
			pCurrToken = pLexer->GetNextToken();

			REQUIRE(pCurrToken);
			REQUIRE(pCurrToken->GetType() == tokens[i]);
		}

		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;		
	}

	SECTION("TestGetNextToken_PassIntegralValuesInDifferentRadixes_ReturnsCorrectLiterals")
	{
		auto checkLiteral = [](const CLiteralToken* pToken, int expectedValue)
		{
			REQUIRE(pToken);
			REQUIRE(dynamic_cast<const gplc::CIntValue*>(pToken->GetValue())->GetValue() == expectedValue);
		};

		IInputStream* pInputStream = new CStubInputStream(
			{
				"0x42 0b10 042 42 0 42L"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		checkLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 0x42);
		checkLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 0b10);
		checkLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 042);
		checkLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 42);
		checkLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 0);
		checkLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 42);
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}


	SECTION("TestGetNextToken_PassFloatingPointValues_ReturnsCorrectLiterals")
	{
		auto checkFloatLiteral = [](const CLiteralToken* pToken, F32 expectedValue)
		{
			REQUIRE(pToken);
			REQUIRE(dynamic_cast<const gplc::CFloatValue*>(pToken->GetValue())->GetValue() == expectedValue);
		};

		auto checkDoubleLiteral = [](const CLiteralToken* pToken, F64 expectedValue)
		{
			REQUIRE(pToken);
			REQUIRE(dynamic_cast<const gplc::CDoubleValue*>(pToken->GetValue())->GetValue() == expectedValue);
		};

		IInputStream* pInputStream = new CStubInputStream(
			{
				"4.2 0.42 .5 0.f .0f 42f"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		checkDoubleLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 4.2);
		checkDoubleLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 0.42);
		checkDoubleLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 0.5);
		checkFloatLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 0.0f);
		checkFloatLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 0.0f);
		checkFloatLiteral(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()), 42.0f);
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}

	SECTION("TestGetNextToken_PassSingleLineComment_ReturnsNothing")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"//42l 42ul 42s 42uu 42f 42usf"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}

	SECTION("TestGetNextToken_PassMultilineCommentsWithNesting_ReturnsNothing")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"/*42l 42ul 42s /*42uu*//**/ 42f 42usf*/\n/**/"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}

	SECTION("TestGetNextToken_PassStringLiteral_ReturnsLiteralToken")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"\"Hello, world!\""
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		auto pStringLiteral = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pStringLiteral);
		REQUIRE((dynamic_cast<const CStringValue*>(pStringLiteral->GetValue()))->GetValue() == "Hello, world!");
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}

	SECTION("TestGetNextToken_PassCharLiterals_ReturnsLiteralToken")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"'f'  'g'"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		auto pCharLiteral = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCharLiteral);
		REQUIRE((dynamic_cast<const CCharValue*>(pCharLiteral->GetValue()))->GetValue() == "f");
		
		pCharLiteral = dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken());

		REQUIRE(pCharLiteral);
		REQUIRE((dynamic_cast<const CCharValue*>(pCharLiteral->GetValue()))->GetValue() == "g");

		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}	

	SECTION("TestGetNextToken_PassSpecialLiterals_ReturnsCorrectTokens")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"true false null"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		REQUIRE(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()));
		REQUIRE(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()));
		REQUIRE(dynamic_cast<const gplc::CLiteralToken*>(pLexer->GetNextToken()));
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}


	SECTION("TestGetNextToken_PassContinueBreak_ReturnsCorrectTokens")
	{
		IInputStream* pInputStream = new CStubInputStream(
			{
				"continue break"
			});

		REQUIRE(pLexer->Init(pInputStream) == gplc::RV_SUCCESS);

		REQUIRE(pLexer->GetNextToken()->GetType() == TT_CONTINUE_KEYWORD);
		REQUIRE(pLexer->GetNextToken()->GetType() == TT_BREAK_KEYWORD);
		REQUIRE(!pLexer->GetNextToken());

		delete pInputStream;
	}
	
	delete pLexer;
}
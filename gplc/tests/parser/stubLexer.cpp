#include "stubLexer.h"
#include "lexer\gplcTokens.h"


CStubLexer::CStubLexer():
	gplc::ILexer(), mCurrTokenIndex(0)
{
}

CStubLexer::CStubLexer(const CStubLexer& lexer):
	gplc::ILexer(lexer), mCurrTokenIndex(0)
{
}

CStubLexer::~CStubLexer()
{
}

gplc::Result CStubLexer::Init(const std::wstring& inputStream, const std::wstring& configFilename, gplc::TLexerErrorInfo* errorInfo)
{
	errorInfo = nullptr;

	//fill in the tokens' vector
	mTokens.push_back(new gplc::CToken(gplc::TT_IDENTIFIER, 0));
	mTokens.push_back(new gplc::CToken(gplc::TT_COLON, 1));
	mTokens.push_back(new gplc::CToken(gplc::TT_INT32_TYPE, 2));
	
	return gplc::RV_SUCCESS;
}

gplc::Result CStubLexer::Reset()
{
	return gplc::RV_SUCCESS;
}

const gplc::CToken* CStubLexer::GetCurrToken() const
{
	if (mTokens.size() <= mCurrTokenIndex)
	{
		return nullptr;
	}

	return mTokens[mCurrTokenIndex];
}

const gplc::CToken* CStubLexer::GetNextToken()
{
	if (mCurrTokenIndex + 1 >= mTokens.size())
	{
		return nullptr;
	}

	return mTokens[++mCurrTokenIndex];
}

const gplc::CToken* CStubLexer::PeekNextToken(gplc::U32 numOfSteps) const
{
	gplc::U32 neededTokenId = numOfSteps + mCurrTokenIndex;

	if (mTokens.size() <= neededTokenId)
	{
		return nullptr;
	}

	return mTokens[neededTokenId];
}
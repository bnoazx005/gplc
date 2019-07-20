#include "stubLexer.h"
#include "lexer\gplcTokens.h"


CStubLexer::CStubLexer(const std::vector<gplc::CToken*>& tokens):
	gplc::ILexer(), mCurrTokenIndex(0), mSavedTokenIndex(UINT32_MAX)
{
	std::copy(tokens.begin(), tokens.end(), std::back_inserter(mTokens));
}

CStubLexer::~CStubLexer()
{
	gplc::U32 tokensCount = mTokens.size();

	gplc::CToken* pCurrToken = nullptr;

	for (gplc::U32 i = 0; i < tokensCount; i++) //release the memory
	{
		pCurrToken = mTokens[i];

		if (pCurrToken == nullptr)
		{
			continue;
		}

		delete pCurrToken;
	}
}

gplc::Result CStubLexer::Init(gplc::IInputStream* pInputStream)
{	
	return gplc::RV_SUCCESS;
}

gplc::Result CStubLexer::Reset()
{
	gplc::U32 tokensCount = mTokens.size();

	gplc::CToken* pCurrToken = nullptr;

	for (gplc::U32 i = 0; i < tokensCount; i++) //release the memory
	{
		pCurrToken = mTokens[i];

		if (pCurrToken == nullptr)
		{
			continue;
		}

		delete pCurrToken;
	}

	return gplc::RV_SUCCESS;
}

const gplc::CToken* CStubLexer::GetCurrToken()
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

const gplc::CToken* CStubLexer::PeekNextToken(gplc::U32 numOfSteps)
{
	gplc::U32 neededTokenId = numOfSteps + mCurrTokenIndex;

	if (mTokens.size() <= neededTokenId)
	{
		return nullptr;
	}

	return mTokens[neededTokenId];
}
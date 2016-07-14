/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class defenition

	\todo
*/

#include "lexer\gplcLexer.h"
#include "lexer\gplcTokens.h"


namespace gplc
{
	CLexer::CLexer():
		mCurrPos(0), mCurrTokenIndex(0)
	{

	}

	CLexer::CLexer(const CLexer& lexer) :
		mCurrPos(0), mCurrTokenIndex(0)
	{
	}

	CLexer::~CLexer()
	{

	}

	Result CLexer::Init(const std::wstring& inputStream)
	{
		Result result = Reset();

		if (!SUCCESS(result))
		{
			return result;
		}

		U32 streamLength = inputStream.length();
		U32 pos          = mCurrPos;
		
		TLexerErrorInfo errorInfo;

		CToken* pCurrToken = nullptr;

		while ((pos < streamLength) && (inputStream[pos] != WEOF))
		{
			if (inputStream[pos] == ' ') //skip whitespace
			{
				pos++;

				continue;
			}

			// increment is done implicitly in _scanToken
			pCurrToken = _scanToken(inputStream, pos, &errorInfo);

			if (pCurrToken == nullptr)
			{
				return RV_FAIL;
			}

			mTokens.push_back(pCurrToken);
		}

		return RV_SUCCESS;
	}
	
	Result CLexer::Reset()
	{
		mTokens.clear();

		mCurrTokenIndex = 0;
		mCurrPos        = 0;

		if (!mTokens.empty() || mCurrPos != 0)
		{
			return RV_FAIL;
		}

		return RV_SUCCESS;
	}

	const CToken* CLexer::GetCurrToken()
	{
		if (mTokens.size() <= mCurrTokenIndex)
		{
			return nullptr;
		}

		return mTokens[mCurrTokenIndex];
	}

	const CToken* CLexer::GetNextToken()
	{
		if (mTokens.size() <= mCurrTokenIndex + 1)
		{
			return nullptr;
		}

		return mTokens[++mCurrTokenIndex];
	}

	const CToken* CLexer::PeekNextToken(int numOfSteps) const
	{
		return nullptr;
	}
	
	CToken* CLexer::_scanToken(const std::wstring& stream, U32& pos, TLexerErrorInfo* errorInfo)
	{
		W16 currChar = stream[pos];

		errorInfo    = nullptr;

		if (iswalpha(currChar) || currChar == L'_') //try to read identifier's token
		{
			std::wstring identifierName;

			while (iswalpha(currChar) || iswdigit(currChar) || currChar == L'_')
			{
				identifierName.push_back(currChar);

				currChar = stream[++pos];
			}

			mCurrPos = pos;

			return new CIdentifierToken(identifierName);
		}

		return nullptr;
	}
}
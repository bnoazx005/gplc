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
		mCurrPos(0), mCurrLine(0), mCurrTokenIndex(0)
	{

	}

	CLexer::CLexer(const CLexer& lexer) :
		mCurrPos(0), mCurrLine(0), mCurrTokenIndex(0)
	{
	}

	CLexer::~CLexer()
	{

	}

	Result CLexer::Init(const std::wstring& inputStream, TLexerErrorInfo* errorInfo)
	{
		Result result = Reset();

		if (!SUCCESS(result))
		{
			return result;
		}

		U32 streamLength = inputStream.length();
		U32 pos          = mCurrPos;
		
		errorInfo = nullptr;

		CToken* pCurrToken = nullptr;

		while ((pos < streamLength) && (inputStream[pos] != WEOF))
		{
			if (iswblank(inputStream[pos])) //skip whitespace
			{
				pos++;

				continue;
			}

			if (iswspace(inputStream[pos])) //try detect \r and \n, 'cause all spaces and tabulations were passed at the previous case
			{
				pos++;
				mCurrLine++;

				continue;
			}

			// increment is done implicitly in _scanToken
			pCurrToken = _scanToken(inputStream, pos);

			if (pCurrToken == nullptr)
			{
				errorInfo = new TLexerErrorInfo();
				
				errorInfo->mPos  = mCurrPos;
				errorInfo->mLine = mCurrLine;

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

	const CToken* CLexer::PeekNextToken(U32 numOfSteps) const
	{
		U32 neededTokenId = numOfSteps + mCurrTokenIndex;

		if (mTokens.size() <= neededTokenId)
		{
			return nullptr;
		}

		return mTokens[neededTokenId];
	}
	
	CToken* CLexer::_scanToken(const std::wstring& stream, U32& pos)
	{
		W16 currChar = stream[pos];
		
		if (iswalpha(currChar) || currChar == L'_') //try to read identifier's token
		{
			std::wstring identifierName;

			while (iswalnum(currChar) || currChar == L'_')
			{
				identifierName.push_back(currChar);

				currChar = stream[++pos];
			}

			mCurrPos = pos;

			//try to detect reserved keywords here
			///<TODO: think about the way to do it 1)hardcode here; 2)loop over keywords from prepared file

			return new CIdentifierToken(identifierName);
		}

		return nullptr;
	}
}
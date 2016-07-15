/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class defenition

	\todo
*/

#include "lexer\gplcLexer.h"
#include "lexer\gplcTokens.h"
#include <fstream>


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

	Result CLexer::Init(const std::wstring& inputStream, const std::wstring& configFilename, TLexerErrorInfo* errorInfo)
	{
		Result result = Reset();
		
		errorInfo = nullptr;

		if (!SUCCESS(result))
		{
			return result;
		}

		//try parse config file with reserved tokens
		mReservedTokensMap = _readTokensMapFromFile(configFilename, result);

		if (!SUCCESS(result))
		{
			return result;
		}

		U32 streamLength = inputStream.length();
		U32 pos          = mCurrPos;
		
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
			std::map<std::wstring, E_TOKEN_TYPE>::const_iterator token;

			if ((token = mReservedTokensMap.find(identifierName)) != mReservedTokensMap.end())
			{
				return new CToken((*token).second); //return reserved keyword's token
			}

			return new CIdentifierToken(identifierName);	//if it's not reserved just return it as identifier
		}

		return nullptr;
	}
	
	std::map<std::wstring, E_TOKEN_TYPE> CLexer::_readTokensMapFromFile(const std::wstring& filename, Result& result)
	{
		std::map<std::wstring, E_TOKEN_TYPE> tokensMap;

		result = RV_SUCCESS;

		std::wifstream configFileWithTokens;
		
		configFileWithTokens.open(filename.c_str());

		if (!configFileWithTokens.is_open())
		{
			result = RV_FILE_NOT_FOUND;

			return tokensMap;
		}

		//parse config file
		std::wstring currConfigLine;
		std::wstring tokenName;
		std::wstring value;

		U32 pos     = 0;
		U32 prevPos = 0;

		while (std::getline(configFileWithTokens, currConfigLine))
		{
			//the parsing of a current config line
			//its representation (token_name  token_value_in_enum_E_TOKEN_TYPE)

			pos = currConfigLine.find_first_of(L'(');

			if (pos == -1) // there is no open bracket
			{
				result = RV_INCORRECT_CONFIG;

				break;
			}

			//skip white spaces
			pos     = currConfigLine.find_first_not_of(L' ', pos + 1);
			prevPos = pos;

			//try to get a name of token
			pos = currConfigLine.find_first_of(L' ', pos);

			if (pos == -1) // delimiter between name and value is absent
			{
				result = RV_INCORRECT_CONFIG;

				break;
			}

			tokenName = currConfigLine.substr(prevPos, pos - prevPos);
			
			//skip white spaces
			pos     = currConfigLine.find_first_not_of(L' ', pos);
			prevPos = pos;
			
			//check close bracket
			pos = currConfigLine.find_first_of(L')', pos);

			if (pos == -1) // there is no close bracket
			{
				result = RV_INCORRECT_CONFIG;

				break;
			}

			value = currConfigLine.substr(prevPos, pos - prevPos);
			
			//erase all white spaces if they exist
			while ((pos = value.find(L' ')) != -1)
			{
				value.erase(pos, 1);
			}

			tokensMap.insert(std::make_pair(tokenName, (E_TOKEN_TYPE)_wtoi(value.c_str())));
		}

		configFileWithTokens.close();

		return tokensMap;
	}
}
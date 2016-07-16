/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class defenition

	\todo
	1) A supporting of number's literals
	2) Add other reserved tokens recognition. For instance, ==, !=, :, etc.
	3) Refactor a generation of numbers' tokens
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

		W16 currChar = _getCurrChar(inputStream);
		
		CToken* pCurrToken = nullptr;

		while ((currChar = _getCurrChar(inputStream)) != WEOF)
		{
			if (iswblank(currChar)) //skip whitespace
			{
				currChar = _getNextChar(inputStream);

				continue;
			}

			if (iswspace(currChar)) //try detect \r and \n, 'cause all spaces and tabulations were passed at the previous case
			{
				mCurrLine++;

				currChar = _getNextChar(inputStream);

				continue;
			}

			// increment is done implicitly in _scanToken
			pCurrToken = _scanToken(inputStream);

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
		if (mCurrTokenIndex + 1 >= mTokens.size())
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

	W16 CLexer::_getCurrChar(const std::wstring& stream) const
	{
		if (mCurrPos >= stream.length())
		{
			return WEOF;
		}

		return stream[mCurrPos];
	}

	W16 CLexer::_getNextChar(const std::wstring& stream)
	{
		if (mCurrPos + 1 >= stream.length())
		{
			mCurrPos++;

			return WEOF;
		}
		
		return stream[++mCurrPos];
	}

	W16 CLexer::_peekNextChar(const std::wstring& stream, U32 offset) const
	{
		U32 newPositionAtStream = mCurrPos + offset;

		if (newPositionAtStream >= stream.length())
		{
			return WEOF;
		}

		return stream[newPositionAtStream];
	}
	
	CToken* CLexer::_scanToken(const std::wstring& stream)
	{
		W16 currChar = _getCurrChar(stream);
		
		if (iswalpha(currChar) || currChar == L'_') //try to read identifier's token
		{
			std::wstring identifierName;

			while (iswalnum(currChar) || currChar == L'_')
			{
				identifierName.push_back(currChar);

				currChar = _getNextChar(stream);
			}
			
			//try to detect reserved keywords here
			
			///<TODO: think about the way to do it 1)hardcode here; 2)loop over keywords from prepared file
			std::map<std::wstring, E_TOKEN_TYPE>::const_iterator token;

			if ((token = mReservedTokensMap.find(identifierName)) != mReservedTokensMap.end())
			{
				return new CToken((*token).second); //return reserved keyword's token
			}

			return new CIdentifierToken(identifierName);	//if it's not reserved just return it as identifier
		}

		if (iswdigit(currChar) || currChar == L'.') //try to get number
		{
			std::wstring numberStr;

			U8 numberType = 0; // flags: 0x0 - int; 0x1 - floating point; 0x80 - signed; 0x40 - long; 0x20 - long; 0x10 - hex; 0x8 - oct; 0x4 - bin;
			//floating point value always has sign bit and cannot has hex, oct, bin representations' bits.

			//try parse decimal integer or floating point value
			if (currChar != L'0' || (currChar == L'0' && _peekNextChar(stream, 1) == L'.'))
			{
				while (iswdigit(currChar)) // pass integers
				{                                           
					numberStr.push_back(currChar);

					currChar = _getNextChar(stream);
				}

				if (currChar == L'.') //it's floating point value
				{
					numberType = 0x1;

					while (iswdigit(currChar) || currChar == L'.') // pass the rest part of the number
					{
						numberStr.push_back(currChar);

						currChar = _getNextChar(stream);
					}
				}
			}
			else if (currChar == L'0') //hex, oct or bin representation of an integer value
			{
				numberStr.push_back(currChar);

				currChar = _getNextChar(stream);

				if (currChar == L'x' || currChar == L'X') //hex value
				{
					numberType |= 0x10; //set the flag of hexademical numeral system

					numberStr.push_back(currChar);

					currChar = _getNextChar(stream);

					const std::wstring hexAlphabet = L"abcdefABCDEF"; //not including 0-9 digits

					while (iswdigit(currChar) || (hexAlphabet.find_first_of(currChar) != -1))
					{
						numberStr.push_back(currChar);

						currChar = _getNextChar(stream);
					}
				}
				else if (currChar == L'b' || currChar == L'B') //bin value
				{
					numberType |= 0x4; //set the flag of binary numeral system
					
					currChar = _getNextChar(stream);

					while (currChar == L'0' || currChar == L'1')
					{
						numberStr.push_back(currChar);

						currChar = _getNextChar(stream);
					}
				}
				else if (iswdigit(currChar)) //oct value
				{
					numberType |= 0x8; //set the flag of octal numeral system
					
					numberStr.push_back(currChar);

					currChar = _getNextChar(stream);

					while (iswdigit(currChar))
					{
						numberStr.push_back(currChar);

						currChar = _getNextChar(stream);
					}
				}
				else //error
				{
					return nullptr;
				}
			}

			//while (isdigit(currChar) || currChar == L'.' || currChar == L'b' ||
			//	    currChar == L'x' || currChar == L'X') // pass integers, floating point values 
			//{                                            // and its hexademical, octal and binary representations
			//	numberStr.push_back(currChar);

			//	currChar = stream[++pos];
			//}
			//
			//check a literal after the number
			//const std::wstring allowableLiterals              = L"lLuUsS";
			//const std::wstring allowableFloatingPointLiterals = L"lLfF";
			//std::wstring       currLiterals;

			//while (allowableLiterals.find_first_of(currChar) != -1)
			//{
			//	switch (currChar)
			//	{
			//		case L'l': case L'L': // long integer
			//			break;

			//		case L'u': case L'U': // unsigned integer
			//			break;

			//		case L's': case L'S': // signed integer
			//			break;

			//		case L'f': case L'F': // single precision floating point value
			//			break;
			//	}
			//}
			
			I32 numSysBasis = (numberType & 0x10) ? 16 : (numberType & 0x8) ? 8 : (numberType & 0x4) ? 2 : 10;

			switch (numberType & 0x1)
			{
				case 0: // integer

					switch (numberType & 0x80) //Is it signed? If bit is turn on then it's signed value
					{
						case 0: //unsigned
							return new CNumberToken<U32>(TT_UINT, wcstoul(numberStr.c_str(), nullptr, numSysBasis));
							break;

						case 1: //signed
							return new CNumberToken<I32>(TT_INT, wcstol(numberStr.c_str(), nullptr, numSysBasis));
							break;
					}

					return new CNumberToken<I32>(TT_INT, wcstol(numberStr.c_str(), nullptr, numSysBasis));

					break;

				case 1: // floating point

					switch (numberType & 0x40) //1 - double; 0; - float
					{
						case 0:
							return new CNumberToken<F32>(TT_FLOAT, wcstof(numberStr.c_str(), nullptr));
							break;
						case 1:
							return new CNumberToken<F64>(TT_DOUBLE, _wtof(numberStr.c_str()));
							break;
					}

					break;

				default: // this case won't be never reached, but let it be here for safe code execution
					return nullptr;
			}
		}

		//try to recognize other reserved tokens
		///<TODO a recognition of other reserved tokens

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
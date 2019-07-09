/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class defenition

	\todo
	1) Add strings and chars tokens
	2) By now all strings are recognized as wide characters strings (unicode strings). Should thing about the way to change it.
	3) Add literals for strings
*/

#include "lexer\gplcLexer.h"
#include "lexer\gplcTokens.h"
#include "common\gplcConstants.h"
#include "common/gplcLiterals.h"
#include <fstream>


namespace gplc
{
	CLexer::CLexer():
		ILexer(), mCurrPos(0), mCurrLine(0), mCurrTokenIndex(0), mSavedTokenIndex(UINT32_MAX)
	{

	}

	CLexer::CLexer(const CLexer& lexer) :
		ILexer(lexer), mCurrPos(0), mCurrLine(0), mCurrTokenIndex(0), mSavedTokenIndex(UINT32_MAX)
	{
	}

	CLexer::~CLexer()
	{
		Reset();
	}

	Result CLexer::Init(const std::string& inputStream, const std::string& configFilename)
	{
		Result result = Reset();
		
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

		C8 currChar = _getCurrChar(inputStream);
		
		CToken* pCurrToken = nullptr;

		U8 numOfNestedCommentsBlocks = 0;

		while ((currChar = _getCurrChar(inputStream)) != EOF)
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

			// skip comments
			//try to recognize single- and milti- line comments
			if (currChar == '/')
			{
				currChar = _peekNextChar(inputStream, 1);

				if (currChar == '/') //single-line comment
				{
					//skip symbols until neither \n nor \r
					do
					{
						currChar = _getNextChar(inputStream);
					} while (currChar != '\n' && currChar != '\r' && currChar != EOF);

					continue;
				}
				else if (currChar == '*')
				{
					currChar = _getNextChar(inputStream); //get '*'
					numOfNestedCommentsBlocks = 1;

					do
					{
						currChar = _getNextChar(inputStream);

						if (currChar == '/' && _peekNextChar(inputStream, 1) == '*')
						{
							numOfNestedCommentsBlocks++;

							_getNextChar(inputStream); //get '*'

							continue;
						}

						if (currChar == '*' && _peekNextChar(inputStream, 1) == '/')
						{
							if (numOfNestedCommentsBlocks == 0)
							{
								TLexerErrorInfo errorInfo;

								memset(&errorInfo, 0, sizeof(errorInfo));

								errorInfo.mPos = mCurrPos;
								errorInfo.mLine = mCurrLine;

								OnErrorOutput.Invoke(errorInfo);

								return RV_INCORRECT_TOKEN;
							}

							numOfNestedCommentsBlocks--;

							_getNextChar(inputStream); //get '/'
						}
					} while (currChar != '*' && _peekNextChar(inputStream, 1) != '/' && numOfNestedCommentsBlocks > 0);

					_getNextChar(inputStream);

					continue;
				}
			}

			// increment is done implicitly in _scanToken
			pCurrToken = _scanToken(inputStream);

			if (pCurrToken == nullptr)
			{
				TLexerErrorInfo errorInfo;

				memset(&errorInfo, 0, sizeof(errorInfo));

				errorInfo.mPos = mCurrPos;
				errorInfo.mLine = mCurrLine;

				OnErrorOutput.Invoke(errorInfo);

				return RV_INCORRECT_TOKEN;
			}

			mTokens.push_back(pCurrToken);
		}

		return RV_SUCCESS;
	}
	
	Result CLexer::Reset()
	{
		U32 tokensCount = mTokens.size();

		CToken* pCurrToken = nullptr;

		for (U32 i = 0; i < tokensCount; i++) //release the memory
		{
			pCurrToken = mTokens[i];

			if (pCurrToken == nullptr)
			{
				continue;
			}

			delete pCurrToken;
		}

		mTokens.clear();

		mCurrPos         = 0;
		mCurrTokenIndex  = 0;
		mSavedTokenIndex = UINT32_MAX;

		if (!mTokens.empty() || mCurrPos != 0)
		{
			return RV_FAIL;
		}

		return RV_SUCCESS;
	}

	const CToken* CLexer::GetCurrToken() const
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

	void CLexer::SavePosition()
	{
		mSavedTokenIndex = mCurrTokenIndex;
	}

	void CLexer::RestorePosition()
	{
		if (mSavedTokenIndex >= UINT32_MAX)
		{
			return;
		}

		mCurrTokenIndex = mSavedTokenIndex;
	}

	C8 CLexer::_getCurrChar(const std::string& stream) const
	{
		if (mCurrPos >= stream.length())
		{
			return EOF;
		}

		return stream[mCurrPos];
	}

	C8 CLexer::_getNextChar(const std::string& stream)
	{
		if (mCurrPos + 1 >= stream.length())
		{
			mCurrPos++;

			return EOF;
		}
		
		return stream[++mCurrPos];
	}

	C8 CLexer::_peekNextChar(const std::string& stream, U32 offset) const
	{
		U32 newPositionAtStream = mCurrPos + offset;

		if (newPositionAtStream >= stream.length())
		{
			return EOF;
		}

		return stream[newPositionAtStream];
	}
	
	CToken* CLexer::_scanToken(const std::string& stream)
	{
		C8 currChar = _getCurrChar(stream);

		//try to recognize reserved symbols' sequences
		std::string expectedToken;

		expectedToken.push_back(currChar);
		
		std::map<std::string, E_TOKEN_TYPE>::const_iterator tokenIter, additionalTokenIter;

		if ((tokenIter = mReservedTokensMap.find(expectedToken)) != mReservedTokensMap.end())
		{
			currChar = _peekNextChar(stream, 1);
			
			expectedToken.push_back(currChar); // try to find another longer token

			if ((additionalTokenIter = mReservedTokensMap.find(expectedToken)) != mReservedTokensMap.end())
			{
				_getNextChar(stream);
				_getNextChar(stream);

				return new CToken((*additionalTokenIter).second, mCurrPos - 2);
			}

			if ((*tokenIter).second != TT_POINT || !iswdigit(currChar)) //check is it just a point or delimiter in a floating point number
			{
				_getNextChar(stream);

				return new CToken((*tokenIter).second, mCurrPos - 1);
			}
		}
		else
		{
			currChar = _peekNextChar(stream, 1);

			expectedToken.push_back(currChar); // try to find another longer token

			if ((additionalTokenIter = mReservedTokensMap.find(expectedToken)) != mReservedTokensMap.end())
			{
				_getNextChar(stream);
				_getNextChar(stream); //set pos to new char

				return new CToken((*additionalTokenIter).second, mCurrPos - 2);
			}
		}

		currChar = _getCurrChar(stream);
		
		//try to read identifier's token
		if (iswalpha(currChar) || currChar == '_') 
		{
			std::string identifierName;

			while (iswalnum(currChar) || currChar == '_')
			{
				identifierName.push_back(currChar);

				currChar = _getNextChar(stream);
			}
			
			//try to detect reserved keywords here
			
			///<TODO: think about the way to do it 1)hardcode here; 2)loop over keywords from prepared file
			std::map<std::string, E_TOKEN_TYPE>::const_iterator token;

			if ((token = mReservedTokensMap.find(identifierName)) != mReservedTokensMap.end())
			{
				return new CToken((*token).second, mCurrPos); //return reserved keyword's token
			}

			return new CIdentifierToken(identifierName, mCurrPos);	//if it's not reserved just return it as identifier
		}

		if (iswdigit(currChar) || currChar == '.') //try to get number
		{
			std::string numberStr;

			U8 numberType = NB_INT | NB_SIGNED; // flags: 0x0 - int; 0x1 - floating point; 0x80 - signed; 0x40 - long; 0x20 - long; 
			//0x10 - hex; 0x8 - oct; 0x4 - bin; floating point value always has sign bit and cannot has hex, oct, bin representations' bits.

			//try parse decimal integer or floating point value
			if (currChar != '0' || (currChar == '0' && _peekNextChar(stream, 1) == '.'))
			{
				while (iswdigit(currChar)) // pass integers
				{                                           
					numberStr.push_back(currChar);

					currChar = _getNextChar(stream);
				}

				if (currChar == '.' && iswdigit(_peekNextChar(stream, 1))) //it's floating point value
				{
					numberType = NB_FLOAT;

					while (iswdigit(currChar) || currChar == '.') // pass the rest part of the number
					{
						numberStr.push_back(currChar);

						currChar = _getNextChar(stream);
					}
				}
			}
			else if (currChar == '0') //hex, oct or bin representation of an integer value
			{
				numberStr.push_back(currChar);

				currChar = _getNextChar(stream);

				if (currChar == 'x' || currChar == 'X') //hex value
				{
					numberType |= NB_HEX; //set the flag of hexademical numeral system

					numberStr.push_back(currChar);

					currChar = _getNextChar(stream);

					const std::string hexAlphabet = "abcdefABCDEF"; //not including 0-9 digits

					while (iswdigit(currChar) || (hexAlphabet.find_first_of(currChar) != -1))
					{
						numberStr.push_back(currChar);

						currChar = _getNextChar(stream);
					}
				}
				else if (currChar == 'b' || currChar == 'B') //bin value
				{
					numberType |= NB_BIN; //set the flag of binary numeral system
					
					currChar = _getNextChar(stream);

					while (currChar == '0' || currChar == '1')
					{
						numberStr.push_back(currChar);

						currChar = _getNextChar(stream);
					}
				}
				else if (iswdigit(currChar)) //oct value
				{
					numberType |= NB_OCT; //set the flag of octal numeral system
					
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

			//check up literals
			const std::string allowableIntLiterals = "lLuUsS";

			U8 literalsCount = 0x0; 

			switch (numberType & NB_FLOAT)
			{
				case NB_INT:

					//all possible literals are placed in allowableLiterals
					while (allowableIntLiterals.find_first_of(currChar = _getCurrChar(stream)) != -1)
					{
						switch (currChar) //only first literal of 'u' or 's' influences on number's mask
						{
							case 'l':case 'L':

								if (!(numberType & NB_LONG))
								{
									numberType |= NB_LONG;
								}
								else
								{
									numberType |= NB_ADD_LONG;
								}

								break;
							case 'u':case 'U':
								numberType <<= 1; //clear 8th bit
								numberType >>= 1; 
								break;
							case 's':case 'S':
								numberType |= NB_SIGNED;
								break;
						}

						currChar = _getNextChar(stream);
					}

					if (currChar == 'f' || currChar == 'F')
					{
						numberType |= NB_FLOAT;

						currChar = _getNextChar(stream);
					}

					break;

				case NB_FLOAT:

					//try to get literals 
					//there is possible only one kind of literal for floating point values. It's 'f' or 'F'.
					currChar = _getCurrChar(stream);

					if (currChar == 'f' || currChar == 'F')
					{
						currChar = _getNextChar(stream); //read this value from the stream

						numberType &= ~NB_LONG; //clear 'long' bit
					}
					else if (allowableIntLiterals.find_first_of(currChar) != -1)
					{
						return nullptr; //incorrect literal for floating point was found
					}
					else //double
					{
						numberType |= NB_LONG;
					}

					break;
			}
			
			//the contruction of a token

			U32 numSysBasis = (numberType & NB_HEX) | (numberType & NB_OCT) | ((numberType & NB_BIN) >> 1);
			
			switch (numberType & NB_FLOAT)
			{
				case 0: // integer

					switch (numberType & NB_SIGNED) //Is it signed? If bit is turn on then it's signed value
					{
						case 0: //unsigned
							return new CLiteralToken(new CUIntLiteral(strtoul(numberStr.c_str(), nullptr, numSysBasis)), mCurrPos);

						case NB_SIGNED: //signed
							return new CLiteralToken(new CIntLiteral(strtol(numberStr.c_str(), nullptr, numSysBasis)), mCurrPos);
							
						default:
							return new CLiteralToken(new CIntLiteral(strtol(numberStr.c_str(), nullptr, numSysBasis)), mCurrPos);
					}

					break;

				case 1: // floating point

					switch (numberType & NB_LONG) //1 - double; 0; - float
					{
						case 0:
							return new CLiteralToken(new CFloatLiteral(atof(numberStr.c_str())), mCurrPos);

						case NB_LONG:
							return new CLiteralToken(new CDoubleLiteral(atof(numberStr.c_str())), mCurrPos);
					}

					break;

				default: // this case won't be never reached, but let it be here for safe code execution
					return nullptr;
			}
		}

		//try to get a string
		
		std::string strConstantValue;

		if (currChar == '\"')
		{
			while ((currChar = _getNextChar(stream)) != '\"' && currChar != EOF)
			{
				strConstantValue.push_back(currChar);
			}

			if (currChar == EOF)
			{
				return nullptr;
			}

			_getNextChar(stream); //get \"

			return new CLiteralToken(new CStringLiteral(strConstantValue), mCurrPos - 1);
		}
		
		//try to get a char value
		
		if (currChar == '\'')
		{
			strConstantValue.clear();

			while ((currChar = _getNextChar(stream)) != '\'' && currChar != EOF)
			{
				strConstantValue.push_back(currChar);
			}

			if (currChar == EOF)
			{
				return nullptr;
			}

			_getNextChar(stream); //get \'

			return new CLiteralToken(new CCharLiteral(strConstantValue), mCurrPos - 1);
		}
		
		return nullptr;
	}
	
	std::map<std::string, E_TOKEN_TYPE> CLexer::_readTokensMapFromFile(const std::string& filename, Result& result)
	{
		std::map<std::string, E_TOKEN_TYPE> tokensMap;

		result = RV_SUCCESS;

		std::ifstream configFileWithTokens;
		
		configFileWithTokens.open(filename.c_str());

		if (!configFileWithTokens.is_open())
		{
			result = RV_FILE_NOT_FOUND;

			return tokensMap;
		}

		//parse config file
		std::string currConfigLine;
		std::string tokenName;
		std::string value;

		U32 pos     = 0;
		U32 prevPos = 0;

		while (std::getline(configFileWithTokens, currConfigLine))
		{
			//the parsing of a current config line
			//its representation (token_name  token_value_in_enum_E_TOKEN_TYPE)

			pos = currConfigLine.find_first_of('(');

			if (pos == -1) // there is no open bracket
			{
				result = RV_INCORRECT_CONFIG;

				break;
			}

			//skip white spaces
			pos     = currConfigLine.find_first_not_of(' ', pos + 1);
			prevPos = pos;

			//try to get a name of token
			pos = currConfigLine.find_first_of(' ', pos);

			if (pos == -1) // delimiter between name and value is absent
			{
				result = RV_INCORRECT_CONFIG;

				break;
			}

			tokenName = currConfigLine.substr(prevPos, pos - prevPos);
			
			//skip white spaces
			pos     = currConfigLine.find_first_not_of(' ', pos);
			prevPos = pos;
			
			//check close bracket
			pos = currConfigLine.find_first_of(')', pos);

			if (pos == -1) // there is no close bracket
			{
				result = RV_INCORRECT_CONFIG;

				break;
			}

			value = currConfigLine.substr(prevPos, pos - prevPos);
			
			//erase all white spaces if they exist
			while ((pos = value.find(' ')) != -1)
			{
				value.erase(pos, 1);
			}

			tokensMap.insert(std::make_pair(tokenName, (E_TOKEN_TYPE)atoi(value.c_str())));
		}

		configFileWithTokens.close();

		return tokensMap;
	}
}
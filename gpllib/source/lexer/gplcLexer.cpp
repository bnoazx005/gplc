/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class defenition

	\todo", TT_DEFAULT }, Add strings and chars tokens", TT_DEFAULT }, By now all strings are recognized as wide characters strings (unicode strings). Should thing about the way to change it.", TT_DEFAULT }, Add literals for strings
*/

#include "lexer\gplcLexer.h"
#include "lexer\gplcTokens.h"
#include "common\gplcConstants.h"
#include "common/gplcValues.h"
#include "lexer/gplcInputStream.h"
#include <cctype>


namespace gplc
{
	CLexer::TReservedTokensTable CLexer::mReservedTokensMap
	{
		{ "=", TT_ASSIGN_OP },
		{ ":", TT_COLON },
		{ ";", TT_SEMICOLON },
		{ ",", TT_COMMA },
		{ "*", TT_STAR },
		{ "&", TT_AMPERSAND },
		{ ">", TT_GT },
		{ "<", TT_LT },
		{ ">=", TT_GE },
		{ "<=", TT_LE },
		{ "!=", TT_NE },
		{ "==", TT_EQ },
		{ "+", TT_PLUS },
		{ "-", TT_MINUS },
		{ "/", TT_SLASH },
		{ "\\", TT_BACKSLASH },
		{ "(", TT_OPEN_BRACKET },
		{ ")", TT_CLOSE_BRACKET },
		{ "[", TT_OPEN_SQR_BRACE },
		{ "]", TT_CLOSE_SQR_BRACE },
		{ "{", TT_OPEN_BRACE },
		{ "}", TT_CLOSE_BRACE },
		{ "|", TT_VLINE },
		{ ".", TT_POINT },
		{ "int8", TT_INT8_TYPE },
		{ "int16", TT_INT16_TYPE },
		{ "int32", TT_INT32_TYPE },
		{ "int64", TT_INT64_TYPE },
		{ "uint8", TT_UINT8_TYPE },
		{ "uint16", TT_UINT16_TYPE },
		{ "uint32", TT_UINT32_TYPE },
		{ "uint64", TT_UINT64_TYPE },
		{ "float", TT_FLOAT_TYPE },
		{ "double", TT_DOUBLE_TYPE },
		{ "string", TT_STRING_TYPE },
		{ "char", TT_CHAR_TYPE },
		{ "bool", TT_BOOL_TYPE },
		{ "void", TT_VOID_TYPE },
		{ "enum", TT_ENUM_TYPE },
		{ "struct", TT_STRUCT_TYPE },
		{ "if", TT_IF_KEYWORD },
		{ "else", TT_ELSE_KEYWORD },
		{ "while", TT_WHILE_KEYWORD },
		{ "for", TT_FOR_KEYWORD },
		{ "loop", TT_LOOP_KEYWORD },
		{ "->", TT_ARROW },
		{ "return", TT_RETURN_KEYWORD },
		{ "!", TT_NOT },
		{ "&&", TT_AND },
		{ "||", TT_OR },
		{ "module", TT_MODULE_KEYWORD },
		{ "static", TT_STATIC_KEYWORD },
		{ "break", TT_BREAK_KEYWORD },
		{ "continue", TT_CONTINUE_KEYWORD },
		{ "import", TT_IMPORT_KEYWORD },
		{ "as", TT_AS_KEYWORD },
		{ "%", TT_PERCENT_SIGN },
	};

	CLexer::CLexer():
		ILexer(), mCurrPos(0), mCurrLine(1), mpLastRecognizedToken(nullptr)
	{
	}

	CLexer::~CLexer()
	{
	}

	Result CLexer::Init(IInputStream* pInputStream)
	{
		mpInputStream = pInputStream;

		if (!mpInputStream)
		{
			return RV_INVALID_ARGUMENTS;
		}

		Result result = Reset();

		if (!SUCCESS(result))
		{
			return result;
		}

		return mpInputStream->Open();
	}
	
	Result CLexer::Reset()
	{
		CToken* pCurrToken = nullptr;

		for (U32 i = 0; i < mpTokens.size(); ++i) //release the memory
		{
			pCurrToken = mpTokens[i];

			if (pCurrToken == nullptr)
			{
				continue;
			}

			delete pCurrToken;
		}

		mpTokens.clear();

		mCurrPos  = 0;
		mCurrLine = 1;

		mpLastRecognizedToken = nullptr;

		mCurrStreamBuffer.clear();
		
		while (!mpPeekTokensBuffer.empty())
		{
			mpPeekTokensBuffer.pop();
		}

		return RV_SUCCESS;
	}

	const CToken* CLexer::GetCurrToken()
	{
		if (!mpLastRecognizedToken)
		{
			return GetNextToken();
		}

		return mpLastRecognizedToken;
	}

	const CToken* CLexer::GetNextToken()
	{
		CToken* pToken = nullptr;
		
		if (!mpPeekTokensBuffer.empty())
		{
			pToken = mpPeekTokensBuffer.front();
			mpPeekTokensBuffer.pop();
		}
		else
		{
			pToken = _scanNextToken();
		}

		mpTokens.push_back(pToken);

		mpLastRecognizedToken = pToken;

		return mpLastRecognizedToken;
	}

	const CToken* CLexer::PeekNextToken(U32 numOfSteps)
	{
		if (!numOfSteps)
		{
			return mpLastRecognizedToken;
		}

		CToken* pToken = nullptr;
		
		if (numOfSteps <= mpPeekTokensBuffer.size())
		{
			return *(mpPeekTokensBuffer._Get_container().cbegin() + (numOfSteps - 1));
		}

		for (U32 i = 0; i < numOfSteps - mpPeekTokensBuffer.size(); ++i)
		{
			pToken = _scanNextToken();

			mpPeekTokensBuffer.push(pToken);
		}
		
		return pToken;
	}
	
	C8 CLexer::_getNextChar(std::string& currStreamBuffer, IInputStream* pInputStream, U32& currPos)
	{
		_streamInputData(currStreamBuffer, pInputStream);

		// if current buffer is still empty this means we've reached the end of a file
		if (currStreamBuffer.empty())
		{
			return EOF;
		}

		C8 currCh = currStreamBuffer.front();

		currStreamBuffer.erase(currStreamBuffer.cbegin()); // remove read symbol

		++currPos;

		return currCh;
	}

	C8 CLexer::_peekNextChar(std::string& currStreamBuffer, IInputStream* pInputStream, U32& currPos, U32 offset)
	{
		_streamInputData(currStreamBuffer, pInputStream);
		
		// if current buffer is still empty this means we've reached the end of a file
		if (currStreamBuffer.empty())
		{
			return EOF;
		}

		auto iter = currStreamBuffer.begin() + offset;

		return iter != currStreamBuffer.cend() ? *iter : EOF;
	}
	
	CToken* CLexer::_scanNextToken()
	{
		C8 currCh = ' ';

		CToken* pRecognizedToken = nullptr;

		while ((currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos)) != EOF)
		{
			// skip whitespaces
			if (std::isblank(currCh))
			{
				continue;
			}

			if (std::isspace(currCh))
			{
				_pushNewLine(mCurrPos, mCurrLine);

				continue;
			}

			// skip comments
			if (_skipComments(currCh))
			{
				continue;
			}
			
			// try to parse literal
			if (pRecognizedToken = _tryRecognizeLiteral(currCh))
			{
				return pRecognizedToken;
			}

			// try to parse reserved keywords
			if (pRecognizedToken = _tryRecognizeKeywordOrIdentifier(currCh))
			{
				return pRecognizedToken;
			}
		}

		return nullptr;
	}

	bool CLexer::_skipComments(C8 currCh)
	{
		if (currCh != '/')
		{
			return false;
		}

		currCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0);

		if (std::isspace(currCh))
		{
			_pushNewLine(mCurrPos, mCurrLine);

			return false;
		}

		switch (currCh)
		{
			case '/':
				_getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				_skipSingleLineComment();
				break;
			case '*':
				_getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				_skipMultiLineComment();
				break;
			default:
				return false;
		}

		return true;
	}

	void CLexer::_skipSingleLineComment()
	{
		C8 currCh = ' ';

		while ((currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos)) != EOF && currCh != '\n')
		{
		}		

		_pushNewLine(mCurrPos, mCurrLine);
	}

	void CLexer::_skipMultiLineComment()
	{
		C8 currCh = ' ';
		C8 nextCh = ' ';

		U32 x = mCurrPos;
		U32 y = mCurrLine;

		while ((currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos)) != EOF && currCh != '*' ||
			   (currCh == '*' && (nextCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0)) != '/'))
		{
			_skipComments(currCh);
		}

		switch (currCh)
		{
			case EOF:
				// the end of the file was reached, but there is no end of the comment
				OnErrorOutput.Invoke({ LE_INVALID_END_OF_MULTILINE_COMMENT, x, y });
				break;

			case '*':
				// try to read '/'
				currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);

				if (currCh != '/')
				{
					// invalid end of the multi-line comment
					OnErrorOutput.Invoke({ LE_INVALID_END_OF_MULTILINE_COMMENT, x, y });
				}
				break;
		}
	}

	CToken* CLexer::_tryRecognizeKeywordOrIdentifier(C8 currCh)
	{
		if (std::isdigit(currCh))
		{
			return nullptr;
		}

		bool isFound = false;

		std::string currSequence;

		auto iter = mReservedTokensMap.cbegin();

		U32 x = mCurrPos;
		U32 y = mCurrLine;

		// try to detect identifier
		if (std::isalpha(currCh) || currCh == '_')
		{
			currSequence.push_back(currCh);

			while ((currCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0)) != EOF && (std::isalnum(currCh) || currCh == '_'))
			{
				currSequence.push_back(currCh);

				currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
			} 

			// the sequence is a keyword
			if ((iter = mReservedTokensMap.find(currSequence)) != mReservedTokensMap.cend())
			{
				return new CToken((*iter).second, x, y);
			}

			// \todo Uncomment this code later when refactoring will be done
			// try to parse some of literals true, false, null
			if (currSequence == "false")
			{
				return new CLiteralToken(new CBoolValue(false), x, y);
			}

			if (currSequence == "true")
			{
				return new CLiteralToken(new CBoolValue(true), x, y);
			}

			if (currSequence == "null")
			{
				return new CLiteralToken(new CPointerValue(), x, y);
			}

			return new CIdentifierToken(currSequence, x, y);
		}

		// try to detect some operator symbol
		currSequence += currCh;
		currSequence += _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0);

		// the sequence is a keyword
		for (U8 i = 0; i < 2; ++i)
		{
			if ((iter = mReservedTokensMap.find(currSequence)) != mReservedTokensMap.cend())
			{
				if (i == 0)
				{
					_getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos); // take symbol from buffer which was peeked
				}

				return new CToken((*iter).second, x, y);
			}

			currSequence = currCh;
		}
				
		return nullptr;
	}

	CToken* CLexer::_tryRecognizeLiteral(C8 currCh)
	{
		CToken* pToken = nullptr;

		if (pToken = _tryRecognizeNumberLiteral(currCh))
		{
			return pToken;
		}

		if (pToken = _tryRecognizeStringOrCharLiteral(currCh))
		{
			return pToken;
		}

		return nullptr;
	}

	CToken* CLexer::_tryRecognizeStringOrCharLiteral(C8 currCh)
	{
		bool isString = currCh == '\"';

		if (currCh != '\'' && !isString)
		{
			return nullptr;
		}

		U32 x = mCurrPos;
		U32 y = mCurrLine;

		std::string literal;

		while ((currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos)) != '\'' && currCh != EOF && currCh != '\"')
		{
			literal += currCh;
		}

		if (currCh == EOF)
		{
			return nullptr;
		}

		return isString ? new CLiteralToken(new CStringValue(literal), x, y) : new CLiteralToken(new CCharValue(literal), x, y);
	}

	CToken* CLexer::_tryRecognizeNumberLiteral(C8 currCh)
	{
		static const std::string hexAlphabet { "abcdefABCDEF" };

		C8 nextCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0);

		if (currCh != '.' && !std::isdigit(currCh) || (currCh == '.' && !std::isdigit(nextCh)))
		{
			return nullptr;
		}

		std::string numberLiteral{ currCh };

		U32 x = mCurrPos;
		U32 y = mCurrLine;
		
		U8 numberType = NB_INT | NB_SIGNED; // flags: 0x0 - int; 0x1 - floating point; 0x80 - signed; 0x40 - long; 0x20 - long; 
		//0x10 - hex; 0x8 - oct; 0x4 - bin; floating point value always has sign bit and cannot has hex, oct, bin representations' bits.
		
		// try recognize radix
		if (currCh == '0')
		{
			if (std::isdigit(nextCh)) // possibly octal, also it can be floatint point value or just 0
			{
				numberType |= NB_OCT;

				currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);

				numberLiteral.push_back(currCh);

				while (std::isdigit(currCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0)) && currCh != '9')
				{
					numberLiteral.push_back(currCh);

					currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				}
			}

			if (nextCh == 'x' || nextCh == 'X') // hexagonal 
			{
				numberType |= NB_HEX;

				currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);

				while (std::isdigit(currCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0)))
				{
					numberLiteral.push_back(currCh);

					currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				}
			}

			if (nextCh == 'b' || nextCh == 'B') // binary
			{
				numberType |= NB_BIN;

				currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				
				while ((currCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0)) == '0' || currCh == '1')
				{
					numberLiteral.push_back(currCh);

					currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				}
			}

			if (nextCh == '.') // floating point value
			{
				numberType |= NB_FLOAT;
				
				currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);

				do
				{
					numberLiteral.push_back(currCh);
				}
				while (std::isdigit(currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos)));
			}

			// just zero
		}
		else if (currCh == '.' || std::isdigit(currCh)) // decimal or floating point value
		{
			if (currCh == '.')
			{
				numberType |= NB_FLOAT;
			}

			while (std::isdigit(currCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0)))
			{
				numberLiteral.push_back(currCh);

				currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
			}
			
			if (currCh == '.')
			{
				numberType |= NB_FLOAT;

				numberLiteral.push_back(currCh);

				_getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);

				while (std::isdigit(currCh = _peekNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos, 0)))
				{
					numberLiteral.push_back(currCh);

					currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				}
			}
		}
		
		//check up literals
		static const std::string allowableIntLiterals = "uL";

		U8 literalsCount = 0x0; 

		switch (numberType & NB_FLOAT)
		{
			case NB_INT:

				//all possible literals are placed in allowableLiterals
				while (allowableIntLiterals.find_first_of(currCh) != -1)
				{
					switch (currCh)
					{
						case 'L':
							numberType |= (numberType & NB_LONG) ? NB_ADD_LONG : NB_LONG;
							break;
						case 'u':
							numberType &= ~NB_SIGNED; //clear 'long' bit
							break;
						default:
							numberType |= NB_SIGNED;
							break;
					}

					currCh = _getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				}

				if (currCh == 'f')
				{
					numberType |= NB_FLOAT;

					_getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				}

				break;

			case NB_FLOAT:

				//try to get suffix 
				//there is possible only one kind of suffix for floating point values. It's 'f'
				if (currCh == 'f')
				{
					numberType &= ~NB_LONG; //clear 'long' bit

					_getNextChar(mCurrStreamBuffer, mpInputStream, mCurrPos);
				}
				else if (allowableIntLiterals.find_first_of(currCh) != -1)
				{
					return nullptr; //incorrect literal for floating point was found
				}
				else //double
				{
					numberType |= NB_LONG;
				}

				break;
		}
				
		// token's construction
		U32 numSysBasis = (numberType & NB_HEX) | (numberType & NB_OCT) | ((numberType & NB_BIN) >> 1);
				
		bool isLong = numberType & NB_LONG;

		switch (numberType & NB_FLOAT)
		{
			case 0: // integer

				switch (numberType & NB_SIGNED) //Is it signed? If bit is turn on then it's signed value
				{
					case 0: //unsigned
						return new CLiteralToken(new CUIntValue(strtoul(numberLiteral.c_str(), nullptr, numSysBasis), isLong), x, y);
					default:
						return new CLiteralToken(new CIntValue(strtol(numberLiteral.c_str(), nullptr, numSysBasis), isLong), x, y);
				}

				break;

			case 1: // floating point

				switch (numberType & NB_LONG) //1 - double; 0; - float
				{
					case 0:
						return new CLiteralToken(new CFloatValue(atof(numberLiteral.c_str())), x, y);

					case NB_LONG:
						return new CLiteralToken(new CDoubleValue(atof(numberLiteral.c_str())), x, y);
				}

				break;

			default: // this case won't be never reached, but let it be here for safe code execution
				return nullptr;
		}

		return nullptr;
	}

	void CLexer::_streamInputData(std::string& currStreamBuffer, IInputStream* pInputStream)
	{
		// stream a new portion of data
		if (currStreamBuffer.empty())
		{
			auto result = pInputStream->ReadLine();

			currStreamBuffer.append(result.IsOk() ? result.Get() : "");
		}
	}

	void CLexer::_pushNewLine(U32& currPos, U32& currLine)
	{
		++currLine;

		currPos = 0;
	}
}
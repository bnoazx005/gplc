/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class declaration

	\todo 
*/

#ifndef GPLC_LEXER_H
#define GPLC_LEXER_H


#include "..\common\gplcTypes.h"
#include "gplcTokens.h"
#include <string>
#include <vector>
#include <map>


namespace gplc
{

	class CLexer
	{
		public:
			CLexer();
			~CLexer();

			Result Init(const std::wstring& inputStream, const std::wstring& configFilename, TLexerErrorInfo* errorInfo);

			Result Reset();

			const CToken* GetCurrToken();

			const CToken* GetNextToken();

			const CToken* PeekNextToken(U32 numOfSteps = 1) const;
		private:
			CLexer(const CLexer& lexer);

			W16 _getCurrChar(const std::wstring& stream) const;

			W16 _getNextChar(const std::wstring& stream);
			
			W16 _peekNextChar(const std::wstring& stream, U32 offset = 1) const;

			CToken* _scanToken(const std::wstring& stream, U32& pos);

			std::map<std::wstring, E_TOKEN_TYPE> _readTokensMapFromFile(const std::wstring& filename, Result& result);
		private:
			U32                                  mCurrPos;
			U32                                  mCurrLine;
			U32                                  mCurrTokenIndex;

			std::map<std::wstring, E_TOKEN_TYPE> mReservedTokensMap;
			std::vector<CToken*>                 mTokens;
	};
}

#endif
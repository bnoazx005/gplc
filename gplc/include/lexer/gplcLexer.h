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
#include <string>
#include <vector>


namespace gplc
{
	//forward declarations
	class CToken;


	class CLexer
	{
		public:
			CLexer();
			~CLexer();

			Result Init(const std::wstring& inputStream, TLexerErrorInfo* errorInfo);

			Result Reset();

			const CToken* GetCurrToken();

			const CToken* GetNextToken();

			const CToken* PeekNextToken(U32 numOfSteps = 1) const;
		private:
			CLexer(const CLexer& lexer);

			CToken* _scanToken(const std::wstring& stream, U32& pos);
		private:
			U32                  mCurrPos;
			U32                  mCurrLine;
			U32                  mCurrTokenIndex;

			std::vector<CToken*> mTokens;
	};
}

#endif
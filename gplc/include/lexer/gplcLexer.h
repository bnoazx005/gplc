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


namespace gplc
{
	class CLexer
	{
		public:
			CLexer();
			~CLexer();

			Result Init(const std::wstring& inputStream);
			Result Free();

			Result Reset();

			const CToken* GetNextToken();

			const CToken* PeekNextToken(int numOfSteps = 1) const;
		private:
			CLexer(const CLexer& lexer);
		private:
			std::vector<CToken> mTokens;
	};
}

#endif
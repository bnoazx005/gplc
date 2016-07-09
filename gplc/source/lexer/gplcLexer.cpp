/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class defenition

	\todo
*/

#include "lexer\gplcLexer.h"


namespace gplc
{
	CLexer::CLexer()
	{

	}

	CLexer::CLexer(const CLexer& lexer)
	{
	}

	CLexer::~CLexer()
	{

	}

	Result CLexer::Init(const std::wstring& inputStream)
	{
		return RV_FAIL;
	}

	Result CLexer::Free()
	{
		return RV_FAIL;
	}

	Result CLexer::Reset()
	{
		return RV_FAIL;
	}

	const CToken* CLexer::GetNextToken()
	{
		return nullptr;
	}

	const CToken* CLexer::PeekNextToken(int numOfSteps) const
	{
		return nullptr;
	}
}
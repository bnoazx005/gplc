/*!
\author Ildar Kasimov
\date   09.07.2016
\copyright

\brief The file contains tokens' defenition

\todo
*/

#include "lexer\gplcTokens.h"


namespace gplc
{

	/*!
		CToken's defenition
	*/

	CToken::CToken():
		mType(TT_DEFAULT)
	{
	}

	CToken::CToken(const CToken& token):
		mType(TT_DEFAULT)
	{
	}

	CToken::CToken(E_TOKEN_TYPE type):
		mType(type)
	{
	}

	CToken::~CToken()
	{
	}

	E_TOKEN_TYPE CToken::GetType() const
	{
		return mType;
	}

	/*!
		CIntToken's defenition
	*/

	CIntToken::CIntToken(int value):
		CToken(TT_INTEGER), mValue(value)
	{
	}

	CIntToken::CIntToken():
		CToken(TT_INTEGER)
	{
	}

	CIntToken::CIntToken(const CIntToken& token):
		CToken(token)
	{
	}

	CIntToken::~CIntToken()
	{
	}

	int CIntToken::GetValue() const
	{
		return mValue;
	}
}
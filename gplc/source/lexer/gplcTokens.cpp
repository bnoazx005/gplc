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

	/*CToken::CToken(const CToken& token):
		mType(TT_DEFAULT)
	{
	}*/

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

	std::wstring CToken::ToString() const
	{
		return L"(Token: TT_DEFAULT)";
	}

	/*!
		CIdentifierToken defenition
	*/

	CIdentifierToken::CIdentifierToken(const std::wstring& name) :
		CToken(TT_IDENTIFIER), mName(name)
	{
	}

	CIdentifierToken::CIdentifierToken() :
		CToken(TT_IDENTIFIER)
	{
	}

	CIdentifierToken::CIdentifierToken(const CIdentifierToken& token) :
		CToken(token)
	{
	}

	CIdentifierToken::~CIdentifierToken()
	{
	}

	const std::wstring CIdentifierToken::GetName() const
	{
		return mName;
	}
}
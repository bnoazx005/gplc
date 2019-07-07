/*!
\author Ildar Kasimov
\date   09.07.2016
\copyright

\brief The file contains tokens' defenition

\todo
*/

#include "lexer\gplcTokens.h"
#include "common/gplcLiterals.h"


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

	CToken::CToken(E_TOKEN_TYPE type, U32 posAtStream):
		mType(type), mPos(posAtStream)
	{
	}

	CToken::~CToken()
	{
	}

	E_TOKEN_TYPE CToken::GetType() const
	{
		return mType;
	}

	U32 CToken::GetPos() const
	{
		return mPos;
	}

	std::string CToken::ToString() const
	{
		return "(Token: TT_DEFAULT)";
	}


	CLiteralToken::CLiteralToken(const CBaseLiteral* pValue, U32 posAtStream):
		CToken(TT_LITERAL, posAtStream), mpValue(pValue)
	{
	}

	CLiteralToken::~CLiteralToken()
	{
	}

	const CBaseLiteral* CLiteralToken::GetValue() const
	{
		return mpValue;
	}


	/*!
		CIdentifierToken defenition
	*/

	CIdentifierToken::CIdentifierToken(const std::string& name, U32 posAtStream):
		CToken(TT_IDENTIFIER, posAtStream), mName(name)
	{
	}

	CIdentifierToken::CIdentifierToken() :
		CToken(TT_IDENTIFIER, 0)
	{
	}

	CIdentifierToken::CIdentifierToken(const CIdentifierToken& token) :
		CToken(token)
	{
	}

	CIdentifierToken::~CIdentifierToken()
	{
	}

	const std::string CIdentifierToken::GetName() const
	{
		return mName;
	}
}
/*!
\author Ildar Kasimov
\date   09.07.2016
\copyright

\brief The file contains tokens' defenition

\todo
*/

#include "lexer/gplcTokens.h"
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


	CLiteralToken::CLiteralToken(CBaseLiteral* pValue, U32 posAtStream):
		CToken(TT_LITERAL, posAtStream), mpValue(pValue)
	{
	}

	CLiteralToken::~CLiteralToken()
	{
	}

	CBaseLiteral* CLiteralToken::GetValue() const
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


	std::string TokenTypeToString(E_TOKEN_TYPE type)
	{
		switch (type)
		{
			case TT_INT:
				return "int";
  
			case TT_UINT:
				return "uint";
  
			case TT_FLOAT:
				return "float";
  
			case TT_DOUBLE:
				return "double";
  
			case TT_STRING:
				return "string";
  
			case TT_CHAR:
				return "char";
  
			case TT_IDENTIFIER:
				return "\'identifier\'";
  
			case TT_ASSIGN_OP:
				return "\'=\'";
  
			case TT_COLON:
				return "\':\'";
  
			case TT_SEMICOLON:
				return "\';\'";
  
			case TT_COMMA:
				return "\',\'";

			case TT_STAR:
				return "\'*\'";
 
			case TT_AMPERSAND:
				return "\'&\'";
 
			case TT_GT:
				return "\'>\'";
 
			case TT_LT:
				return "\'<\'";
 
			case TT_GE:
				return "\'>=\'";
 
			case TT_LE:
				return "\'<=\'";
 
			case TT_NE:
				return "\'!=\'";
 
			case TT_EQ:
				return "\'==\'";
 
			case TT_PLUS:
				return "\'+\'";
 
			case TT_MINUS:
				return "\'-\'";
 
			case TT_SLASH:
				return "\'/\'";
 
			case TT_BACKSLASH:
				return "\'\\\'";
 
			case TT_OPEN_BRACKET:
				return "\'(\'";
 
			case TT_CLOSE_BRACKET:
				return "\')\'";
 
			case TT_OPEN_SQR_BRACE:
				return "\'[\'";
 
			case TT_CLOSE_SQR_BRACE:
				return "\']\'";
 
			case TT_OPEN_BRACE:
				return "\'{\'";
 
			case TT_CLOSE_BRACE:
				return "\'}\'";
 
			case TT_VLINE:
				return "\'|\'";
 
			case TT_POINT:
				return "\'.\'";
 
			case TT_INT8_TYPE:
				return "\'int8\'";

			case TT_INT16_TYPE:
				return "\'int16\'";

			case TT_INT32_TYPE:
				return "\'int32\'";

			case TT_INT64_TYPE:
				return "\'int64\'";

			case TT_UINT8_TYPE:
				return "\'uint8\'";

			case TT_UINT16_TYPE:
				return "\'uint16\'";

			case TT_UINT32_TYPE:
				return "\'uint32\'";

			case TT_UINT64_TYPE:
				return "\'uint64\'";

			case TT_FLOAT_TYPE:
				return "\'float\'";

			case TT_DOUBLE_TYPE:
				return "\'double\'";

			case TT_STRING_TYPE:
				return "\'string\'";

			case TT_CHAR_TYPE:
				return "\'char\'";

			case TT_BOOL_TYPE:
				return "\'bool\'";

			case TT_VOID_TYPE:
				return "\'void\'";
 
			case TT_ENUM_TYPE:
				return "\'enum\'";
 
			case TT_STRUCT_TYPE:
				return "\'struct\'";
 
			case TT_IF_KEYWORD:
				return "\'if\'";

			case TT_ELSE_KEYWORD:
				return "\'else\'";

			case TT_WHILE_KEYWORD:
				return "\'while\'";

			case TT_FOR_KEYWORD:
				return "\'for\'";

			case TT_LOOP_KEYWORD:
				return "\'loop\'";

			case TT_ARROW:
				return "\'->\'";

			case TT_RETURN_KEYWORD:
				return "\'return\'";

			case TT_NOT:
				return "\'!\'";

			case TT_AND:
				return "\'&&\'";

			case TT_OR:
				return "\'||\'";

			case TT_FALSE:
				return "\'false\'";

			case TT_TRUE:
				return "\'true\'";

			case TT_NULL:
				return "\'null\'";

			case TT_LITERAL:
				return "\'literal\'";

			case TT_DEFAULT:
			default:
				return "\'default\'";
		}
	}
}
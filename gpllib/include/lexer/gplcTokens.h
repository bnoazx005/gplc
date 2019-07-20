/*!
\author Ildar Kasimov
\date   09.07.2016
\copyright

\brief The file contains tokens' declarations

\todo
*/

#ifndef GPLC_TOKENS_H
#define GPLC_TOKENS_H


#include "..\common\gplcTypes.h"
#include <string>


namespace gplc
{
	class CBaseLiteral;


	/*!
		\brief enum E_TOKEN_TYPE

		All possible tokens' types
	*/
	
	enum E_TOKEN_TYPE
	{
		TT_IDENTIFIER      = 6, 
		TT_ASSIGN_OP       = 7, 
		TT_COLON           = 8, 
		TT_SEMICOLON       = 9, 
		TT_COMMA           = 10,
		TT_STAR            = 11,
		TT_AMPERSAND       = 12,
		TT_GT              = 13,
		TT_LT              = 14,
		TT_GE              = 15,
		TT_LE              = 16,
		TT_NE              = 17,
		TT_EQ              = 18,
		TT_PLUS            = 19,
		TT_MINUS           = 20,
		TT_SLASH           = 21,
		TT_BACKSLASH       = 22,
		TT_OPEN_BRACKET    = 23,
		TT_CLOSE_BRACKET   = 24,
		TT_OPEN_SQR_BRACE  = 25,
		TT_CLOSE_SQR_BRACE = 26,
		TT_OPEN_BRACE      = 27,
		TT_CLOSE_BRACE     = 28,
		TT_VLINE           = 29,
		TT_POINT           = 30,
		TT_INT8_TYPE       = 31,
		TT_INT16_TYPE      = 32,
		TT_INT32_TYPE      = 33,
		TT_INT64_TYPE      = 34,
		TT_UINT8_TYPE      = 35,
		TT_UINT16_TYPE     = 36,
		TT_UINT32_TYPE     = 37,
		TT_UINT64_TYPE     = 38,
		TT_FLOAT_TYPE      = 39,
		TT_DOUBLE_TYPE     = 40,
		TT_STRING_TYPE     = 41,
		TT_CHAR_TYPE       = 42,
		TT_BOOL_TYPE       = 43,
		TT_VOID_TYPE       = 44, 
		TT_ENUM_TYPE       = 45, 
		TT_STRUCT_TYPE     = 46, 
		TT_IF_KEYWORD      = 47,
		TT_ELSE_KEYWORD    = 48,
		TT_WHILE_KEYWORD   = 49,
		TT_FOR_KEYWORD     = 50,
		TT_LOOP_KEYWORD    = 51,
		TT_ARROW           = 52,
		TT_RETURN_KEYWORD  = 53,
		TT_NOT             = 54,
		TT_AND             = 55,
		TT_OR              = 56,
		TT_FALSE           = 57,
		TT_TRUE            = 58,
		TT_NULL            = 59,
		TT_MODULE_KEYWORD,
		TT_LITERAL,
		TT_DEFAULT
	};

	/*!
		\brief CToken class
	*/

	class CToken
	{
		public:
			CToken(E_TOKEN_TYPE type, U32 posAtStream, U32 currLine = 0);
			virtual ~CToken() = default;

			E_TOKEN_TYPE GetType() const;

			U32 GetPos() const;

			U32 GetLine() const;

			virtual std::string ToString() const;
		protected:
			CToken() = default;
		protected:
			E_TOKEN_TYPE mType;

			U32          mPos;

			U32          mCurrLine;
	};


	/*!
		\brief CTypedValueToken class 

		It was derived from CToken.
	*/
	
	class CLiteralToken: public CToken
	{
		public:
			CLiteralToken(CBaseLiteral* pValue, U32 posAtStream, U32 currLine = 0);

			virtual ~CLiteralToken() = default;

			CBaseLiteral* GetValue() const;
		protected:
			CLiteralToken() = default;
			CLiteralToken(const CLiteralToken& token) = default;
		protected:
			CBaseLiteral* mpValue;
	};
		
	/*!
		\brief CIdentifierToken class

		It was derived from CToken and describes identifier's name.
	*/

	class CIdentifierToken : public CToken
	{
		public:
			CIdentifierToken(const std::string& name, U32 posAtStream, U32 currLine = 0);
			virtual ~CIdentifierToken() = default;

			const std::string GetName() const;
		protected:
			CIdentifierToken() = default;
			CIdentifierToken(const CIdentifierToken& token) = delete;
		protected:
			const std::string mName;
	};


	std::string TokenTypeToString(E_TOKEN_TYPE type);
}

#endif
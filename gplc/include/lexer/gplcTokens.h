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

	/*!
		\brief enum E_TOKEN_TYPE

		All possible tokens' types
	*/
	
	enum E_TOKEN_TYPE
	{
		TT_INT             = 0,  ///< Signed integer type
		TT_UINT            = 1,  ///< Unsigned integer type
		TT_FLOAT           = 2,  ///< Single precision floating point type
		TT_DOUBLE          = 3,  ///< Double precision floating point type
		TT_STRING          = 4,  ///< String type
		TT_CHAR            = 5,  ///< Char type
		TT_IDENTIFIER      = 6,  ///< Identifier
		TT_ASSIGN_OP       = 7,  ///< =
		TT_COLON           = 8,  ///< :
		TT_SEMICOLON       = 9,  ///< ;
		TT_COMMA           = 10, ///< ,
		TT_STAR            = 11, ///< *
		TT_AMPERSAND       = 12, ///< &
		TT_GT              = 13, ///< >
		TT_LT              = 14, ///< <
		TT_GE              = 15, ///< >=
		TT_LE              = 16, ///< <=
		TT_NE              = 17, ///< !=
		TT_EQ              = 18, ///< ==
		TT_PLUS            = 19, ///< +
		TT_MINUS           = 20, ///< -
		TT_SLASH           = 21, ///< '/'
		TT_BACKSLASH       = 22, ///< '\'
		TT_OPEN_BRACKET    = 23, ///< (
		TT_CLOSE_BRACKET   = 24, ///< )
		TT_OPEN_SQR_BRACE  = 25, ///< [
		TT_CLOSE_SQR_BRACE = 26, ///< ]
		TT_OPEN_BRACE      = 27, ///< {
		TT_CLOSE_BRACE     = 28, ///< }
		TT_VLINE           = 29, ///< |
		TT_POINT           = 30, ///< .
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
		TT_VOID_TYPE       = 44, ///< void
		TT_ENUM_TYPE       = 45, ///< enum
		TT_STRUCT_TYPE     = 46, ///< struct
		TT_DEFAULT
	};

	/*!
		\brief CToken class
	*/

	class CToken
	{
		public:
			CToken();
			CToken(E_TOKEN_TYPE type, U32 posAtStream);
			virtual ~CToken();

			E_TOKEN_TYPE GetType() const;

			U32 GetPos() const;

			virtual std::string ToString() const;
		/*protected:
			CToken(const CToken& token);*/
		protected:
			E_TOKEN_TYPE mType;

			U32          mPos;
	};


	/*!
		\brief CTypedValueToken class 

		It was derived from CToken.
	*/
	
	template <class T>
	class CTypedValueToken: public CToken
	{
		public:
			CTypedValueToken(E_TOKEN_TYPE type, U32 posAtStream, T value):
				CToken(type, posAtStream), mValue(value)
			{
			}

			virtual ~CTypedValueToken()
			{
			}

			T GetValue() const
			{
				return mValue;
			}
		protected:
			CTypedValueToken():
				CToken(TT_INT)
			{
			}

			CTypedValueToken(const CTypedValueToken& token) :
				CToken(token)
			{
			}
		protected:
			T mValue;
	};
		
	/*!
		\brief CIdentifierToken class

		It was derived from CToken and describes identifier's name.
	*/

	class CIdentifierToken : public CToken
	{
	public:
		CIdentifierToken(const std::string& name, U32 posAtStream);
		virtual ~CIdentifierToken();

		const std::string GetName() const;
	protected:
		CIdentifierToken();
		CIdentifierToken(const CIdentifierToken& token);
	protected:
		const std::string mName;
	};
}

#endif
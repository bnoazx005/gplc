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

	enum E_TOKEN_TYPE
	{
		TT_INTEGER,
		TT_FLOAT,
		TT_DOUBLE,
		TT_STRING,
		TT_CHAR,
		TT_IDENTIFIER,
		TT_DEFAULT
	};

	/*!
		\brief CToken class
	*/

	class CToken
	{
		public:
			CToken();
			CToken(E_TOKEN_TYPE type);
			virtual ~CToken();

			E_TOKEN_TYPE GetType() const;

			virtual std::wstring ToString() const;
		/*protected:
			CToken(const CToken& token);*/
		protected:
			E_TOKEN_TYPE mType;
	};


	/*!
		\brief CIntToken class 

		It was derived from CToken.
	*/
	
	class CIntToken: public CToken
	{
		public:
			CIntToken(I32 value);
			virtual ~CIntToken();

			I32 GetValue() const;
		protected:
			CIntToken();
			CIntToken(const CIntToken& token);
		protected:
			I32 mValue;
	};

	
	/*!
		\brief CIdentifierToken class

		It was derived from CToken and describes identifier's name.
	*/

	class CIdentifierToken : public CToken
	{
	public:
		CIdentifierToken(const std::wstring& name);
		virtual ~CIdentifierToken();

		const std::wstring GetName() const;
	protected:
		CIdentifierToken();
		CIdentifierToken(const CIdentifierToken& token);
	protected:
		const std::wstring mName;
	};
}

#endif
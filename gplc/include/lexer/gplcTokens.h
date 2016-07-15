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
		TT_INT        = 0,
		TT_UINT       = 1,
		TT_FLOAT      = 2,
		TT_DOUBLE     = 3,
		TT_STRING     = 4,
		TT_CHAR       = 5,
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
		\brief CNumberToken class 

		It was derived from CToken.
	*/
	
	template <class T>
	class CNumberToken: public CToken
	{
		public:
			CNumberToken(E_TOKEN_TYPE type, T value):
				CToken(type), mValue(value)
			{
			}

			virtual ~CNumberToken()
			{
			}

			T GetValue() const
			{
				return mValue;
			}
		protected:
			CNumberToken():
				CToken(TT_INT)
			{
			}

			CNumberToken(const CNumberToken& token) :
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
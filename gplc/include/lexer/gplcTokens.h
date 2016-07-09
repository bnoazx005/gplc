/*!
\author Ildar Kasimov
\date   09.07.2016
\copyright

\brief The file contains tokens' declarations

\todo
*/

#ifndef GPLC_TOKENS_H
#define GPLC_TOKENS_H


namespace gplc
{

	enum E_TOKEN_TYPE
	{
		TT_INTEGER,
		TT_FLOAT,
		TT_DOUBLE,
		TT_STRING,
		TT_CHAR,
		TT_DEFAULT
	};

	/*!
		\brief CToken class
	*/

	class CToken
	{
		public:
			CToken(E_TOKEN_TYPE type);
			virtual ~CToken();

			E_TOKEN_TYPE GetType() const;
		protected:
			CToken();
			CToken(const CToken& token);
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
			CIntToken(int value);
			virtual ~CIntToken();

			int GetValue() const;
		protected:
			CIntToken();
			CIntToken(const CIntToken& token);
		protected:
			int mValue;
	};
}

#endif
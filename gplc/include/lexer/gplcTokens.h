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
		TT_DEFAULT
	};

	class CToken
	{
		public:
			CToken(E_TOKEN_TYPE type);
			virtual ~CToken();

			virtual E_TOKEN_TYPE GetType() const = 0;
		protected:
			CToken();
			CToken(const CToken& token);
		protected:
			E_TOKEN_TYPE mType;
	};
}

#endif
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
	class CBaseValue;


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
			CLiteralToken(CBaseValue* pValue, U32 posAtStream, U32 currLine = 0);

			virtual ~CLiteralToken() = default;

			CBaseValue* GetValue() const;
		protected:
			CLiteralToken() = default;
			CLiteralToken(const CLiteralToken& token) = default;
		protected:
			CBaseValue* mpValue;
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
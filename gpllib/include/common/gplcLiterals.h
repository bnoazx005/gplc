/*!
	\author Ildar Kasimov
	\date   07.07.2019
	\copyright

	\brief The file contains definitions of literals

	\todo
*/

#ifndef GPLC_LITERALS_H
#define GPLC_LITERALS_H


#include "gplcTypes.h"
#include <string>


namespace gplc
{
	class CType;


	enum E_LITERAL_TYPE
	{
		LT_INT = 0,  ///< Signed integer type
		LT_UINT = 1,  ///< Unsigned integer type
		LT_FLOAT = 2,  ///< Single precision floating point type
		LT_DOUBLE = 3,  ///< Double precision floating point type
		LT_STRING = 4,  ///< String type
		LT_CHAR = 5,  ///< Char type
		LT_BOOLEAN
	};


	class CBaseLiteral
	{
		public:
			CBaseLiteral(E_LITERAL_TYPE type);
			virtual ~CBaseLiteral() = default;

			E_LITERAL_TYPE GetType() const;

			virtual std::string ToString() const = 0;

			virtual CType* GetTypeInfo() const = 0;
		protected:
			CBaseLiteral() = default;
			CBaseLiteral(const CBaseLiteral& literal) = default;
		protected:
			E_LITERAL_TYPE mLiteralType;
	};


	template <typename T>
	class CGenericBaseLiteral : public CBaseLiteral
	{
		public:
			CGenericBaseLiteral(E_LITERAL_TYPE type, T value):
				CBaseLiteral(type), mValue(value)
			{
			}

			~CGenericBaseLiteral() = default;

			T GetValue() const
			{
				return mValue;
			}

			virtual CType* GetTypeInfo() const = 0;
		protected:
			CGenericBaseLiteral() = default;
			CGenericBaseLiteral(const CGenericBaseLiteral& literal) = default;
		protected:
			T mValue;
	};


	class CIntLiteral : public CGenericBaseLiteral<I64>
	{
		public:
			CIntLiteral(I64 value) :
				CGenericBaseLiteral(LT_INT, value)
			{
			}

			std::string ToString() const override;
			
			CType* GetTypeInfo() const override;
	};


	class CUIntLiteral : public CGenericBaseLiteral<U64>
	{
		public:
			CUIntLiteral(U64 value) :
				CGenericBaseLiteral(LT_UINT, value)
			{
			}

			std::string ToString() const override;

			CType* GetTypeInfo() const override;
	};


	class CFloatLiteral : public CGenericBaseLiteral<F32>
	{
		public:
			CFloatLiteral(F32 value) :
				CGenericBaseLiteral(LT_FLOAT, value)
			{
			}

			std::string ToString() const override;

			CType* GetTypeInfo() const override;
	};


	class CDoubleLiteral : public CGenericBaseLiteral<F64>
	{
		public:
			CDoubleLiteral(F64 value) :
				CGenericBaseLiteral(LT_DOUBLE, value)
			{
			}

			std::string ToString() const override;

			CType* GetTypeInfo() const override;
	};


	class CStringLiteral : public CGenericBaseLiteral<std::string>
	{
		public:
			CStringLiteral(const std::string& value) :
				CGenericBaseLiteral(LT_STRING, value)
			{
			}

			std::string ToString() const override;

			CType* GetTypeInfo() const override;
	};


	class CCharLiteral : public CGenericBaseLiteral<std::string>
	{
		public:
			CCharLiteral(const std::string& value) :
				CGenericBaseLiteral(LT_CHAR, value)
			{
			}

			std::string ToString() const override;

			CType* GetTypeInfo() const override;
	};


	class CBoolLiteral : public CGenericBaseLiteral<bool>
	{
		public:
			CBoolLiteral(bool value) :
				CGenericBaseLiteral(LT_BOOLEAN, value)
			{
			}

			std::string ToString() const override;

			CType* GetTypeInfo() const override;
	};
}

#endif
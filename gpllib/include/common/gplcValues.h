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
#include "gplcVisitor.h"
#include <string>


namespace gplc
{
	class CType;
	class CStringValue;


	enum E_LITERAL_TYPE
	{
		LT_INT = 0,  ///< Signed integer type
		LT_UINT = 1,  ///< Unsigned integer type
		LT_FLOAT = 2,  ///< Single precision floating point type
		LT_DOUBLE = 3,  ///< Double precision floating point type
		LT_STRING = 4,  ///< String type
		LT_CHAR = 5,  ///< Char type
		LT_BOOLEAN,
		LT_POINTER,
	};


	class CBaseValue : public IVisitable<TLLVMIRData, ILiteralVisitor<TLLVMIRData>>
	{
		public:
			CBaseValue(E_LITERAL_TYPE type);
			virtual ~CBaseValue() = default;
			
			virtual TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) = 0;

			E_LITERAL_TYPE GetType() const;

			virtual std::string ToString() const = 0;

			virtual CStringValue* ToStringLiteral() const = 0;

			virtual CType* GetTypeInfo() const = 0;
		protected:
			CBaseValue() = default;
			CBaseValue(const CBaseValue& literal) = default;
		protected:
			E_LITERAL_TYPE mLiteralType;
	};


	template <typename T>
	class CGenericBaseValue : public CBaseValue
	{
		public:
			CGenericBaseValue(E_LITERAL_TYPE type, T value):
				CBaseValue(type), mValue(value)
			{
			}

			~CGenericBaseValue() = default;

			T GetValue() const
			{
				return mValue;
			}

			virtual CType* GetTypeInfo() const = 0;
		protected:
			CGenericBaseValue() = default;
			CGenericBaseValue(const CGenericBaseValue& literal) = default;
		protected:
			T mValue;
	};


	class CIntValue : public CGenericBaseValue<I64>
	{
		public:
			CIntValue(I64 value) :
				CGenericBaseValue(LT_INT, value)
			{
			}

			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;

			CStringValue* ToStringLiteral() const override;
			
			CType* GetTypeInfo() const override;
	};


	class CUIntValue : public CGenericBaseValue<U64>
	{
		public:
			CUIntValue(U64 value) :
				CGenericBaseValue(LT_UINT, value)
			{
			}

			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;
			
			CStringValue* ToStringLiteral() const override;

			CType* GetTypeInfo() const override;
	};


	class CFloatValue : public CGenericBaseValue<F32>
	{
		public:
			CFloatValue(F32 value) :
				CGenericBaseValue(LT_FLOAT, value)
			{
			}

			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;
			
			CStringValue* ToStringLiteral() const override;

			CType* GetTypeInfo() const override;
	};


	class CDoubleValue : public CGenericBaseValue<F64>
	{
		public:
			CDoubleValue(F64 value) :
				CGenericBaseValue(LT_DOUBLE, value)
			{
			}
			
			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;

			CStringValue* ToStringLiteral() const override;

			CType* GetTypeInfo() const override;
	};


	class CStringValue : public CGenericBaseValue<std::string>
	{
		public:
			CStringValue(const std::string& value) :
				CGenericBaseValue(LT_STRING, value)
			{
			}
			
			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;

			CStringValue* ToStringLiteral() const override;

			CType* GetTypeInfo() const override;
	};


	class CCharValue : public CGenericBaseValue<std::string>
	{
		public:
			CCharValue(const std::string& value) :
				CGenericBaseValue(LT_CHAR, value)
			{
			}

			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;

			CStringValue* ToStringLiteral() const override;

			CType* GetTypeInfo() const override;
	};


	class CBoolValue : public CGenericBaseValue<bool>
	{
		public:
			CBoolValue(bool value) :
				CGenericBaseValue(LT_BOOLEAN, value)
			{
			}

			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;

			CStringValue* ToStringLiteral() const override;

			CType* GetTypeInfo() const override;
	};


	class CPointerValue : public CGenericBaseValue<TSymbolHandle>
	{
		public:
			CPointerValue(TSymbolHandle handle = InvalidSymbolHandle) :
				CGenericBaseValue(LT_POINTER, handle)
			{
			}

			TLLVMIRData Accept(ILiteralVisitor<TLLVMIRData>* pVisitor) override;

			std::string ToString() const override;

			CStringValue* ToStringLiteral() const override;

			CType* GetTypeInfo() const override;
	};
}

#endif
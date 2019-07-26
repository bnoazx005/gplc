#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"
#include <cassert>


namespace gplc
{
	CBaseValue::CBaseValue(E_LITERAL_TYPE type):
		mLiteralType(type)
	{
	}

	E_LITERAL_TYPE CBaseValue::GetType() const
	{
		return mLiteralType;
	}

	TLLVMIRData CIntValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitIntLiteral(this);
	}

	std::string CIntValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CIntValue::GetTypeInfo() const
	{
		return new CType(CT_INT64, BTS_INT64, 0x0);
	}

	TLLVMIRData CUIntValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CUIntValue::ToString() const
	{
		return std::to_string(mValue);
	}
	
	CType* CUIntValue::GetTypeInfo() const
	{
		return new CType(CT_UINT64, BTS_UINT64, 0x0);
	}

	TLLVMIRData CFloatValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CFloatValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CFloatValue::GetTypeInfo() const
	{
		return new CType(CT_FLOAT, BTS_FLOAT, 0x0);
	}

	TLLVMIRData CDoubleValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CDoubleValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CDoubleValue::GetTypeInfo() const
	{
		return new CType(CT_DOUBLE, BTS_DOUBLE, 0x0);
	}

	TLLVMIRData CStringValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitStringLiteral(this);
	}

	std::string CStringValue::ToString() const
	{
		return std::string("\"").append(mValue).append("\"");
	}

	CType* CStringValue::GetTypeInfo() const
	{
		return new CType(CT_STRING, BTS_POINTER, 0x0);
	}

	TLLVMIRData CCharValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		assert(false); // \todo implement the method
		return {};
	}

	std::string CCharValue::ToString() const
	{
		return std::string("\'").append(mValue).append("\'");
	}

	CType* CCharValue::GetTypeInfo() const
	{
		return new CType(CT_CHAR, BTS_CHAR, 0x0);
	}

	TLLVMIRData CBoolValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CBoolValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CBoolValue::GetTypeInfo() const
	{
		return new CType(CT_BOOL, BTS_BOOL, 0x0);
	}


	TLLVMIRData CPointerValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitNullLiteral(this);
	}

	std::string CPointerValue::ToString() const
	{
		return "null";
	}

	CType* CPointerValue::GetTypeInfo() const
	{
		return new CType(CT_POINTER, BTS_POINTER, 0x0);
	}
}
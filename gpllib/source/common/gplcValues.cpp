#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcSymTable.h"
#include "common/gplcTypesFactory.h"
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

	CStringValue* CIntValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CIntValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CType(mIsLong ? CT_INT64 : CT_INT32, mIsLong ? BTS_INT64 : BTS_INT32, 0x0, "", pSymTable->GetCurrentScopeType());
	}

	bool CIntValue::IsLong() const
	{
		return mIsLong;
	}

	TLLVMIRData CUIntValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitUIntLiteral(this);
	}

	std::string CUIntValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CStringValue* CUIntValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CUIntValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CType(mIsLong ? CT_UINT64 : CT_UINT32, mIsLong ? BTS_UINT64 : BTS_UINT32, 0x0, "", pSymTable->GetCurrentScopeType());
	}

	bool CUIntValue::IsLong() const
	{
		return mIsLong;
	}

	TLLVMIRData CFloatValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFloatLiteral(this);
	}

	std::string CFloatValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CStringValue* CFloatValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CFloatValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CType(CT_FLOAT, BTS_FLOAT, 0x0, "", pSymTable->GetCurrentScopeType());
	}

	TLLVMIRData CDoubleValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitDoubleLiteral(this);
	}

	std::string CDoubleValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CStringValue* CDoubleValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CDoubleValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CType(CT_DOUBLE, BTS_DOUBLE, 0x0, "", pSymTable->GetCurrentScopeType());
	}

	TLLVMIRData CStringValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitStringLiteral(this);
	}

	std::string CStringValue::ToString() const
	{
		return std::string("\"").append(mValue).append("\"");
	}

	CStringValue* CStringValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CStringValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CType(CT_STRING, BTS_POINTER, 0x0, "", pSymTable->GetCurrentScopeType());
	}

	TLLVMIRData CCharValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VititCharLiteral(this);
	}

	std::string CCharValue::ToString() const
	{
		return std::string("\'").append(mValue).append("\'");
	}

	CStringValue* CCharValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CCharValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CType(CT_CHAR, BTS_CHAR, 0x0, "", pSymTable->GetCurrentScopeType());
	}

	TLLVMIRData CBoolValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitBoolLiteral(this);
	}

	std::string CBoolValue::ToString() const
	{
		return std::to_string(mValue);
	}

	CStringValue* CBoolValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CBoolValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CType(CT_BOOL, BTS_BOOL, 0x0, "", pSymTable->GetCurrentScopeType());
	}


	TLLVMIRData CPointerValue::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitNullLiteral(this);
	}

	std::string CPointerValue::ToString() const
	{
		return "null";
	}

	CStringValue* CPointerValue::ToStringLiteral() const
	{
		return new CStringValue(ToString());
	}

	CType* CPointerValue::GetTypeInfo(ITypesFactory* pTypesFactory, ISymTable* pSymTable) const
	{
		return new CPointerType(nullptr);
	}
}
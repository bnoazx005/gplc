#include "common/gplcLiterals.h"
#include "common/gplcTypeSystem.h"


namespace gplc
{
	CBaseLiteral::CBaseLiteral(E_LITERAL_TYPE type):
		mLiteralType(type)
	{
	}

	E_LITERAL_TYPE CBaseLiteral::GetType() const
	{
		return mLiteralType;
	}

	TLLVMIRData CIntLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitIntLiteral(this);
	}

	std::string CIntLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CIntLiteral::GetTypeInfo() const
	{
		return new CType(CT_INT64, BTS_INT64, 0x0);
	}

	TLLVMIRData CUIntLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CUIntLiteral::ToString() const
	{
		return std::to_string(mValue);
	}
	
	CType* CUIntLiteral::GetTypeInfo() const
	{
		return new CType(CT_UINT64, BTS_UINT64, 0x0);
	}

	TLLVMIRData CFloatLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CFloatLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CFloatLiteral::GetTypeInfo() const
	{
		return new CType(CT_FLOAT, BTS_FLOAT, 0x0);
	}

	TLLVMIRData CDoubleLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CDoubleLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CDoubleLiteral::GetTypeInfo() const
	{
		return new CType(CT_DOUBLE, BTS_DOUBLE, 0x0);
	}

	TLLVMIRData CStringLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CStringLiteral::ToString() const
	{
		return std::string("\"").append(mValue).append("\"");
	}

	CType* CStringLiteral::GetTypeInfo() const
	{
		return new CType(CT_STRING, BTS_POINTER, 0x0);
	}

	TLLVMIRData CCharLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CCharLiteral::ToString() const
	{
		return std::string("\'").append(mValue).append("\'");
	}

	CType* CCharLiteral::GetTypeInfo() const
	{
		return new CType(CT_CHAR, BTS_CHAR, 0x0);
	}

	TLLVMIRData CBoolLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CBoolLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CBoolLiteral::GetTypeInfo() const
	{
		return new CType(CT_BOOL, BTS_BOOL, 0x0);
	}

	TLLVMIRData CNullLiteral::Accept(ILiteralVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	std::string CNullLiteral::ToString() const
	{
		return "null";
	}

	CType* CNullLiteral::GetTypeInfo() const
	{
		return new CType(CT_POINTER, BTS_POINTER, 0x0);
	}
}
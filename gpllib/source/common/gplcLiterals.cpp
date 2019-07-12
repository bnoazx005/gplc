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

	std::string CIntLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CIntLiteral::GetTypeInfo() const
	{
		return new CType(CT_INT64, BTS_INT64, 0x0);
	}

	std::string CUIntLiteral::ToString() const
	{
		return std::to_string(mValue);
	}
	
	CType* CUIntLiteral::GetTypeInfo() const
	{
		return new CType(CT_UINT64, BTS_UINT64, 0x0);
	}

	std::string CFloatLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CFloatLiteral::GetTypeInfo() const
	{
		return new CType(CT_FLOAT, BTS_FLOAT, 0x0);
	}

	std::string CDoubleLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CDoubleLiteral::GetTypeInfo() const
	{
		return new CType(CT_DOUBLE, BTS_DOUBLE, 0x0);
	}

	std::string CStringLiteral::ToString() const
	{
		return std::string("\"").append(mValue).append("\"");
	}

	CType* CStringLiteral::GetTypeInfo() const
	{
		return new CType(CT_STRING, BTS_POINTER, 0x0);
	}

	std::string CCharLiteral::ToString() const
	{
		return std::string("\'").append(mValue).append("\'");
	}

	CType* CCharLiteral::GetTypeInfo() const
	{
		return new CType(CT_CHAR, BTS_CHAR, 0x0);
	}

	std::string CBoolLiteral::ToString() const
	{
		return std::to_string(mValue);
	}

	CType* CBoolLiteral::GetTypeInfo() const
	{
		return new CType(CT_BOOL, BTS_BOOL, 0x0);
	}
}
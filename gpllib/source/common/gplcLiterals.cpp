#include "common/gplcLiterals.h"

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
}
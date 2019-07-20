#include "stubInputStream.h"


CStubInputStream::CStubInputStream(const std::vector<std::string>& lines)
{
	std::copy(lines.begin(), lines.end(), std::back_inserter(mLines));
}

CStubInputStream::~CStubInputStream()
{
}

gplc::Result CStubInputStream::Open()
{
	mCurrLineIndex = 0;

	return gplc::RV_SUCCESS;
}

gplc::Result CStubInputStream::Close()
{
	return gplc::RV_SUCCESS;
}

gplc::TResult<std::string> CStubInputStream::ReadLine()
{
	if (mCurrLineIndex >= mLines.size())
	{
		return gplc::TErrorValue(gplc::RV_FAIL);
	}

	return gplc::TOkValue<std::string>(mLines[mCurrLineIndex++]);
}
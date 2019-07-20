#include "lexer/gplcInputStream.h"


namespace gplc
{
	CFileInputStream::CFileInputStream(const std::string& filename):
		mFilename(filename)
	{
	}

	CFileInputStream::~CFileInputStream()
	{
		Close();
	}

	Result CFileInputStream::Open() 
	{
		if (mInputFile.is_open())
		{
			return RV_SUCCESS;
		}

		mInputFile.open(mFilename);

		if (!mInputFile.is_open())
		{
			return RV_FILE_NOT_FOUND;
		}

		return RV_SUCCESS;
	}

	Result CFileInputStream::Close()
	{
		if (!mInputFile.is_open())
		{
			return RV_FAIL;
		}

		mInputFile.close();

		return RV_SUCCESS;
	}

	TResult<std::string> CFileInputStream::ReadLine()
	{
		if (!mInputFile.is_open())
		{
			return TErrorValue<E_RESULT_VALUE>(RV_FAIL);
		}

		std::string readLine;

		if (!std::getline(mInputFile, readLine))
		{
			return TErrorValue<E_RESULT_VALUE>(RV_FAIL);
		}

		return TOkValue(readLine.append("\n"));
	}
}
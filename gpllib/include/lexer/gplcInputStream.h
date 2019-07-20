/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright
*/

#ifndef GPLC_INPUT_STREAM_H
#define GPLC_INPUT_STREAM_H


#include "common/gplcTypes.h"
#include "utils/CResult.h"
#include <string>
#include <fstream>


namespace gplc
{
	class IInputStream
	{
		public:
			IInputStream() = default;
			virtual ~IInputStream() = default;

			virtual Result Open() = 0;
			virtual Result Close() = 0;

			virtual TResult<std::string> ReadLine() = 0;
		protected:
			IInputStream(IInputStream&) = delete;
	};


	class CFileInputStream: public IInputStream
	{
		public:
			CFileInputStream(const std::string& filename);
			virtual ~CFileInputStream();

			Result Open() override;
			Result Close() override;

			TResult<std::string> ReadLine() override;
		protected:
			CFileInputStream() = delete;
			CFileInputStream(CFileInputStream&) = delete;
		protected:
			std::string   mFilename;

			std::ifstream mInputFile;
	};
}

#endif
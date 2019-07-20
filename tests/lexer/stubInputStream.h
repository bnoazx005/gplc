/*!
	\author Ildar Kasimov
	\date   20.07.2019
	\copyright

	\brief The file contains stub input stream class

	\todo
*/
#ifndef STUB_INPUT_STREAM_H
#define STUB_INPUT_STREAM_H


#include <gplc.h>
#include <vector>
#include <string>


class CStubInputStream final : public gplc::IInputStream
{
	public:
		CStubInputStream(const std::vector<std::string>& lines);
		virtual ~CStubInputStream();

		gplc::Result Open() override;
		gplc::Result Close() override;

		gplc::TResult<std::string> ReadLine() override;
	private:
		std::vector<std::string> mLines;

		gplc::U32 mCurrLineIndex;
};

#endif
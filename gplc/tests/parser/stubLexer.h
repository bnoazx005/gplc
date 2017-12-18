/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright

	\brief The file contains stub lexer class

	\todo
*/
#ifndef STUB_LEXER_H
#define STUB_LEXER_H


#include "lexer\gplcLexer.h"


class CStubLexer : public gplc::ILexer
{
	public:
		CStubLexer();
		virtual ~CStubLexer();

		/*!
			\brief The function reads input stream of characters and prepares a tokens' sequence.

			\param[in] inputStream An input characters sequence.
			\param[in] configFilename A name of file, which stores reserved keywords' declarations.

			\return A function's result code.
		*/

		virtual gplc::Result Init(const std::wstring& inputStream, const std::wstring& configFilename);

		/*!
			\brief The function clears the current state of an object.

			The method implicitly is called from Init function of the class.

			\return A function's result code.
		*/

		virtual gplc::Result Reset();

		/*!
			\brief The function returns a current token from tokens' sequence

			A call of this method doesn't change a state of an object.

			\return A current token from tokens' sequence
		*/

		virtual const gplc::CToken* GetCurrToken() const;

		/*!
			\brief The function returns next token from tokens' sequence

			A call of this method increments value of an inner pointer to tokens' sequence.

			\return A next token from tokens' sequence
		*/

		virtual const gplc::CToken* GetNextToken();

		/*!
			\brief The function allows to peek some token after the current one with specified offset.

			\return A token from tokens' sequence with specified offset from the current one.
		*/

		virtual const gplc::CToken* PeekNextToken(gplc::U32 numOfSteps = 1) const;

		/*!
			\brief The method saves current state of an object. It's used for backtracking.
		*/

		virtual void SavePosition();

		/*!
			\brief The method restores previous state of an object. It's used for backtracking.
		*/

		virtual void RestorePosition();
	protected:
		CStubLexer(const CStubLexer& lexer);
	private:
		gplc::U32                  mCurrTokenIndex;
		gplc::U32                  mSavedTokenIndex;

		std::vector<gplc::CToken*> mTokens;
};

#endif
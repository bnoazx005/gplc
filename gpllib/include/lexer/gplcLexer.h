/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains lexer's class declaration

	\todo 
*/

#ifndef GPLC_LEXER_H
#define GPLC_LEXER_H


#include "..\common\gplcTypes.h"
#include "gplcTokens.h"
#include "..\utils\Delegate.h"
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>


namespace gplc
{
	class IInputStream;


	/*!
		\brief ILexer interface

		All lexers should implement its methods. The first goal of using it is unit testing.
	*/
	
	class ILexer
	{
		public:
			ILexer() = default;
			virtual ~ILexer() = default;

			/*!
				\brief The function reads input stream of characters and prepares a tokens' sequence

				\param[in] pInputStream An input characters sequence

				\return A function's result code.
			*/

			virtual Result Init(IInputStream* pInputStream) = 0;

			/*!
				\brief The function clears the current state of an object.

				The method implicitly is called from Init function of the class.

				\return A function's result code.
			*/

			virtual Result Reset() = 0;

			/*!
				\brief The function returns a current token from tokens' sequence

				A call of this method doesn't change a state of an object

				\return A current token from tokens' sequence
			*/

			virtual const CToken* GetCurrToken() = 0;

			/*!
				\brief The function returns next token from tokens' sequence

				A call of this method increments value of an inner pointer to tokens' sequence.

				\return A next token from tokens' sequence
			*/

			virtual const CToken* GetNextToken() = 0;

			/*!
				\brief The function allows to peek some token after the current one with specified offset.

				\return A token from tokens' sequence with specified offset from the current one.
			*/

			virtual const CToken* PeekNextToken(U32 numOfSteps = 1) = 0;
		public:
			CDelegate<void, const TLexerErrorInfo&> OnErrorOutput;
		protected:
			ILexer(const ILexer& lexer) = delete;
	};


	/*!
		\brief CLexer class

		The class provides methods for tokens' recognition. All allowable tokens' types are placed in gplcTokens.h.
		All reserved keywords are placed in a special file .tokens. They duplicate E_TOKEN_TYPE's values.
	*/

	class CLexer : public ILexer
	{
		protected:
			typedef std::unordered_map<std::string, E_TOKEN_TYPE> TReservedTokensTable;
			typedef std::queue<CToken*>                           TReadTokensQueue;
		public:
			CLexer();
			virtual ~CLexer();

			/*!
				\brief The function reads input stream of characters and prepares a tokens' sequence.

				\param[in] pInputStream An input characters sequence

				\return A function's result code.
			*/

			virtual Result Init(IInputStream* pInputStream);

			/*!
				\brief The function clears the current state of an object.

				The method implicitly is called from Init function of the class.

				\return A function's result code.
			*/

			virtual Result Reset();

			/*!
				\brief The function returns a current token from tokens' sequence

				A call of this method doesn't change a state of an object

				\return A current token from tokens' sequence
			*/

			virtual const CToken* GetCurrToken();

			/*!
				\brief The function returns next token from tokens' sequence

				A call of this method increments value of an inner pointer to tokens' sequence.

				\return A next token from tokens' sequence
			*/

			virtual const CToken* GetNextToken();

			/*!
				\brief The function allows to peek some token after the current one with specified offset.

				\return A token from tokens' sequence with specified offset from the current one.
			*/

			virtual const CToken* PeekNextToken(U32 numOfSteps = 1);
		private:
			CLexer(const CLexer& lexer) = delete;

			C8 _getNextChar(std::string& currStreamBuffer, IInputStream* pInputStream, U32& currPos);

			bool _skipComments(C8 currCh);

			void _skipSingleLineComment();

			void _skipMultiLineComment();

			C8 _peekNextChar(std::string& currStreamBuffer, IInputStream* pInputStream, U32& currPos, U32 offset = 1);

			/*!
				\brief The private method reads input stream and returns recognized tokens.

				\param[in] inputStream An input characters sequence.

				\return A recognized token.
			*/

			CToken* _scanToken(const std::string& stream);

			CToken* _scanNextToken();

			CToken* _tryRecognizeKeywordOrIdentifier(C8 currCh);

			CToken* _tryRecognizeLiteral(C8 currCh);

			CToken* _tryRecognizeNumberLiteral(C8 currCh);

			CToken* _tryRecognizeStringOrCharLiteral(C8 currCh);
			
			void _streamInputData(std::string& currStreamBuffer, IInputStream* pInputStream);

			void _pushNewLine(U32& currPos, U32& currLine);
		private:
			static TReservedTokensTable mReservedTokensMap;

			IInputStream*               mpInputStream;

			U32                         mCurrPos;

			U32                         mCurrLine;

			std::string                 mCurrStreamBuffer; ///< The member contains current read line from the input stream

			CToken*                     mpLastRecognizedToken;

			std::vector<CToken*>        mpTokens;

			TReadTokensQueue            mpPeekTokensBuffer;
	};
}

#endif
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
#include <map>


namespace gplc
{

	/*!
		\brief ILexer interface

		All lexers should implement its methods. The first goal of using it is unit testing.
	*/
	
	class ILexer
	{
		public:
			ILexer() {}
			virtual ~ILexer() {}

			/*!
				\brief The function reads input stream of characters and prepares a tokens' sequence.

				\param[in] inputStream An input characters sequence.
				\param[in] configFilename A name of file, which stores reserved keywords' declarations.

				\return A function's result code.
			*/

			virtual Result Init(const std::wstring& inputStream, const std::wstring& configFilename) = 0;

			/*!
				\brief The function clears the current state of an object.

				The method implicitly is called from Init function of the class.

				\return A function's result code.
			*/

			virtual Result Reset() = 0;

			/*!
				\brief The function returns a current token from tokens' sequence

				A call of this method doesn't change a state of an object.

				\return A current token from tokens' sequence
			*/

			virtual const CToken* GetCurrToken() const = 0;

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

			virtual const CToken* PeekNextToken(U32 numOfSteps = 1) const = 0;

			/*!
				\brief The method saves current state of an object. It's used for backtracking.
			*/
			
			virtual void SavePosition() = 0;

			/*!
				\brief The method restores previous state of an object. It's used for backtracking.
			*/
			
			virtual void RestorePosition() = 0;
		public:
			CDelegate<void, const TLexerErrorInfo&> OnErrorOutput;
		protected:
			ILexer(const ILexer& lexer) {}
	};


	/*!
		\brief CLexer class

		The class provides methods for tokens' recognition. All allowable tokens' types are placed in gplcTokens.h.
		All reserved keywords are placed in a special file .tokens. They duplicate E_TOKEN_TYPE's values.
	*/

	class CLexer : public ILexer
	{
		public:
			/*!
				\brief Default contructor of the class
			*/

			CLexer();

			/*!
				\brief Default contructor of the class
			*/

			virtual ~CLexer();

			/*!
				\brief The function reads input stream of characters and prepares a tokens' sequence.

				\param[in] inputStream An input characters sequence.
				\param[in] configFilename A name of file, which stores reserved keywords' declarations.

				\return A function's result code.
			*/

			virtual Result Init(const std::wstring& inputStream, const std::wstring& configFilename);

			/*!
				\brief The function clears the current state of an object.

				The method implicitly is called from Init function of the class.

				\return A function's result code.
			*/

			virtual Result Reset();

			/*!
				\brief The function returns a current token from tokens' sequence

				A call of this method doesn't change a state of an object.

				\return A current token from tokens' sequence
			*/

			virtual const CToken* GetCurrToken() const;

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

			virtual const CToken* PeekNextToken(U32 numOfSteps = 1) const;

			/*!
				\brief The method saves current state of an object. It's used for backtracking.
			*/

			virtual void SavePosition();

			/*!
				\brief The method restores previous state of an object. It's used for backtracking.
			*/

			virtual void RestorePosition();
		private:
			/*!
				\brief Copy contructor
				
				\param[in] lexer A reference to CLexer object-initializer

				It's don't used. Therefore it's marked as private.
			*/

			CLexer(const CLexer& lexer);

			/*!
				\brief The private method returns the current character.

				\param[in] stream An input characters sequence.

				\return A wide character at current position of the inner pointer.
			*/

			W16 _getCurrChar(const std::wstring& stream) const;

			/*!
				\brief The private method returns the next character.

				\param[in] stream An input characters sequence.

				\return A wide character after current position of the inner pointer.
			*/

			W16 _getNextChar(const std::wstring& stream);
			
			/*!
				\brief The private method returns a character, which is placed after current one with specified offset.

				\param[in] stream An input characters sequence.
				\param[in] offset An offset from the current position in the characters' sequence.

				\return A wide character at specified position.
			*/

			W16 _peekNextChar(const std::wstring& stream, U32 offset = 1) const;

			/*!
				\brief The private method reads input stream and returns recognized tokens.

				\param[in] inputStream An input characters sequence.

				\return A recognized token.
			*/

			CToken* _scanToken(const std::wstring& stream);

			/*!
				\brief The private method reads a specified file with tokens and contructs a map with following structure map<std::wstring, E_TOKEN_TYPE>.

				\param[in] filename A filename with tokens.
				\param[out] result It stores a result of method's execution.

				\return A map of std::wstring to E_TOKEN_TYPE.
			*/

			std::map<std::wstring, E_TOKEN_TYPE> _readTokensMapFromFile(const std::wstring& filename, Result& result);
		private:
			U32                                  mCurrPos;
			U32                                  mCurrLine;
			U32                                  mCurrTokenIndex;

			U32                                  mSavedTokenIndex;

			std::map<std::wstring, E_TOKEN_TYPE> mReservedTokensMap;
			std::vector<CToken*>                 mTokens;
	};
}

#endif
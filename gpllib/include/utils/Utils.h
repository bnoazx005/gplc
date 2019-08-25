/*!
	\author Ildar Kasimov
	\date   07.08.2019
	\copyright

	\brief

	\todo
*/

#ifndef GPLC_UTILS_H
#define GPLC_UTILS_H


#include "common/gplcTypes.h"
#include <string>
#include <iostream>


namespace gplc
{
	class CMessageOutputUtils
	{
		public:
			static std::string MessageTypeToString(E_MESSAGE_TYPE messageType);

			static std::string LexerMessageToString(E_LEXER_ERRORS message);

			static std::string ParserMessageToString(const TParserErrorInfo& info);

			static std::string SemanticAnalyserMessageToString(E_SEMANTIC_ANALYSER_MESSAGE message);
	};


	/*!
		\brief The method computes 32 bits hash based on an input string's value.
		The underlying algorithm's description can be found here
		http://www.cse.yorku.ca/~oz/hash.html

		\param[in] pStr An input string

		\param[in] hash The argument is used to store current hash value during a recursion

		\return 32 bits hash of the input string
	*/

	constexpr U32 ComputeHash(const C8* pStr, U32 hash = 5381)
	{
		return (*pStr != 0) ? ComputeHash(pStr + 1, ((hash << 5) + hash) + *pStr) : hash;
	}


	#define UNIMPLEMENTED() \
			 do { \
				std::cerr << "The feature is not implemented yet (" << __FILE__ << "; " << __LINE__ << ")\n"; \
				abort(); \
			 } while (0)

	#define UNREACHABLE() \
			do { \
				std::cerr << "The program has reached unreachable section, possibly a bug (" << __FILE__ << "; " << __LINE__ << ")\n"; \
				abort(); \
			} while (0)
}

#endif
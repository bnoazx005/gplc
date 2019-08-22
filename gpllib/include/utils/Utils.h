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
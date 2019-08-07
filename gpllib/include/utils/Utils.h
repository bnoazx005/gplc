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


namespace gplc
{
	class CMessageOutputUtils
	{
		public:
			static std::string MessageTypeToString(E_MESSAGE_TYPE messageType);

			static std::string LexerMessageToString(E_LEXER_ERRORS message);

			static std::string SemanticAnalyserMessageToString(E_SEMANTIC_ANALYSER_MESSAGE message);
	};
}

#endif
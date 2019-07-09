/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief Main header file of the compiler's project
*/

#ifndef GPLC_H
#define GPLC_H

/// Lexer's files group
#include "lexer/gplcLexer.h"
#include "lexer/gplcTokens.h"

/// Common data structures and utilities
#include "common/gplcTypes.h"
#include "common/gplcConstants.h"
#include "common/gplcSymTable.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcLiterals.h"
#include "common/gplcVisitor.h"

/// Parser's files group
#include "parser/gplcParser.h"
#include "parser/gplcASTNodes.h"

#include "utils/CASTLispyPrinter.h"
#include "utils/CResult.h"

#endif
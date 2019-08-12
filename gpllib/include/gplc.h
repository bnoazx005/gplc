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
#include "lexer/gplcInputStream.h"

/// Common data structures and utilities
#include "common/gplcTypes.h"
#include "common/gplcConstants.h"
#include "common/gplcSymTable.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcValues.h"
#include "common/gplcVisitor.h"
#include "common/gplcConstExprInterpreter.h"
#include "common/gplcModuleResolver.h"
#include "common/gplcTypesFactory.h"

/// Parser's files group
#include "parser/gplcParser.h"
#include "parser/gplcASTNodes.h"
#include "parser/gplcSemanticAnalyser.h"
#include "parser/gplcASTNodesFactory.h"

#include "utils/CASTLispyPrinter.h"
#include "utils/CResult.h"
#include "utils/Utils.h"

#include "codegen/gplcCodegen.h"
#include "codegen/gplcLLVMCodegen.h"
#include "codegen/gplcLLVMLiteralVisitor.h"
#include "codegen/gplcLLVMTypeVisitor.h"
#include "codegen/ctplr/gplcCCodegen.h"
#include "codegen/ctplr/gplcCTypeVisitor.h"
#include "codegen/ctplr/gplcCLiteralVisitor.h"

#endif
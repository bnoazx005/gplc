/*!
	\author Ildar Kasimov
	\date   08.07.2019
	\copyright

	\brief The file contains definitions of a pretty printer of AST which
	looks like LISP

	\todo
*/

#ifndef GPLC_AST_LISPY_PRINTER_H
#define GPLC_AST_LISPY_PRINTER_H


#include "common/gplcVisitor.h"
#include <string>


namespace gplc
{
	class CASTLispyPrinter : public IVisitor<std::string>
	{
		public:
			CASTLispyPrinter() = default;
			virtual ~CASTLispyPrinter() = default;
			
			std::string Print(CASTNode* pSourceUnitNode);

			std::string VisitProgramUnit(CASTNode* pProgramNode) override;

			std::string VisitIdentifier(CASTIdentifierNode* pNode) override;

			std::string VisitLiteral(CASTLiteralNode* pNode) override;

			std::string VisitUnaryExpression(CASTUnaryExpressionNode* pNode) override;

			std::string VisitBinaryExpression(CASTBinaryExpressionNode* pNode) override;

			std::string VisitAssignment(CASTAssignmentNode* pNode) override;
		protected:
			CASTLispyPrinter(const CASTLispyPrinter& printer) = default;
		protected:
	};
}

#endif
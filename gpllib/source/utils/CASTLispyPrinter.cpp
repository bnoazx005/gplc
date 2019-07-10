#include "utils/CASTLispyPrinter.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcLiterals.h"


namespace gplc
{
	std::string CASTLispyPrinter::Print(CASTNode* pSourceUnitNode)
	{
		if (pSourceUnitNode == nullptr)
		{
			return "";
		}

		return pSourceUnitNode->Accept(this);
	}

	std::string CASTLispyPrinter::VisitProgramUnit(CASTNode* pProgramNode)
	{
		return "()";
	}

	std::string CASTLispyPrinter::VisitIdentifier(CASTIdentifierNode* pNode) 
	{
		if (pNode == nullptr)
		{
			return {};
		}

		return pNode->GetName();
	}

	std::string CASTLispyPrinter::VisitLiteral(CASTLiteralNode* pNode) 
	{
		if (pNode == nullptr)
		{
			return {};
		}

		return pNode->GetValue()->ToString();
	}

	std::string CASTLispyPrinter::VisitUnaryExpression(CASTUnaryExpressionNode* pNode) 
	{
		return pNode->GetData()->Accept(this);
	}

	std::string CASTLispyPrinter::VisitBinaryExpression(CASTBinaryExpressionNode* pNode) 
	{
		std::string result("(");

		switch (pNode->GetOpType())
		{
			case TT_PLUS:
				result.append("+ ");
				break;
			case TT_MINUS:
				result.append("- ");
				break;
			case TT_STAR:
				result.append("* ");
				break;
			case TT_SLASH:
				result.append("/ ");
				break;
		}

		return result.append(pNode->GetLeft()->Accept(this))
					 .append(" ")
				     .append(pNode->GetRight()->Accept(this))
					 .append(")");
	}

	std::string CASTLispyPrinter::VisitAssignment(CASTAssignmentNode* pNode) 
	{
		std::string leftStr  = pNode->GetLeft()->Accept(this);
		std::string rightStr = pNode->GetRight()->Accept(this);

		return "(set! " + leftStr + " " + rightStr + ")";
	}

	std::string CASTLispyPrinter::VisitStatementsBlock(CASTBlockNode* pNode)
	{
		auto children = pNode->GetChildren();

		std::string output { "(" };

		for (auto iter : children)
		{
			output.append(iter->Accept(this));
		}

		return output.append(")");
	}

	std::string CASTLispyPrinter::VisitIfStatement(CASTIfStatementNode* pNode) 
	{
		std::string t = std::string("(if ")
						.append(pNode->GetCondition()->Accept(this))
						.append(" ")
						.append(pNode->GetThenBlock()->Accept(this))
						.append(" ")
						.append(pNode->GetElseBlock()->Accept(this))
						.append(")");

		return t;
	}

	std::string CASTLispyPrinter::VisitLoopStatement(CASTLoopStatementNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitFunctionClosure(CASTFunctionClosureNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitFunctionArgs(CASTFunctionArgsNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitFunctionCall(CASTFunctionCallNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitReturnStatement(CASTReturnStatementNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitDefinitionNode(CASTDefinitionNode* pNode) 
	{
		return {};
	}

	std::string CASTLispyPrinter::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) 
	{
		return {};
	}
}
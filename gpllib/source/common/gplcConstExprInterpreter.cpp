#include "common/gplcConstExprInterpreter.h"
#include "common/gplcSymTable.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcValues.h"


namespace gplc
{
	TResult<U32> CConstExprInterpreter::Eval(CASTExpressionNode* pExpr, ISymTable* pSymTable)
	{
		if (!pExpr)
		{
			return TOkValue(std::numeric_limits<U32>::max());
		}

		mpSymTable = pSymTable;

		U32 result = std::get<U32>(pExpr->Accept(this));

		if (result == std::numeric_limits<U32>::max())
		{
			return TErrorValue(RV_FAIL);
		}

		return TOkValue(result);
	}

	TLLVMIRData CConstExprInterpreter::VisitProgramUnit(CASTSourceUnitNode* pProgramNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitDeclaration(CASTDeclarationNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitIdentifier(CASTIdentifierNode* pNode) 
	{
		auto pSymbolDesc = mpSymTable->LookUp(pNode->GetName());
		
		return pSymbolDesc->mpValue->Accept(this);
	}

	TLLVMIRData CConstExprInterpreter::VisitLiteral(CASTLiteralNode* pNode) 
	{
		CBaseValue* pValue = pNode->GetValue();

		if (pValue->GetType() == LT_INT)
		{
			return dynamic_cast<CIntValue*>(pValue)->GetValue();
		}

		if (pValue->GetType() == LT_UINT)
		{
			return dynamic_cast<CUIntValue*>(pValue)->GetValue();
		}

		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitUnaryExpression(CASTUnaryExpressionNode* pNode) 
	{
		U32 result = std::get<U32>(pNode->GetData()->Accept(this));

		return (pNode->GetOpType() == TT_MINUS) ? -result : result;
	}

	TLLVMIRData CConstExprInterpreter::VisitBinaryExpression(CASTBinaryExpressionNode* pNode) 
	{
		U32 leftExpr  = std::get<U32>(pNode->GetLeft()->Accept(this));
		U32 rightExpr = std::get<U32>(pNode->GetRight()->Accept(this));

		switch (pNode->GetOpType())
		{
			case TT_PLUS:
				return leftExpr + rightExpr;

			case TT_MINUS:
				return leftExpr - rightExpr;

			case TT_STAR:
				return leftExpr * rightExpr;

			case TT_SLASH:
				return leftExpr / rightExpr;
		}

		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitAssignment(CASTAssignmentNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitStatementsBlock(CASTBlockNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitIfStatement(CASTIfStatementNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitLoopStatement(CASTLoopStatementNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitFunctionClosure(CASTFunctionClosureNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitFunctionArgs(CASTFunctionArgsNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitFunctionCall(CASTFunctionCallNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitReturnStatement(CASTReturnStatementNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitDefinitionNode(CASTDefinitionNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitEnumDeclaration(CASTEnumDeclNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitStructDeclaration(CASTStructDeclNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitBreakOperator(CASTBreakOperatorNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitContinueOperator(CASTContinueOperatorNode* pNode) 
	{
		return std::numeric_limits<U32>::max();
	}

	TLLVMIRData CConstExprInterpreter::VisitAccessOperator(CASTAccessOperatorNode* pNode) 
	{
		return {};
	}

	TLLVMIRData CConstExprInterpreter::VisitArrayTypeNode(CASTArrayTypeNode* pNode)
	{
		return {};
	}

	TLLVMIRData CConstExprInterpreter::VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode)
	{
		return {};
	}

	TLLVMIRData CConstExprInterpreter::VisitPointerTypeNode(CASTPointerTypeNode* pNode)
	{
		return {};
	}

	TLLVMIRData CConstExprInterpreter::VisitImportDirectiveNode(CASTImportDirectiveNode* pNode)
	{
		return {};
	}

	TLLVMIRData CConstExprInterpreter::VisitDeferOperatorNode(CASTDeferOperatorNode* pNode)
	{
		return {};
	}
}
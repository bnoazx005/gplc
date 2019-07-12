#include "parser/gplcSemanticAnalyser.h"
#include "common/gplcSymTable.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcTypeSystem.h"


namespace gplc
{
	bool CSemanticAnalyser::Analyze(CASTNode* pInput, ITypeResolver* pTypeResolver, ISymTable* pSymTable)
	{
		if (!pSymTable || !pInput || !pTypeResolver)
		{
			return false;
		}

		mpTypeResolver = pTypeResolver;
		mpSymTable     = pSymTable;

		return pInput->Accept(this);
	}

	bool CSemanticAnalyser::VisitProgramUnit(CASTSourceUnitNode* pProgramNode) 
	{
		if (pProgramNode->GetChildrenCount() < 1)
		{
			return true;
		}

		auto pStatements = pProgramNode->GetStatements();

		for (auto pCurrStatement : pStatements)
		{
			if (!pCurrStatement->Accept(this))
			{
				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitDeclaration(CASTDeclarationNode* pNode)
	{
		CASTNode* pIdentifiersList = pNode->GetIdentifiers();

		CType* pTypeInfo = mpTypeResolver->Resolve(pNode->GetTypeInfo(), mpSymTable); // visit node and deduce the type's info

		CASTIdentifierNode* pCurrIdentifierNode = nullptr;

		for (CASTNode* pCurrChild : pIdentifiersList->GetChildren())
		{
			pCurrIdentifierNode = dynamic_cast<CASTIdentifierNode*>(pCurrChild);
			
			if (!SUCCESS(mpSymTable->AddVariable(pCurrIdentifierNode->GetName(), { pTypeInfo->GetDefaultValue(), pTypeInfo })))
			{
				OnErrorOutput.Invoke(SAE_IDENTIFIER_ALREADY_DECLARED);

				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitIdentifier(CASTIdentifierNode* pNode) 
	{
		if (!mpSymTable->LookUp(pNode->GetName()))
		{
			OnErrorOutput.Invoke(SAE_UNDECLARED_IDENTIFIER);

			return false;
		}

		return true;
	}

	bool CSemanticAnalyser::VisitLiteral(CASTLiteralNode* pNode) 
	{
		return true; // for now all literals are assumed as valid 
	}

	bool CSemanticAnalyser::VisitUnaryExpression(CASTUnaryExpressionNode* pNode) 
	{
		return pNode->GetData()->Accept(this);
	}

	bool CSemanticAnalyser::VisitBinaryExpression(CASTBinaryExpressionNode* pNode) 
	{
		CASTExpressionNode* pLeftExpr  = pNode->GetLeft();
		CASTExpressionNode* pRightExpr = pNode->GetRight();

		// check left expression
		CType* pLeftValueType = nullptr;

		if (!pLeftExpr->Accept(this) ||
			!(pLeftValueType = pLeftExpr->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		// check right expression
		CType* pRightValueType = nullptr;

		if (!pRightExpr->Accept(this) ||
			!(pRightValueType = pRightExpr->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		// \todo check whether an operator is implemented for both these types

		return pLeftValueType->AreSame(pRightValueType);
	}

	bool CSemanticAnalyser::VisitAssignment(CASTAssignmentNode* pNode) 
	{
		CASTExpressionNode* pLeftExpr  = pNode->GetLeft();
		CASTExpressionNode* pRightExpr = pNode->GetRight();

		// check left side
		CType* pLeftValueType = nullptr;

		if (!pLeftExpr->Accept(this) ||
			!(pLeftValueType = pLeftExpr->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		// check right side
		CType* pRightValueType = nullptr;

		if (!pRightExpr->Accept(this) ||
			!(pRightValueType = pRightExpr->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		// check their compatibility
		return pLeftValueType->AreSame(pRightValueType);
	}

	bool CSemanticAnalyser::VisitStatementsBlock(CASTBlockNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitIfStatement(CASTIfStatementNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitLoopStatement(CASTLoopStatementNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitFunctionClosure(CASTFunctionClosureNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitFunctionArgs(CASTFunctionArgsNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitFunctionCall(CASTFunctionCallNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitReturnStatement(CASTReturnStatementNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitDefinitionNode(CASTDefinitionNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) 
	{
		return false;
	}
}
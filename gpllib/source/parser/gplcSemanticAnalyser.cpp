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
		auto pStatements = pNode->GetStatements();

		for (auto pCurrStatement : pStatements)
		{
			if (!pCurrStatement->Accept(this))
			{
				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitIfStatement(CASTIfStatementNode* pNode) 
	{
		auto pCondition = pNode->GetCondition();
		auto pThenBlock = pNode->GetThenBlock();
		auto pElseBlock = pNode->GetElseBlock();

		// check condition
		CType* pConditionType = nullptr;

		if (!pCondition->Accept(this) || !(pConditionType = pCondition->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		if (pConditionType->GetType() != CT_BOOL)
		{
			OnErrorOutput.Invoke(SAE_LOGIC_EXPR_IS_EXPECTED);

			return false;
		}

		if (!pThenBlock->Accept(this) || (pElseBlock && !pElseBlock->Accept(this)))
		{
			return false;
		}

		return true;
	}

	bool CSemanticAnalyser::VisitLoopStatement(CASTLoopStatementNode* pNode) 
	{
		if (pNode->GetBody()->GetChildrenCount() < 1)
		{
			OnErrorOutput.Invoke(SAE_REDUNDANT_LOOP_STATEMENT);
		}

		return pNode->GetBody()->Accept(this);
	}

	bool CSemanticAnalyser::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) 
	{
		if (pNode->GetBody()->GetChildrenCount() < 1)
		{
			OnErrorOutput.Invoke(SAE_REDUNDANT_LOOP_STATEMENT);
		}

		auto pCondition = pNode->GetCondition();
		auto pLoopBody  = pNode->GetBody();

		// check condition
		CType* pConditionType = nullptr;

		if (!pCondition->Accept(this) || !(pConditionType = pCondition->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		if (pConditionType->GetType() != CT_BOOL)
		{
			OnErrorOutput.Invoke(SAE_LOGIC_EXPR_IS_EXPECTED);

			return false;
		}

		return pLoopBody->Accept(this);
	}

	bool CSemanticAnalyser::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) 
	{
		auto pClosureDecl     = pNode->GetClosure();
		auto pArgs            = pNode->GetArgs();
		auto pReturnValueType = pNode->GetReturnValueType();
		
		return pClosureDecl->Accept(this) && 
			   pArgs->Accept(this) && 
			   pReturnValueType->Accept(this);
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
		return true;
	}

	bool CSemanticAnalyser::VisitDefinitionNode(CASTDefinitionNode* pNode) 
	{
		auto pDeclNode  = pNode->GetDeclaration();
		auto pValueNode = dynamic_cast<CASTExpressionNode*>(pNode->GetValue());

		// check a declaration
		CType* pDeclType = nullptr;

		if (!pDeclNode->Accept(this) || !(pDeclType = pDeclNode->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		// check value
		CType* pValueType = nullptr;

		if (!pValueNode->Accept(this) || !(pValueType = pValueNode->Resolve(mpTypeResolver, mpSymTable)))
		{
			return false;
		}

		// check their compatibility

		return pDeclType->AreSame(pValueType);
	}

	bool CSemanticAnalyser::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) 
	{
		return false;
	}
}
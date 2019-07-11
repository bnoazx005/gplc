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

	bool CSemanticAnalyser::VisitProgramUnit(CASTNode* pProgramNode) 
	{
		return false;
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
		return false;
	}

	bool CSemanticAnalyser::VisitLiteral(CASTLiteralNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitUnaryExpression(CASTUnaryExpressionNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitBinaryExpression(CASTBinaryExpressionNode* pNode) 
	{
		return false;
	}

	bool CSemanticAnalyser::VisitAssignment(CASTAssignmentNode* pNode) 
	{
		return false;
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
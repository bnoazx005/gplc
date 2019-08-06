#include "parser/gplcASTNodesFactory.h"


namespace gplc
{
	CASTNodesFactory::CASTNodesFactory()
	{
	}

	CASTNodesFactory::~CASTNodesFactory()
	{
		CASTNode* pCurrNode = nullptr;

		while (!mpCachedNodes.empty())
		{
			pCurrNode = mpCachedNodes.front();

			if (pCurrNode)
			{
				delete pCurrNode;

				pCurrNode = nullptr;
			}

			mpCachedNodes.erase(mpCachedNodes.begin());
		}
	}

	CASTNode* CASTNodesFactory::CreateNode(E_NODE_TYPE type)
	{
		return _insertNode(new CASTNode(type));
	}

	CASTSourceUnitNode* CASTNodesFactory::CreateSourceUnitNode()
	{
		return _insertNode(new CASTSourceUnitNode());
	}

	CASTTypeNode* CASTNodesFactory::CreateTypeNode(E_NODE_TYPE type)
	{
		return _insertNode(new CASTTypeNode(type));
	}

	CASTDeclarationNode* CASTNodesFactory::CreateDeclNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo, U32 attributes)
	{
		return _insertNode(new CASTDeclarationNode(pIdentifiers, pTypeInfo, attributes));
	}

	CASTBlockNode* CASTNodesFactory::CreateBlockNode()
	{
		return _insertNode(new CASTBlockNode());
	}

	CASTIdentifierNode* CASTNodesFactory::CreateIdNode(const std::string& name, U32 attributes)
	{
		return _insertNode(new CASTIdentifierNode(name, attributes));
	}

	CASTLiteralNode* CASTNodesFactory::CreateLiteralNode(CBaseValue* pValue)
	{
		return _insertNode(new CASTLiteralNode(pValue));
	}

	CASTUnaryExpressionNode* CASTNodesFactory::CreateUnaryExpr(E_TOKEN_TYPE opType, CASTNode* pNode)
	{
		return _insertNode(new CASTUnaryExpressionNode(opType, pNode));
	}

	CASTBinaryExpressionNode* CASTNodesFactory::CreateBinaryExpr(CASTExpressionNode* pLeft, E_TOKEN_TYPE opType, CASTExpressionNode* pRight)
	{
		return _insertNode(new CASTBinaryExpressionNode(pLeft, opType, pRight));
	}

	CASTAssignmentNode* CASTNodesFactory::CreateAssignNode(CASTExpressionNode* pLeft, CASTExpressionNode* pRight)
	{
		return _insertNode(new CASTAssignmentNode(pLeft, pRight));
	}

	CASTIfStatementNode* CASTNodesFactory::CreateIfStmtNode(CASTExpressionNode* pCondition, CASTBlockNode* pThenBlock, CASTBlockNode* pElseBlock)
	{
		return _insertNode(new CASTIfStatementNode(pCondition, pThenBlock, pElseBlock));
	}

	CASTLoopStatementNode* CASTNodesFactory::CreateLoopStmtNode(CASTBlockNode* pBody)
	{
		return _insertNode(new CASTLoopStatementNode(pBody));
	}

	CASTWhileLoopStatementNode* CASTNodesFactory::CreateWhileStmtNode(CASTExpressionNode* pCondition, CASTBlockNode* pBody)
	{
		return _insertNode(new CASTWhileLoopStatementNode(pCondition, pBody));
	}

	CASTFunctionClosureNode* CASTNodesFactory::CreateFuncClosureNode()
	{
		return _insertNode(new CASTFunctionClosureNode());
	}

	CASTFunctionArgsNode* CASTNodesFactory::CreateFuncArgsNode()
	{
		return _insertNode(new CASTFunctionArgsNode());
	}

	CASTFunctionDeclNode* CASTNodesFactory::CreateFuncDeclNode(CASTFunctionClosureNode* pClosure, CASTFunctionArgsNode* pArgs, CASTNode* pReturnValue)
	{
		return _insertNode(new CASTFunctionDeclNode(pClosure, pArgs, pReturnValue));
	}

	CASTFunctionCallNode* CASTNodesFactory::CreateFuncCallNode(CASTUnaryExpressionNode* pIdentifier, CASTNode* pArgsList)
	{
		return _insertNode(new CASTFunctionCallNode(pIdentifier, pArgsList));
	}

	CASTReturnStatementNode* CASTNodesFactory::CreateReturnStmtNode(CASTExpressionNode* pExpression)
	{
		return _insertNode(new CASTReturnStatementNode(pExpression));
	}

	CASTDefinitionNode* CASTNodesFactory::CreateDefNode(CASTDeclarationNode* pDecl, CASTNode* pValue)
	{
		return _insertNode(new CASTDefinitionNode(pDecl, pValue));
	}

	CASTFuncDefinitionNode* CASTNodesFactory::CreateFuncDefNode(CASTDeclarationNode* pDecl, CASTFunctionDeclNode* pLambdaType, CASTNode* pBody)
	{
		return _insertNode(new CASTFuncDefinitionNode(pDecl, pLambdaType, pBody));
	}

	CASTEnumDeclNode* CASTNodesFactory::CreateEnumDeclNode(CASTIdentifierNode* pEnumName)
	{
		return _insertNode(new CASTEnumDeclNode(pEnumName));
	}

	CASTStructDeclNode* CASTNodesFactory::CreateStructDeclNode(CASTIdentifierNode* pStructName, CASTBlockNode* pStructFields)
	{
		return _insertNode(new CASTStructDeclNode(pStructName, pStructFields));
	}

	CASTNamedTypeNode* CASTNodesFactory::CreateNamedTypeNode(CASTIdentifierNode* pIdentifier)
	{
		return _insertNode(new CASTNamedTypeNode(pIdentifier));
	}

	CASTBreakOperatorNode* CASTNodesFactory::CreateBreakNode()
	{
		return _insertNode(new CASTBreakOperatorNode());
	}

	CASTContinueOperatorNode* CASTNodesFactory::CreateContinueNode()
	{
		return _insertNode(new CASTContinueOperatorNode());
	}

	CASTAccessOperatorNode* CASTNodesFactory::CreateAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pMemberName)
	{
		return _insertNode(new CASTAccessOperatorNode(pExpression, pMemberName));
	}

	CASTArrayTypeNode* CASTNodesFactory::CreateArrayTypeNode(CASTNode* pTypeInfo, CASTExpressionNode* pSizeExpr)
	{
		return _insertNode(new CASTArrayTypeNode(pTypeInfo, pSizeExpr));
	}

	CASTIndexedAccessOperatorNode* CASTNodesFactory::CreateIndexedAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pIndexExpr, U32 attributes)
	{
		return _insertNode(new CASTIndexedAccessOperatorNode(pExpression, pIndexExpr, attributes));
	}

	CASTPointerTypeNode* CASTNodesFactory::CreatePointerTypeNode(CASTNode* pTypeInfo)
	{
		return _insertNode(new CASTPointerTypeNode(pTypeInfo));
	}
}
/*!
	\author Ildar Kasimov
	\date   05.08.2019
	\copyright

	\brief The file contains definition of AST nodes' factory

	\todo
*/

#ifndef GPLC_AST_NODES_FACTORY_H
#define GPLC_AST_NODES_FACTORY_H


#include <vector>
#include <string>
#include "gplcASTNodes.h"


namespace gplc
{
	class CBaseValue;


	class IASTNodesFactory
	{
		public:
			IASTNodesFactory() = default;
			virtual ~IASTNodesFactory() = default;

			virtual CASTNode* CreateNode(E_NODE_TYPE type) = 0;
			virtual CASTSourceUnitNode* CreateSourceUnitNode(const std::string& moduleName = "") = 0;
			virtual CASTTypeNode* CreateTypeNode(E_NODE_TYPE type, U32 attributes = 0x0) = 0;
			virtual CASTDeclarationNode* CreateDeclNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo, U32 attributes = 0x0) = 0;
			virtual CASTBlockNode* CreateBlockNode() = 0;
			virtual CASTIdentifierNode* CreateIdNode(const std::string& name, U32 attributes = 0x0) = 0;
			virtual CASTLiteralNode* CreateLiteralNode(CBaseValue* pValue) = 0;
			virtual CASTUnaryExpressionNode* CreateUnaryExpr(E_TOKEN_TYPE opType, CASTNode* pNode) = 0;
			virtual CASTBinaryExpressionNode* CreateBinaryExpr(CASTExpressionNode* pLeft, E_TOKEN_TYPE opType, CASTExpressionNode* pRight) = 0;
			virtual CASTAssignmentNode* CreateAssignNode(CASTExpressionNode* pLeft, CASTExpressionNode* pRight) = 0;
			virtual CASTIfStatementNode* CreateIfStmtNode(CASTExpressionNode* pCondition, CASTBlockNode* pThenBlock, CASTBlockNode* pElseBlock) = 0;
			virtual CASTLoopStatementNode* CreateLoopStmtNode(CASTBlockNode* pBody) = 0;
			virtual CASTWhileLoopStatementNode* CreateWhileStmtNode(CASTExpressionNode* pCondition, CASTBlockNode* pBody) = 0;
			virtual CASTFunctionClosureNode* CreateFuncClosureNode() = 0;
			virtual CASTFunctionArgsNode* CreateFuncArgsNode() = 0;
			virtual CASTFunctionDeclNode* CreateFuncDeclNode(CASTFunctionClosureNode* pClosure, CASTFunctionArgsNode* pArgs, CASTNode* pReturnValue, U32 attributes = 0x0) = 0;
			virtual CASTFunctionCallNode* CreateFuncCallNode(CASTUnaryExpressionNode* pIdentifier, CASTNode* pArgsList) = 0;
			virtual CASTReturnStatementNode* CreateReturnStmtNode(CASTExpressionNode* pExpression) = 0;
			virtual CASTDefinitionNode* CreateDefNode(CASTDeclarationNode* pDecl, CASTNode* pValue) = 0;
			virtual CASTFuncDefinitionNode* CreateFuncDefNode(CASTDeclarationNode* pDecl, CASTFunctionDeclNode* pLambdaType, CASTNode* pBody) = 0;
			virtual CASTEnumDeclNode* CreateEnumDeclNode(CASTIdentifierNode* pEnumName) = 0;
			virtual CASTStructDeclNode* CreateStructDeclNode(CASTIdentifierNode* pStructName, CASTBlockNode* pStructFields) = 0;
			virtual CASTNamedTypeNode* CreateNamedTypeNode(CASTIdentifierNode* pIdentifier, U32 attributes = 0x0) = 0;
			virtual CASTBreakOperatorNode* CreateBreakNode() = 0;
			virtual CASTContinueOperatorNode* CreateContinueNode() = 0;
			virtual CASTAccessOperatorNode* CreateAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pMemberName) = 0;
			virtual CASTArrayTypeNode* CreateArrayTypeNode(CASTNode* pTypeInfo, CASTExpressionNode* pSizeExpr) = 0;
			virtual CASTIndexedAccessOperatorNode* CreateIndexedAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pIndexExpr, U32 attributes) = 0;
			virtual CASTPointerTypeNode* CreatePointerTypeNode(CASTNode* pTypeInfo) = 0;
			virtual CASTImportDirectiveNode* CreateImportDirective(const std::string& modulePath, const std::string& moduleName) = 0;
			virtual CASTDeferOperatorNode* CreateDeferOperator(CASTExpressionNode* pExpression) = 0;
			virtual CASTIntrinsicCallNode* CreateIntrinsicCall(E_NODE_TYPE intrinsicType, CASTNode* pArgsList) = 0;
			virtual CASTVariantDeclNode* CreateVariantDeclNode(CASTIdentifierNode* pVariantName, CASTBlockNode* pVariantTypes) = 0;
		protected:
			IASTNodesFactory(const IASTNodesFactory&) = default;
	};


	class CASTNodesFactory : public IASTNodesFactory
	{
		public:
			CASTNodesFactory();
			virtual ~CASTNodesFactory();

			CASTNode* CreateNode(E_NODE_TYPE type) override;
			CASTSourceUnitNode* CreateSourceUnitNode(const std::string& moduleName = "") override;
			CASTTypeNode* CreateTypeNode(E_NODE_TYPE type, U32 attributes = 0x0) override;
			CASTDeclarationNode* CreateDeclNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo, U32 attributes = 0x0) override;
			CASTBlockNode* CreateBlockNode() override;
			CASTIdentifierNode* CreateIdNode(const std::string& name, U32 attributes = 0x0) override;
			CASTLiteralNode* CreateLiteralNode(CBaseValue* pValue) override;
			CASTUnaryExpressionNode* CreateUnaryExpr(E_TOKEN_TYPE opType, CASTNode* pNode) override;
			CASTBinaryExpressionNode* CreateBinaryExpr(CASTExpressionNode* pLeft, E_TOKEN_TYPE opType, CASTExpressionNode* pRight) override;
			CASTAssignmentNode* CreateAssignNode(CASTExpressionNode* pLeft, CASTExpressionNode* pRight) override;
			CASTIfStatementNode* CreateIfStmtNode(CASTExpressionNode* pCondition, CASTBlockNode* pThenBlock, CASTBlockNode* pElseBlock) override;
			CASTLoopStatementNode* CreateLoopStmtNode(CASTBlockNode* pBody) override;
			CASTWhileLoopStatementNode* CreateWhileStmtNode(CASTExpressionNode* pCondition, CASTBlockNode* pBody) override;
			CASTFunctionClosureNode* CreateFuncClosureNode() override;
			CASTFunctionArgsNode* CreateFuncArgsNode() override;
			CASTFunctionDeclNode* CreateFuncDeclNode(CASTFunctionClosureNode* pClosure, CASTFunctionArgsNode* pArgs, CASTNode* pReturnValue, U32 attributes = 0x0) override;
			CASTFunctionCallNode* CreateFuncCallNode(CASTUnaryExpressionNode* pIdentifier, CASTNode* pArgsList) override;
			CASTReturnStatementNode* CreateReturnStmtNode(CASTExpressionNode* pExpression) override;
			CASTDefinitionNode* CreateDefNode(CASTDeclarationNode* pDecl, CASTNode* pValue) override;
			CASTFuncDefinitionNode* CreateFuncDefNode(CASTDeclarationNode* pDecl, CASTFunctionDeclNode* pLambdaType, CASTNode* pBody) override;
			CASTEnumDeclNode* CreateEnumDeclNode(CASTIdentifierNode* pEnumName) override;
			CASTStructDeclNode* CreateStructDeclNode(CASTIdentifierNode* pStructName, CASTBlockNode* pStructFields) override;
			CASTNamedTypeNode* CreateNamedTypeNode(CASTIdentifierNode* pIdentifier, U32 attributes = 0x0) override;
			CASTBreakOperatorNode* CreateBreakNode() override;
			CASTContinueOperatorNode* CreateContinueNode() override;
			CASTAccessOperatorNode* CreateAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pMemberName) override;
			CASTArrayTypeNode* CreateArrayTypeNode(CASTNode* pTypeInfo, CASTExpressionNode* pSizeExpr) override;
			CASTIndexedAccessOperatorNode* CreateIndexedAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pIndexExpr, U32 attributes) override;
			CASTPointerTypeNode* CreatePointerTypeNode(CASTNode* pTypeInfo) override;
			CASTImportDirectiveNode* CreateImportDirective(const std::string& modulePath, const std::string& moduleName) override;
			CASTDeferOperatorNode* CreateDeferOperator(CASTExpressionNode* pExpression) override;
			CASTIntrinsicCallNode* CreateIntrinsicCall(E_NODE_TYPE intrinsicType, CASTNode* pArgsList) override;
			CASTVariantDeclNode* CreateVariantDeclNode(CASTIdentifierNode* pVariantName, CASTBlockNode* pVariantTypes) override;
		protected:
			CASTNodesFactory(const CASTNodesFactory&) = default;

			template <typename T>
			inline T* _insertNode(T* pNode)
			{
				mpCachedNodes.push_back(pNode);

				return pNode;
			}
		protected:
			std::vector<CASTNode*> mpCachedNodes;
	};
}

#endif
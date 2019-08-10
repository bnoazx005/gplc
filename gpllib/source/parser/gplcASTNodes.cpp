/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright

	\brief The file contains AST nodes defenitions

	\todo
*/

#include "parser/gplcASTNodes.h"
#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcSymTable.h"
#include <stack>


namespace gplc
{
	/*!
		CASTNode class definition
	*/

	CASTNode::CASTNode()
	{
	}

	CASTNode::CASTNode(E_NODE_TYPE type):
		mType(type)
	{
	}

	CASTNode::CASTNode(const CASTNode& node)
	{
	}
		
	std::string CASTNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return std::string();
	}

	bool CASTNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return false;
	}

	TLLVMIRData CASTNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	Result CASTNode::AttachChild(CASTNode* node)
	{
		/*if (node == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}*/

		mChildren.push_back(node);

		return RV_SUCCESS;
	}
	
	Result CASTNode::AttachChildren(const std::vector<CASTNode*>& nodes)
	{
		if (nodes.empty())
		{
			return RV_FAIL;
		}

		mChildren.insert(mChildren.end(), nodes.begin(), nodes.end());

		return RV_SUCCESS;
	}

	Result CASTNode::DettachChild(CASTNode** node)
	{
		if (node == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}

		std::vector<CASTNode*>::iterator currElement = std::find(mChildren.begin(), mChildren.end(), *node);

		if (currElement == mChildren.end())
		{
			return RV_FAIL;
		}

		mChildren.erase(currElement);

		delete *currElement;

		*currElement = nullptr;
		
		return RV_SUCCESS;
	}

	const std::vector<CASTNode*> CASTNode::GetChildren() const
	{
		return mChildren;
	}

	U32 CASTNode::GetChildrenCount() const
	{
		return mChildren.size();
	}
	
	E_NODE_TYPE CASTNode::GetType() const
	{
		return mType;
	}

	void CASTNode::_removeNode(CASTNode** node)
	{
		CASTNode* pNode = *node;

		if (pNode == nullptr)
		{
			return;
		}

		U32 childrenCount = pNode->mChildren.size();

		if (childrenCount < 1)
		{
			return;
		}

		std::stack<CASTNode*> nodesInStack;

		nodesInStack.push(pNode->mChildren[0]);

		const CASTNode* pCurrNode = nullptr;

		while (!nodesInStack.empty())
		{
			pCurrNode = nodesInStack.top();

			nodesInStack.pop();

			for (U32 i = 0; i < childrenCount; i++)
			{
				nodesInStack.push(pCurrNode->mChildren[i]);
			}

			delete pCurrNode;

			pCurrNode = nullptr;
		}
	}


	/*!
		\brief CASTSourceUnitNode's definition
	*/

	CASTSourceUnitNode::CASTSourceUnitNode(const std::string& moduleName):
		CASTNode(NT_PROGRAM_UNIT), mModuleName(moduleName)
	{
	}

	std::string CASTSourceUnitNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitProgramUnit(this);
	}

	bool CASTSourceUnitNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitProgramUnit(this);
	}

	TLLVMIRData CASTSourceUnitNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitProgramUnit(this);
	}

	const std::vector<CASTNode*>& CASTSourceUnitNode::GetStatements() const
	{
		return mChildren;
	}

	const std::string& CASTSourceUnitNode::GetModuleName() const
	{
		return mModuleName;
	}


	CASTDeclarationNode::CASTDeclarationNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo, U32 attributes):
		CASTTypeNode(NT_DECL), mAttributes(attributes)
	{
		AttachChild(pIdentifiers);
		AttachChild(pTypeInfo);
	}

	std::string CASTDeclarationNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitDeclaration(this);
	}

	bool CASTDeclarationNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitDeclaration(this);
	}

	TLLVMIRData CASTDeclarationNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitDeclaration(this);
	}

	CType* CASTDeclarationNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitDeclaration(this);
	}

	void CASTDeclarationNode::SetTypeInfo(CASTNode* pTypeInfo)
	{
		mChildren[1] = pTypeInfo;
	}

	CASTNode* CASTDeclarationNode::GetIdentifiers() const
	{
		return mChildren[0];
	}

	CASTTypeNode* CASTDeclarationNode::GetTypeInfo() const
	{
		return dynamic_cast<CASTTypeNode*>(mChildren[1]);
	}

	U32 CASTDeclarationNode::GetAttributes() const
	{
		return mAttributes;
	}


	CASTBlockNode::CASTBlockNode():
		CASTNode(NT_BLOCK)
	{
	}

	std::string CASTBlockNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitStatementsBlock(this);
	}

	bool CASTBlockNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitStatementsBlock(this);
	}

	TLLVMIRData CASTBlockNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitStatementsBlock(this);
	}

	const std::vector<CASTNode*>& CASTBlockNode::GetStatements() const
	{
		return mChildren;
	}

	
	/*!
		\brief CASTTypeNode's definition
	*/

	CASTTypeNode::CASTTypeNode(E_NODE_TYPE type):
		CASTNode(type)
	{
	}

	std::string CASTTypeNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return {};
	}

	bool CASTTypeNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return true;
	}

	TLLVMIRData CASTTypeNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	CType* CASTTypeNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitBaseNode(this);
	}


	/*!
		CASTIdentifierNode definition
	*/
	
	CASTIdentifierNode::CASTIdentifierNode(const std::string& name, U32 attributes) :
		CASTTypeNode(NT_IDENTIFIER), mName(name), mAttributes(attributes)
	{
	}

	std::string CASTIdentifierNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitIdentifier(this);
	}

	bool CASTIdentifierNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitIdentifier(this);
	}

	TLLVMIRData CASTIdentifierNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitIdentifier(this);
	}

	CType* CASTIdentifierNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitIdentifier(this);
	}

	const std::string& CASTIdentifierNode::GetName() const
	{
		return mName;
	}

	U32 CASTIdentifierNode::GetAttributes() const
	{
		return mAttributes;
	}


	/*!
		\brief CASTLiteralNode's definition
	*/

	CASTLiteralNode::CASTLiteralNode(CBaseValue* pValue):
		CASTTypeNode(NT_LITERAL), mpValue(pValue)
	{
	}

	std::string CASTLiteralNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitLiteral(this);
	}

	bool CASTLiteralNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitLiteral(this);
	}

	TLLVMIRData CASTLiteralNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitLiteral(this);
	}

	CType* CASTLiteralNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitLiteral(this);
	}

	CBaseValue* CASTLiteralNode::GetValue() const
	{
		return mpValue;
	}


	CASTExpressionNode::CASTExpressionNode(E_NODE_TYPE type):
		CASTTypeNode(type)
	{
	}


	/*!
		\brief CASTUnaryExpressionNode's definition
	*/
	
	CASTUnaryExpressionNode::CASTUnaryExpressionNode(E_TOKEN_TYPE opType, CASTNode* pNode):
		CASTExpressionNode(NT_UNARY_EXPR), mOpType(opType)
	{
		AttachChild(pNode);
	}

	std::string CASTUnaryExpressionNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitUnaryExpression(this);
	}

	bool CASTUnaryExpressionNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitUnaryExpression(this);
	}

	TLLVMIRData CASTUnaryExpressionNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitUnaryExpression(this);
	}

	CType* CASTUnaryExpressionNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitUnaryExpression(this);
	}

	E_TOKEN_TYPE CASTUnaryExpressionNode::GetOpType() const
	{
		return mOpType;
	}

	CASTNode* CASTUnaryExpressionNode::GetData() const
	{
		return mChildren[0];
	}


	/*!
		\brief CASTBinaryExpressionNode's definition
	*/

	CASTBinaryExpressionNode::CASTBinaryExpressionNode(CASTExpressionNode* pLeft, E_TOKEN_TYPE opType, CASTExpressionNode* pRight):
		CASTExpressionNode(NT_BINARY_EXPR), mOpType(opType)
	{
		AttachChild(pLeft);
		AttachChild(pRight);
	}

	std::string CASTBinaryExpressionNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitBinaryExpression(this);
	}

	bool CASTBinaryExpressionNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitBinaryExpression(this);
	}

	TLLVMIRData CASTBinaryExpressionNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitBinaryExpression(this);
	}

	CType* CASTBinaryExpressionNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitBinaryExpression(this);
	}

	CASTExpressionNode* CASTBinaryExpressionNode::GetLeft() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[0]);
	}

	CASTExpressionNode* CASTBinaryExpressionNode::GetRight() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[1]);
	}

	E_TOKEN_TYPE CASTBinaryExpressionNode::GetOpType() const
	{
		return mOpType;
	}


	/*!
		\brief CASTAssignmentNode definition
	*/

	CASTAssignmentNode::CASTAssignmentNode(CASTExpressionNode* pLeft, CASTExpressionNode* pRight) :
		CASTNode(NT_ASSIGNMENT)
	{
		AttachChild(pLeft);
		AttachChild(pRight);
	}

	std::string CASTAssignmentNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitAssignment(this);
	}

	bool CASTAssignmentNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitAssignment(this);
	}

	TLLVMIRData CASTAssignmentNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitAssignment(this);
	}

	CASTExpressionNode* CASTAssignmentNode::GetLeft() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[0]);
	}

	CASTExpressionNode* CASTAssignmentNode::GetRight() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[1]);
	}


	/*!
		\brief CASTIfStatementNode's definition
	*/

	CASTIfStatementNode::CASTIfStatementNode(CASTExpressionNode* pCondition, CASTBlockNode* pThenBlock, CASTBlockNode* pElseBlock):
		CASTNode(NT_IF_STATEMENT)
	{
		AttachChild(pCondition);
		AttachChild(pThenBlock);
		AttachChild(pElseBlock);
	}

	std::string CASTIfStatementNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitIfStatement(this);
	}

	bool CASTIfStatementNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitIfStatement(this);
	}

	TLLVMIRData CASTIfStatementNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitIfStatement(this);
	}

	CASTExpressionNode* CASTIfStatementNode::GetCondition() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[0]);
	}

	CASTBlockNode* CASTIfStatementNode::GetThenBlock() const
	{
		return dynamic_cast<CASTBlockNode*>(mChildren[1]);
	}

	CASTBlockNode* CASTIfStatementNode::GetElseBlock() const
	{
		if (mChildren.size() < 3)
		{
			return nullptr;
		}

		return dynamic_cast<CASTBlockNode*>(mChildren[2]);
	}


	/*!
		\brief CASTLoopStatementNode's definition
	*/

	CASTLoopStatementNode::CASTLoopStatementNode(CASTBlockNode* pBody):
		CASTNode(NT_LOOP_STATEMENT)
	{
		AttachChild(pBody);
	}

	std::string CASTLoopStatementNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitLoopStatement(this);
	}

	bool CASTLoopStatementNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitLoopStatement(this);
	}

	TLLVMIRData CASTLoopStatementNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitLoopStatement(this);
	}

	CASTBlockNode* CASTLoopStatementNode::GetBody() const
	{
		return dynamic_cast<CASTBlockNode*>(mChildren[0]);
	}


	/*!
		\brief CASTWhileLoopStatementNode's definition
	*/

	CASTWhileLoopStatementNode::CASTWhileLoopStatementNode(CASTExpressionNode* pCondition, CASTBlockNode* pBody) :
		CASTNode(NT_WHILE_STATEMENT)
	{
		AttachChild(pCondition);
		AttachChild(pBody);
	}

	std::string CASTWhileLoopStatementNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitWhileLoopStatement(this);
	}

	bool CASTWhileLoopStatementNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitWhileLoopStatement(this);
	}

	TLLVMIRData CASTWhileLoopStatementNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitWhileLoopStatement(this);
	}

	CASTExpressionNode* CASTWhileLoopStatementNode::GetCondition() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[0]);
	}

	CASTBlockNode* CASTWhileLoopStatementNode::GetBody() const
	{
		return dynamic_cast<CASTBlockNode*>(mChildren[1]);
	}


	/*!
		\brief CASTFunctionDeclNode's definition
	*/


	CASTFunctionDeclNode::CASTFunctionDeclNode(CASTFunctionClosureNode* pClosure, CASTFunctionArgsNode* pArgs, CASTNode* pReturnValue):
		CASTTypeNode(NT_FUNC_DECL)
	{
		AttachChild(pArgs);
		AttachChild(pReturnValue);
		AttachChild(pClosure);
	}

	std::string CASTFunctionDeclNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionDeclaration(this);
	}

	bool CASTFunctionDeclNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitFunctionDeclaration(this);
	}

	TLLVMIRData CASTFunctionDeclNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFunctionDeclaration(this);
	}

	CType* CASTFunctionDeclNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitFunctionDeclaration(this);
	}

	CASTFunctionClosureNode* CASTFunctionDeclNode::GetClosure() const
	{
		if (mChildren.size() < 3)
		{
			return nullptr;
		}

		return dynamic_cast<CASTFunctionClosureNode*>(mChildren[2]);
	}

	CASTFunctionArgsNode* CASTFunctionDeclNode::GetArgs() const
	{
		return dynamic_cast<CASTFunctionArgsNode*>(mChildren[0]);
	}

	CASTNode* CASTFunctionDeclNode::GetReturnValueType() const
	{
		return mChildren[1];
	}


	/*!
		\brief CASTFunctionClosureNode's definition
	*/


	CASTFunctionClosureNode::CASTFunctionClosureNode():
		CASTNode(NT_FUNC_CLOSURE)
	{
	}

	std::string CASTFunctionClosureNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionClosure(this);
	}

	bool CASTFunctionClosureNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitFunctionClosure(this);
	}

	TLLVMIRData CASTFunctionClosureNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFunctionClosure(this);
	}


	/*!
		\brief CASTFunctionArgsNode's definition
	*/


	CASTFunctionArgsNode::CASTFunctionArgsNode() :
		CASTNode(NT_FUNC_ARGS)
	{
	}

	std::string CASTFunctionArgsNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionArgs(this);
	}

	bool CASTFunctionArgsNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitFunctionArgs(this);
	}

	TLLVMIRData CASTFunctionArgsNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFunctionArgs(this);
	}


	/*!
		\brief CASTFunctionCallNode's definition
	*/

	CASTFunctionCallNode::CASTFunctionCallNode(CASTUnaryExpressionNode* pIdentifier, CASTNode* pArgsList):
		CASTTypeNode(NT_FUNC_CALL)
	{
		AttachChild(pIdentifier);
		AttachChild(pArgsList);
	}

	std::string CASTFunctionCallNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionCall(this);
	}

	bool CASTFunctionCallNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitFunctionCall(this);
	}

	TLLVMIRData CASTFunctionCallNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFunctionCall(this);
	}

	CType* CASTFunctionCallNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitFunctionCall(this);
	}

	CASTUnaryExpressionNode* CASTFunctionCallNode::GetIdentifier() const
	{
		return dynamic_cast<CASTUnaryExpressionNode*>(mChildren[0]);
	}

	CASTNode* CASTFunctionCallNode::GetArgs() const
	{
		return mChildren[1];
	}

	
	/*!
		\brief CASTReturnStatementNode's definition
	*/

	CASTReturnStatementNode::CASTReturnStatementNode(CASTExpressionNode* pExpression):
		CASTNode(NT_RETURN_STATEMENT)
	{
		AttachChild(pExpression);
	}

	std::string CASTReturnStatementNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitReturnStatement(this);
	}

	bool CASTReturnStatementNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitReturnStatement(this);
	}

	TLLVMIRData CASTReturnStatementNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitReturnStatement(this);
	}

	CASTExpressionNode* CASTReturnStatementNode::GetExpr() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[0]);
	}


	/*!
		\brief CASTDefinitionNode's definition
	*/

	CASTDefinitionNode::CASTDefinitionNode(CASTDeclarationNode* pDecl, CASTNode* pValue) :
		CASTTypeNode(NT_DEFINITION)
	{
		AttachChild(pDecl);
		AttachChild(pValue);
	}

	std::string CASTDefinitionNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitDefinitionNode(this);
	}
	
	bool CASTDefinitionNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitDefinitionNode(this);
	}

	TLLVMIRData CASTDefinitionNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitDefinitionNode(this);
	}

	CType* CASTDefinitionNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitDefinition(this);
	}

	CASTDeclarationNode* CASTDefinitionNode::GetDeclaration() const
	{
		return dynamic_cast<CASTDeclarationNode*>(mChildren[0]);
	}

	CASTNode* CASTDefinitionNode::GetValue() const
	{
		return mChildren[1];
	}


	/*!
		\brief CASTFuncDefinitionNode's definition
	*/
	
	CASTFuncDefinitionNode::CASTFuncDefinitionNode(CASTDeclarationNode* pDecl, CASTFunctionDeclNode* pLambdaType, CASTNode* pBody):
		CASTDefinitionNode(pDecl, pBody)
	{
		AttachChild(pLambdaType);
	}

	std::string CASTFuncDefinitionNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionDefNode(this);
	}

	bool CASTFuncDefinitionNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitFunctionDefNode(this);
	}

	TLLVMIRData CASTFuncDefinitionNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFunctionDefNode(this);
	}

	CASTFunctionDeclNode* CASTFuncDefinitionNode::GetLambdaTypeInfo() const
	{
		return dynamic_cast<CASTFunctionDeclNode*>(mChildren[2]);
	}


	/*!
		\brief CASTEnumDeclNode's definition
	*/

	CASTEnumDeclNode::CASTEnumDeclNode(CASTIdentifierNode* pEnumName):
		CASTTypeNode(NT_ENUM_DECL)
	{
		AttachChild(pEnumName);
	}

	std::string CASTEnumDeclNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitEnumDeclaration(this);
	}

	bool CASTEnumDeclNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitEnumDeclaration(this);
	}

	TLLVMIRData CASTEnumDeclNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitEnumDeclaration(this);
	}

	CType* CASTEnumDeclNode::Resolve(ITypeResolver* pResolver)
	{
		return {};
	}

	// get values
	CASTIdentifierNode* CASTEnumDeclNode::GetEnumName() const
	{
		return dynamic_cast<CASTIdentifierNode*>(mChildren[0]);
	}


	/*!
		\brief CASTStructDeclNode's definition
	*/

	CASTStructDeclNode::CASTStructDeclNode(CASTIdentifierNode* pStructName, CASTBlockNode* pStructFields):
		CASTTypeNode(NT_STRUCT_DECL)
	{
		AttachChild(pStructName);
		AttachChild(pStructFields);
	}

	std::string CASTStructDeclNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitStructDeclaration(this);
	}

	bool CASTStructDeclNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitStructDeclaration(this);
	}

	TLLVMIRData CASTStructDeclNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitStructDeclaration(this);
	}

	CType* CASTStructDeclNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitStructDeclaration(this);
	}

	// get values
	CASTIdentifierNode* CASTStructDeclNode::GetStructName() const
	{
		return dynamic_cast<CASTIdentifierNode*>(mChildren[0]);
	}

	CASTBlockNode* CASTStructDeclNode::GetFieldsDeclarations() const
	{
		return dynamic_cast<CASTBlockNode*>(mChildren[1]);
	}


	/*!
		\brief CASTNamedTypeNode's definition
	*/

	CASTNamedTypeNode::CASTNamedTypeNode(CASTIdentifierNode* pIdentifier):
		CASTTypeNode(NT_DEPENDENT_TYPE)
	{
		AttachChild(pIdentifier);
	}

	std::string CASTNamedTypeNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return {};
	}

	bool CASTNamedTypeNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitIdentifier(GetTypeInfo());
	}

	TLLVMIRData CASTNamedTypeNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	CType* CASTNamedTypeNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitNamedType(this);
	}
	
	CASTIdentifierNode* CASTNamedTypeNode::GetTypeInfo() const
	{
		return dynamic_cast<CASTIdentifierNode*>(mChildren[0]);
	}


	/*!
		\brief CASTBreakOperator's definition
	*/
	
	CASTBreakOperatorNode::CASTBreakOperatorNode():
		CASTNode(NT_BREAK_OPERATOR)
	{
	}

	std::string CASTBreakOperatorNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitBreakOperator(this);
	}

	bool CASTBreakOperatorNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitBreakOperator(this);
	}

	TLLVMIRData CASTBreakOperatorNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitBreakOperator(this);
	}


	/*!
		\brief CASTContinueOperator's definition
	*/

	CASTContinueOperatorNode::CASTContinueOperatorNode() :
		CASTNode(NT_CONTINUE_OPERATOR)
	{
	}

	std::string CASTContinueOperatorNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitContinueOperator(this);
	}

	bool CASTContinueOperatorNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitContinueOperator(this);
	}

	TLLVMIRData CASTContinueOperatorNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitContinueOperator(this);
	}


	/*!
		\brief CASTAccessOperatorNode's definition
	*/

	CASTAccessOperatorNode::CASTAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pMemberName):
		CASTExpressionNode(NT_ACCESS_OPERATOR)
	{
		AttachChild(pExpression);
		AttachChild(pMemberName);
	}

	std::string CASTAccessOperatorNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitAccessOperator(this);
	}

	bool CASTAccessOperatorNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitAccessOperator(this);
	}

	TLLVMIRData CASTAccessOperatorNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitAccessOperator(this);
	}

	CType* CASTAccessOperatorNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitAccessOperator(this);
	}

	CASTExpressionNode* CASTAccessOperatorNode::GetExpression() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[0]);
	}

	CASTExpressionNode* CASTAccessOperatorNode::GetMemberName() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[1]);
	}


	/*!
		\brief CASTArrayTypeNode's definition
	*/

	CASTArrayTypeNode::CASTArrayTypeNode(CASTNode* pTypeInfo, CASTExpressionNode* pSizeExpr):
		CASTTypeNode(NT_ARRAY)
	{
		AttachChild(pTypeInfo);
		AttachChild(pSizeExpr);
	}

	std::string CASTArrayTypeNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitArrayTypeNode(this);
	}

	bool CASTArrayTypeNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitArrayTypeNode(this);
	}

	TLLVMIRData CASTArrayTypeNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitArrayTypeNode(this);
	}

	CType* CASTArrayTypeNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitArrayType(this);
	}

	CASTNode* CASTArrayTypeNode::GetTypeInfo() const
	{
		return mChildren[0];
	}

	CASTExpressionNode* CASTArrayTypeNode::GetSizeExpr() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[1]);
	}


	/*!
		\brief CASTIndexedAccessOperatorNode's definition
	*/

	CASTIndexedAccessOperatorNode::CASTIndexedAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pIndexExpr, U32 attributes) :
		CASTExpressionNode(NT_INDEXED_ACCESS_OPERATOR), mAttributes(attributes)
	{
		AttachChild(pExpression);
		AttachChild(pIndexExpr);
	}

	std::string CASTIndexedAccessOperatorNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitIndexedAccessOperator(this);
	}

	bool CASTIndexedAccessOperatorNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitIndexedAccessOperator(this);
	}

	TLLVMIRData CASTIndexedAccessOperatorNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitIndexedAccessOperator(this);
	}

	CType* CASTIndexedAccessOperatorNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitIndexedAccessOperator(this);
	}

	CASTExpressionNode* CASTIndexedAccessOperatorNode::GetExpression() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[0]);
	}

	CASTExpressionNode* CASTIndexedAccessOperatorNode::GetIndexExpression() const
	{
		return dynamic_cast<CASTExpressionNode*>(mChildren[1]);
	}

	U32 CASTIndexedAccessOperatorNode::GetAttributes() const
	{
		return mAttributes;
	}


	/*!
		\brief CASTPointerTypeNode's definition
	*/

	CASTPointerTypeNode::CASTPointerTypeNode(CASTNode* pTypeInfo):
		CASTTypeNode(NT_POINTER)
	{
		AttachChild(pTypeInfo);
	}
		
	std::string CASTPointerTypeNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitPointerTypeNode(this);
	}

	bool CASTPointerTypeNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitPointerTypeNode(this);
	}

	TLLVMIRData CASTPointerTypeNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitPointerTypeNode(this);
	}

	CType* CASTPointerTypeNode::Resolve(ITypeResolver* pResolver)
	{
		return pResolver->VisitPointerType(this);
	}

	CASTNode* CASTPointerTypeNode::GetTypeInfo() const
	{
		return mChildren[0];
	}


	/*!
		\brief CASTImportDirectiveNode's definition
	*/

	CASTImportDirectiveNode::CASTImportDirectiveNode(const std::string& modulePath, const std::string& moduleName):
		CASTNode(NT_IMPORT), mModulePath(modulePath), mModuleName(moduleName)
	{
	}

	std::string CASTImportDirectiveNode::Accept(IASTNodeVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitImportDirectiveNode(this);
	}

	bool CASTImportDirectiveNode::Accept(IASTNodeVisitor<bool>* pVisitor)
	{
		return pVisitor->VisitImportDirectiveNode(this);
	}

	TLLVMIRData CASTImportDirectiveNode::Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitImportDirectiveNode(this);
	}

	const std::string& CASTImportDirectiveNode::GetModulePath() const
	{
		return mModulePath;
	}

	const std::string& CASTImportDirectiveNode::GetImportedModuleName() const
	{
		return mModuleName;
	}
}
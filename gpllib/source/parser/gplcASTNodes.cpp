/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright

	\brief The file contains AST nodes defenitions

	\todo
*/

#include "parser/gplcASTNodes.h"
#include "common/gplcLiterals.h"
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

	CASTNode::~CASTNode()
	{
		U32 childrenCount = mChildren.size();

		if (childrenCount >= 1)
		{
			std::stack<const CASTNode*> nodesInStack;

			nodesInStack.push(mChildren[0]);

			const CASTNode* pCurrNode = nullptr;

			while (!nodesInStack.empty())
			{
				pCurrNode = nodesInStack.top();

				nodesInStack.pop();

				childrenCount = pCurrNode->mChildren.size();

				for (U32 i = 0; i < childrenCount; i++)
				{
					nodesInStack.push(pCurrNode->mChildren[i]);
				}

				delete pCurrNode;

				pCurrNode = nullptr;
			}
		}
	}
	
	std::string CASTNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return std::string();
	}

	Result CASTNode::AttachChild(CASTNode* node)
	{
		if (node == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}

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

	CASTSourceUnitNode::CASTSourceUnitNode():
		CASTNode(NT_PROGRAM_UNIT)
	{
	}

	CASTSourceUnitNode::~CASTSourceUnitNode()
	{
	}

	std::string CASTSourceUnitNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitProgramUnit(this);
	}

	const std::vector<CASTNode*>& CASTSourceUnitNode::GetStatements() const
	{
		return mChildren;
	}


	CASTDeclarationNode::CASTDeclarationNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo):
		CASTNode(NT_DECL)
	{
		AttachChild(pIdentifiers);
		AttachChild(pTypeInfo);
	}

	CASTDeclarationNode::~CASTDeclarationNode()
	{
	}

	std::string CASTDeclarationNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return {};
	}

	CASTNode* CASTDeclarationNode::GetIdentifiers() const
	{
		return mChildren[0];
	}

	CASTNode* CASTDeclarationNode::GetTypeInfo() const
	{
		return mChildren[1];
	}


	CASTBlockNode::CASTBlockNode():
		CASTNode(NT_BLOCK)
	{
	}

	CASTBlockNode::~CASTBlockNode()
	{
	}

	std::string CASTBlockNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitStatementsBlock(this);
	}

	const std::vector<CASTNode*>& CASTBlockNode::GetStatements() const
	{
		return mChildren;
	}


	/*!
		CASTIdentifierNode definition
	*/

	CASTIdentifierNode::CASTIdentifierNode() :
		CASTNode(NT_IDENTIFIER)
	{
	}

	CASTIdentifierNode::CASTIdentifierNode(const CASTIdentifierNode& node) :
		CASTNode(node)
	{
	}

	CASTIdentifierNode::CASTIdentifierNode(const std::string& name) :
		CASTNode(NT_IDENTIFIER), mName(name)
	{
	}

	CASTIdentifierNode::~CASTIdentifierNode()
	{
	}

	std::string CASTIdentifierNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitIdentifier(this);
	}

	const std::string& CASTIdentifierNode::GetName() const
	{
		return mName;
	}


	/*!
		\brief CASTLiteralNode's definition
	*/

	CASTLiteralNode::CASTLiteralNode(const CBaseLiteral* pValue):
		CASTNode(NT_LITERAL), mpValue(pValue)
	{
	}

	CASTLiteralNode::~CASTLiteralNode()
	{

	}

	std::string CASTLiteralNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitLiteral(this);
	}

	const CBaseLiteral* CASTLiteralNode::GetValue() const
	{
		return mpValue;
	}


	CASTExpressionNode::CASTExpressionNode(E_NODE_TYPE type):
		CASTNode(type)
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

	CASTUnaryExpressionNode:: ~CASTUnaryExpressionNode()
	{
		// \todo add implementation of the destructor
	}

	std::string CASTUnaryExpressionNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitUnaryExpression(this);
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

	CASTBinaryExpressionNode::~CASTBinaryExpressionNode()
	{
		// \todo add implementation of the destructor
	}

	std::string CASTBinaryExpressionNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitBinaryExpression(this);
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

	CASTAssignmentNode::CASTAssignmentNode(CASTUnaryExpressionNode* pLeft, CASTExpressionNode* pRight) :
		CASTNode(NT_ASSIGNMENT)
	{
		AttachChild(pLeft);
		AttachChild(pRight);
	}

	CASTAssignmentNode::~CASTAssignmentNode()
	{
		// \todo add implementation of the destructor
	}

	std::string CASTAssignmentNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitAssignment(this);
	}

	CASTUnaryExpressionNode* CASTAssignmentNode::GetLeft() const
	{
		return dynamic_cast<CASTUnaryExpressionNode*>(mChildren[0]);
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

	CASTIfStatementNode::~CASTIfStatementNode()
	{
	}

	std::string CASTIfStatementNode::Accept(IVisitor<std::string>* pVisitor)
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

	CASTLoopStatementNode::~CASTLoopStatementNode()
	{
	}

	std::string CASTLoopStatementNode::Accept(IVisitor<std::string>* pVisitor)
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

	CASTWhileLoopStatementNode::~CASTWhileLoopStatementNode()
	{
	}

	std::string CASTWhileLoopStatementNode::Accept(IVisitor<std::string>* pVisitor)
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
		CASTNode(NT_FUNC_DECL)
	{
		AttachChild(pClosure);
		AttachChild(pArgs);
		AttachChild(pReturnValue);
	}

	CASTFunctionDeclNode::~CASTFunctionDeclNode()
	{
	}

	std::string CASTFunctionDeclNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionDeclaration(this);
	}

	CASTFunctionClosureNode* CASTFunctionDeclNode::GetClosure() const
	{
		return dynamic_cast<CASTFunctionClosureNode*>(mChildren[0]);
	}

	CASTFunctionArgsNode* CASTFunctionDeclNode::GetArgs() const
	{
		return dynamic_cast<CASTFunctionArgsNode*>(mChildren[1]);
	}

	CASTNode* CASTFunctionDeclNode::GetReturnValueType() const
	{
		return mChildren[2];
	}


	/*!
		\brief CASTFunctionClosureNode's definition
	*/


	CASTFunctionClosureNode::CASTFunctionClosureNode():
		CASTNode(NT_FUNC_CLOSURE)
	{
	}

	CASTFunctionClosureNode::~CASTFunctionClosureNode() 
	{
	}

	std::string CASTFunctionClosureNode::Accept(IVisitor<std::string>* pVisitor)
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

	CASTFunctionArgsNode::~CASTFunctionArgsNode()
	{
	}

	std::string CASTFunctionArgsNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionArgs(this);
	}


	/*!
		\brief CASTFunctionCallNode's definition
	*/

	CASTFunctionCallNode::CASTFunctionCallNode(CASTUnaryExpressionNode* pIdentifier, CASTNode* pArgsList):
		CASTNode(NT_FUNC_CALL)
	{
		AttachChild(pIdentifier);
		AttachChild(pArgsList);
	}

	CASTFunctionCallNode::~CASTFunctionCallNode()
	{
	}

	std::string CASTFunctionCallNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionCall(this);
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

	CASTReturnStatementNode::~CASTReturnStatementNode()
	{
	}

	std::string CASTReturnStatementNode::Accept(IVisitor<std::string>* pVisitor)
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
		CASTNode(NT_DEFINITION)
	{
		AttachChild(pDecl);
		AttachChild(pValue);
	}

	CASTDefinitionNode::~CASTDefinitionNode()
	{
	}

	std::string CASTDefinitionNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitDefinitionNode(this);
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

	CASTFuncDefinitionNode::~CASTFuncDefinitionNode()
	{
	}

	std::string CASTFuncDefinitionNode::Accept(IVisitor<std::string>* pVisitor)
	{
		return pVisitor->VisitFunctionDefNode(this);
	}

	CASTFunctionDeclNode* CASTFuncDefinitionNode::GetLambdaTypeInfo() const
	{
		return dynamic_cast<CASTFunctionDeclNode*>(mChildren[2]);
	}
}
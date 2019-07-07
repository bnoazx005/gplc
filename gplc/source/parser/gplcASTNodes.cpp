/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright

	\brief The file contains AST nodes defenitions

	\todo
*/

#include "parser\gplcASTNodes.h"
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

	Result CASTNode::AttachChild(const CASTNode* node)
	{
		if (node == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}

		mChildren.push_back(node);

		return RV_SUCCESS;
	}
	
	Result CASTNode::AttachChildren(const std::vector<const CASTNode*>& nodes)
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

		std::vector<const CASTNode*>::iterator currElement = std::find(mChildren.begin(), mChildren.end(), *node);

		if (currElement == mChildren.end())
		{
			return RV_FAIL;
		}

		mChildren.erase(currElement);

		delete *currElement;

		*currElement = nullptr;
		
		return RV_SUCCESS;
	}

	const std::vector<const CASTNode*> CASTNode::GetChildren() const
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

		std::stack<const CASTNode*> nodesInStack;

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
	
	CASTUnaryExpressionNode::CASTUnaryExpressionNode(const CASTNode* pOpNode, const CASTNode* pNode):
		CASTExpressionNode(NT_UNARY_EXPR)
	{
		AttachChild(pOpNode);
		AttachChild(pNode);
	}

	CASTUnaryExpressionNode:: ~CASTUnaryExpressionNode()
	{
		// \todo add implementation of the destructor
	}

	const CASTNode* CASTUnaryExpressionNode::GetOperator() const
	{
		return mChildren[0];
	}

	const CASTNode* CASTUnaryExpressionNode::GetData() const
	{
		return mChildren[1];
	}


	/*!
		\brief CASTBinaryExpressionNode's definition
	*/

	CASTBinaryExpressionNode::CASTBinaryExpressionNode(const CASTExpressionNode* pLeft, E_TOKEN_TYPE opType, const CASTExpressionNode* pRight):
		CASTExpressionNode(NT_BINARY_EXPR), mOpType(opType)
	{
		AttachChild(pLeft);
		AttachChild(pRight);
	}

	CASTBinaryExpressionNode::~CASTBinaryExpressionNode()
	{
		// \todo add implementation of the destructor
	}

	const CASTExpressionNode* CASTBinaryExpressionNode::GetLeft() const
	{
		return dynamic_cast<const CASTExpressionNode*>(mChildren[0]);
	}

	const CASTExpressionNode* CASTBinaryExpressionNode::GetRight() const
	{
		return dynamic_cast<const CASTExpressionNode*>(mChildren[1]);
	}

	E_TOKEN_TYPE CASTBinaryExpressionNode::GetOpType() const
	{
		return mOpType;
	}


	/*!
		\brief CASTAssignmentNode definition
	*/

	CASTAssignmentNode::CASTAssignmentNode(const CASTUnaryExpressionNode* pLeft, const CASTExpressionNode* pRight) :
		CASTNode(NT_ASSIGNMENT)
	{
		AttachChild(pLeft);
		AttachChild(pRight);
	}

	CASTAssignmentNode::~CASTAssignmentNode()
	{
		// \todo add implementation of the destructor
	}

	const CASTUnaryExpressionNode* CASTAssignmentNode::GetLeft() const
	{
		return dynamic_cast<const CASTUnaryExpressionNode*>(mChildren[0]);
	}

	const CASTExpressionNode* CASTAssignmentNode::GetRight() const
	{
		return dynamic_cast<const CASTExpressionNode*>(mChildren[1]);
	}
}
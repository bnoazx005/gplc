/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright

	\brief The file contains AST nodes defenitions

	\todo
*/

#include "parser\gplcASTNodes.h"


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

	/*!
		CASTIdentifierNode defenition
	*/

	CASTIdentifierNode::CASTIdentifierNode() :
		CASTNode(NT_IDENTIFIER)
	{
	}

	CASTIdentifierNode::CASTIdentifierNode(const CASTIdentifierNode& node) :
		CASTNode(node)
	{
	}

	CASTIdentifierNode::CASTIdentifierNode(const std::wstring& name) :
		CASTNode(NT_IDENTIFIER), mName(name)
	{
	}

	CASTIdentifierNode::~CASTIdentifierNode()
	{
	}

	const std::wstring& CASTIdentifierNode::GetName() const
	{
		return mName;
	}
}
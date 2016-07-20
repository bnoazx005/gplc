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

	CASTNode::CASTNode(const CASTNode& node)
	{
	}

	CASTNode::~CASTNode()
	{
	}

	Result CASTNode::AttachChild(const CASTNode* node)
	{
		return RV_FAIL;
	}

	Result CASTNode::DettachChild(CASTNode** node)
	{
		return RV_FAIL;
	}

	const std::vector<CASTNode*> CASTNode::GetChildren() const
	{
		return mChildren;
	}

	U32 CASTNode::GetChildrenCount() const
	{
		return mChildren.size();
	}
}
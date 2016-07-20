/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright

	\brief The file contains AST nodes declarations

	\todo
*/

#ifndef GPLC_AST_NODES_H
#define GPLC_AST_NODES_H


#include "common\gplcTypes.h"
#include <vector>


namespace gplc
{
	class CASTNode
	{
		public:
			CASTNode();
			virtual ~CASTNode();

			Result AttachChild(const CASTNode* node);
			Result DettachChild(CASTNode** node);

			const std::vector<CASTNode*> GetChildren() const;
			U32 GetChildrenCount() const;
		protected:
			CASTNode(const CASTNode& node);
		protected:
			std::vector<CASTNode*> mChildren;
	};
}

#endif

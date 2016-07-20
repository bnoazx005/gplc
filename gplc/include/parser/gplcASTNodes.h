/*!
	\author Ildar Kasimov
	\date   20.07.2016
	\copyright

	\brief The file contains AST nodes declarations

	\todo
*/

#ifndef GPLC_AST_NODES_H
#define GPLC_AST_NODES_H


#include <vector>


namespace gplc
{
	class CASTNode
	{
		public:
			CASTNode();
			virtual ~CASTNode();

			void AttachChild(const CASTNode* node);
			void DettachChild(CASTNode** node);

			const std::vector<CASTNode*> GetChildren() const;
			U32 GetChildrenCount() const;
		protected:
			std::vector<CASTNode*> mChildren;
	};
}

#endif

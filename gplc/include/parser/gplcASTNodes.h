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
	/*!
		\brief E_NODE_TYPE enumeration
	*/
	
	enum E_NODE_TYPE
	{
		NT_PROGRAM_UNIT, ///< A program unit
		NT_STATEMENTS,   ///< Statements
		NT_OPERATOR,     ///< An operator
		NT_DECL,         ///< A declration operator
	};

	/*!
		\brief CASTNode class
	*/

	class CASTNode
	{
		public:
			CASTNode(E_NODE_TYPE type);
			virtual ~CASTNode();

			Result AttachChild(const CASTNode* node);
			
			Result AttachChildren(const std::vector<const CASTNode*>& nodes);

			Result DettachChild(CASTNode** node);
			
			const std::vector<const CASTNode*> GetChildren() const;

			U32 GetChildrenCount() const;

			E_NODE_TYPE GetType() const;
		protected:
			CASTNode(const CASTNode& node);
		protected:
			E_NODE_TYPE                  mType;

			std::vector<const CASTNode*> mChildren;
	};
}

#endif

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
		NT_IDENTIFIER,   ///< An identifier
		NT_BUILTIN_TYPE, ///< A builtin type (intX, uintX, float, double, string, char)
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
			CASTNode();
			CASTNode(const CASTNode& node);
		protected:
			E_NODE_TYPE                  mType;

			std::vector<const CASTNode*> mChildren;
	};


	/*!
		\brief CASTIdentifierNode
	*/

	class CASTIdentifierNode : public CASTNode
	{
		public:
			CASTIdentifierNode(const std::wstring& name);
			virtual ~CASTIdentifierNode();

			const std::wstring& GetName() const;
		protected:
			CASTIdentifierNode();
			CASTIdentifierNode(const CASTIdentifierNode& node);
		protected:
			std::wstring mName;
	};

}

#endif

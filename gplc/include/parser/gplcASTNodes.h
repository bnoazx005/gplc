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
#include "..\lexer\gplcTokens.h"
#include <vector>


namespace gplc
{
	/*!
		\brief E_NODE_TYPE enumeration
	*/
	
	enum E_NODE_TYPE
	{
		NT_PROGRAM_UNIT,     ///< A program unit
		NT_STATEMENTS,       ///< Statements
		NT_OPERATOR,         ///< An operator
		NT_DECL,             ///< A declration operator
		NT_IDENTIFIER,       ///< An identifier
		NT_IDENTIFIERS_LIST,
		NT_INT8,
		NT_INT16,
		NT_INT32,
		NT_INT64,
		NT_UINT8,
		NT_UINT16,
		NT_UINT32,
		NT_UINT64,
		NT_CHAR,
		NT_STRING,
		NT_BOOL,
		NT_VOID,
		NT_POINTER,
		NT_ASSIGNMENT,
		NT_UNARY_EXPR,
		NT_BINARY_EXPR,
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

			virtual void _removeNode(CASTNode** node);
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
			CASTIdentifierNode(const std::string& name);
			virtual ~CASTIdentifierNode();

			const std::string& GetName() const;
		protected:
			CASTIdentifierNode();
			CASTIdentifierNode(const CASTIdentifierNode& node);
		protected:
			std::string mName;
	};


	class CASTExpressionNode : public CASTNode
	{
		public:
			CASTExpressionNode(E_NODE_TYPE type);
			virtual ~CASTExpressionNode() = default;
		protected:
			CASTExpressionNode() = default;
			CASTExpressionNode(const CASTExpressionNode& node) = default;
	};


	class CASTUnaryExpressionNode : public CASTExpressionNode
	{
		public:
			CASTUnaryExpressionNode(const CASTNode* pOpNode, const CASTNode* pNode);
			virtual ~CASTUnaryExpressionNode();

			const CASTNode* GetOperator() const;

			const CASTNode* GetData() const;
		protected:
			CASTUnaryExpressionNode() = default;
			CASTUnaryExpressionNode(const CASTUnaryExpressionNode& node) = default;
	};

	
	class CASTBinaryExpressionNode : public CASTExpressionNode
	{
		public:
			CASTBinaryExpressionNode(const CASTExpressionNode* pLeft, E_TOKEN_TYPE opType, const CASTExpressionNode* pRight);
			virtual ~CASTBinaryExpressionNode();

			const CASTExpressionNode* GetLeft() const;

			const CASTExpressionNode* GetRight() const;

			E_TOKEN_TYPE GetOpType() const;
		protected:
			CASTBinaryExpressionNode() = default;
			CASTBinaryExpressionNode(const CASTBinaryExpressionNode& node) = default;
		protected:
			E_TOKEN_TYPE mOpType;
	};


	class CASTAssignmentNode : public CASTNode
	{
		public:
			CASTAssignmentNode(const CASTUnaryExpressionNode* pLeft, const CASTExpressionNode* pRight);
			virtual ~CASTAssignmentNode();

			const CASTUnaryExpressionNode* GetLeft() const;

			const CASTExpressionNode* GetRight() const;
		protected:
			CASTAssignmentNode() = default;
			CASTAssignmentNode(const CASTAssignmentNode& node) = default;
	};

}

#endif

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
#include "common/gplcVisitor.h"
#include <vector>


namespace gplc
{
	class CBaseLiteral;


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
		NT_LITERAL,
	};

	/*!
		\brief CASTNode class
	*/

	class CASTNode: public virtual IVisitable<std::string>
	{
		public:
			CASTNode(E_NODE_TYPE type);
			virtual ~CASTNode(); 
			
			std::string Accept(IVisitor<std::string>* pVisitor) override;

			Result AttachChild(CASTNode* node);
			
			Result AttachChildren(const std::vector<CASTNode*>& nodes);

			Result DettachChild(CASTNode** node);
			
			const std::vector<CASTNode*> GetChildren() const;

			U32 GetChildrenCount() const;

			E_NODE_TYPE GetType() const;
		protected:
			CASTNode();
			CASTNode(const CASTNode& node);

			virtual void _removeNode(CASTNode** node);
		protected:
			E_NODE_TYPE            mType;

			std::vector<CASTNode*> mChildren;
	};
	

	class CASTSourceUnitNode: public CASTNode
	{
		public:
			CASTSourceUnitNode();
			virtual ~CASTSourceUnitNode();

			std::string Accept(IVisitor<std::string>* pVisitor) override;

			const std::vector<CASTNode*>& GetStatements() const;
		protected:
			CASTSourceUnitNode(const CASTSourceUnitNode& node) = default;
	};


	class CASTDeclarationNode : public CASTNode
	{
		public:
			CASTDeclarationNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo);
			virtual ~CASTDeclarationNode();

			std::string Accept(IVisitor<std::string>* pVisitor) override;

			CASTNode* GetIdentifiers() const;

			CASTNode* GetTypeInfo() const;
		protected:
			CASTDeclarationNode() = default;
			CASTDeclarationNode(const CASTDeclarationNode& node) = default;
	};


	/*!
		\brief CASTIdentifierNode
	*/

	class CASTIdentifierNode : public CASTNode
	{
		public:
			CASTIdentifierNode(const std::string& name);
			virtual ~CASTIdentifierNode();

			std::string Accept(IVisitor<std::string>* pVisitor) override;

			const std::string& GetName() const;
		protected:
			CASTIdentifierNode();
			CASTIdentifierNode(const CASTIdentifierNode& node);
		protected:
			std::string mName;
	};


	class CASTLiteralNode : public CASTNode
	{
		public:
			CASTLiteralNode(const CBaseLiteral* pValue);
			virtual ~CASTLiteralNode();

			std::string Accept(IVisitor<std::string>* pVisitor) override;

			const CBaseLiteral* GetValue() const;
		protected:
			CASTLiteralNode() = default;
			CASTLiteralNode(const CASTLiteralNode& node) = default;
		protected:
			const CBaseLiteral* mpValue;
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
			CASTUnaryExpressionNode(E_TOKEN_TYPE opType, CASTNode* pNode);
			virtual ~CASTUnaryExpressionNode();

			std::string Accept(IVisitor<std::string>* pVisitor) override;

			E_TOKEN_TYPE GetOpType() const;

			CASTNode* GetData() const;
		protected:
			CASTUnaryExpressionNode() = default;
			CASTUnaryExpressionNode(const CASTUnaryExpressionNode& node) = default;
		protected:
			E_TOKEN_TYPE mOpType;
	};

	
	class CASTBinaryExpressionNode : public CASTExpressionNode
	{
		public:
			CASTBinaryExpressionNode(CASTExpressionNode* pLeft, E_TOKEN_TYPE opType, CASTExpressionNode* pRight);
			virtual ~CASTBinaryExpressionNode();

			std::string Accept(IVisitor<std::string>* pVisitor) override;

			CASTExpressionNode* GetLeft() const;

			CASTExpressionNode* GetRight() const;

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
			CASTAssignmentNode(CASTUnaryExpressionNode* pLeft, CASTExpressionNode* pRight);
			virtual ~CASTAssignmentNode();

			std::string Accept(IVisitor<std::string>* pVisitor) override;

			CASTUnaryExpressionNode* GetLeft() const;

			CASTExpressionNode* GetRight() const;
		protected:
			CASTAssignmentNode() = default;
			CASTAssignmentNode(const CASTAssignmentNode& node) = default;
	};

}

#endif

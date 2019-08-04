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
	class CBaseValue;
	class CASTTypeNode;
	class CType;
	class ITypeResolver;
	class ISymTable;


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
		NT_FLOAT,
		NT_DOUBLE,
		NT_VOID,
		NT_POINTER,
		NT_ASSIGNMENT,
		NT_UNARY_EXPR,
		NT_BINARY_EXPR,
		NT_LITERAL,
		NT_IF_STATEMENT,
		NT_LOOP_STATEMENT,
		NT_WHILE_STATEMENT,
		NT_FUNC_DECL,
		NT_FUNC_CLOSURE,
		NT_FUNC_ARGS,
		NT_FUNC_CALL,
		NT_RETURN_STATEMENT,
		NT_DEFINITION,
		NT_BLOCK,
		NT_ENUM_DECL,
		NT_STRUCT_DECL,
		NT_DEPENDENT_TYPE,
		NT_BREAK_OPERATOR,
		NT_CONTINUE_OPERATOR,
		NT_ACCESS_OPERATOR,
		NT_ARRAY,
		NT_INDEXED_ACCESS_OPERATOR,
	};

	/*!
		\brief CASTNode class
	*/

	class CASTNode: public virtual IVisitable<std::string, IStringASTNodeVisitor>, public virtual IVisitable<bool, IBoolASTNodeVisitor>, 
					public virtual IVisitable<TLLVMIRData, IASTNodeVisitor<TLLVMIRData>>
	{
		public:
			CASTNode(E_NODE_TYPE type);
			virtual ~CASTNode(); 
			
			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

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

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			const std::vector<CASTNode*>& GetStatements() const;
		protected:
			CASTSourceUnitNode(const CASTSourceUnitNode& node) = default;
	};


	class CASTTypeNode : public CASTNode
	{
		public:
			CASTTypeNode(E_NODE_TYPE type);
			virtual ~CASTTypeNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			virtual CType* Resolve(ITypeResolver* pResolver);
		protected:
			CASTTypeNode(const CASTTypeNode& node) = default;
	};


	class CASTVariableDeclNode : public CASTTypeNode
	{
		public:
			CASTVariableDeclNode(CASTIdentifierNode* pIdentifier, CASTNode* pTypeInfo);
			virtual ~CASTVariableDeclNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTIdentifierNode* GetIdentifier() const;

			CASTTypeNode* GetTypeInfo() const;
		protected:
			CASTVariableDeclNode() = default;
			CASTVariableDeclNode(const CASTVariableDeclNode& node) = default;
	};


	class CASTDeclarationNode : public CASTTypeNode
	{
		public:
			CASTDeclarationNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo, U32 attributes = 0x0);
			virtual ~CASTDeclarationNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			void SetTypeInfo(CASTNode* pTypeInfo);

			CASTNode* GetIdentifiers() const;

			CASTTypeNode* GetTypeInfo() const;

			U32 GetAttributes() const;
		protected:
			CASTDeclarationNode() = default;
			CASTDeclarationNode(const CASTDeclarationNode& node) = default;
		protected:
			U32 mAttributes;
	};


	class CASTBlockNode : public CASTNode
	{
		public:
			CASTBlockNode();
			virtual ~CASTBlockNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			const std::vector<CASTNode*>& GetStatements() const;
		protected:
			CASTBlockNode(const CASTBlockNode& node) = default;
	};


	/*!
		\brief CASTIdentifierNode
	*/

	class CASTIdentifierNode : public CASTTypeNode
	{
		public:
			CASTIdentifierNode(const std::string& name, U32 attributes = 0x0);
			virtual ~CASTIdentifierNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			const std::string& GetName() const;

			U32 GetAttributes() const;
		protected:
			CASTIdentifierNode() = default;
			CASTIdentifierNode(const CASTIdentifierNode& node) = default;
		protected:
			std::string mName;

			U32         mAttributes;
	};


	class CASTLiteralNode : public CASTTypeNode
	{
		public:
			CASTLiteralNode(CBaseValue* pValue);
			virtual ~CASTLiteralNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CBaseValue* GetValue() const;
		protected:
			CASTLiteralNode() = default;
			CASTLiteralNode(const CASTLiteralNode& node) = default;
		protected:
			CBaseValue* mpValue;
	};


	class CASTExpressionNode : public CASTTypeNode
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

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

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

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

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
			CASTAssignmentNode(CASTExpressionNode* pLeft, CASTExpressionNode* pRight);
			virtual ~CASTAssignmentNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetLeft() const;

			CASTExpressionNode* GetRight() const;
		protected:
			CASTAssignmentNode() = default;
			CASTAssignmentNode(const CASTAssignmentNode& node) = default;
	};


	class CASTIfStatementNode : public CASTNode
	{
		public:
			CASTIfStatementNode(CASTExpressionNode* pCondition, CASTBlockNode* pThenBlock, CASTBlockNode* pElseBlock);
			virtual ~CASTIfStatementNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetCondition() const;

			CASTBlockNode* GetThenBlock() const;

			CASTBlockNode* GetElseBlock() const;
		protected:
			CASTIfStatementNode() = default;
			CASTIfStatementNode(const CASTIfStatementNode& node) = default;
	};


	class CASTLoopStatementNode : public CASTNode
	{
		public:
			CASTLoopStatementNode(CASTBlockNode* pBody);
			virtual ~CASTLoopStatementNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTBlockNode* GetBody() const;
		protected:
			CASTLoopStatementNode() = default;
			CASTLoopStatementNode(const CASTLoopStatementNode& node) = default;
	};

	
	class CASTWhileLoopStatementNode : public CASTNode
	{
		public:
			CASTWhileLoopStatementNode(CASTExpressionNode* pCondition, CASTBlockNode* pBody);
			virtual ~CASTWhileLoopStatementNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetCondition() const;

			CASTBlockNode* GetBody() const;
		protected:
			CASTWhileLoopStatementNode() = default;
			CASTWhileLoopStatementNode(const CASTWhileLoopStatementNode& node) = default;
	};


	class CASTFunctionClosureNode : public CASTNode
	{
		public:
			CASTFunctionClosureNode();
			virtual ~CASTFunctionClosureNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;
		protected:
			CASTFunctionClosureNode(const CASTFunctionClosureNode& node) = default;
	};


	class CASTFunctionArgsNode : public CASTNode
	{
		public:
			CASTFunctionArgsNode();
			virtual ~CASTFunctionArgsNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;
		protected:
			CASTFunctionArgsNode(const CASTFunctionArgsNode& node) = default;
	};
	

	class CASTFunctionDeclNode : public CASTTypeNode
	{
		public:
			CASTFunctionDeclNode(CASTFunctionClosureNode* pClosure, CASTFunctionArgsNode* pArgs, CASTNode* pReturnValue);
			virtual ~CASTFunctionDeclNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTFunctionClosureNode* GetClosure() const;

			CASTFunctionArgsNode* GetArgs() const;

			CASTNode* GetReturnValueType() const;
		protected:
			CASTFunctionDeclNode() = default;
			CASTFunctionDeclNode(const CASTFunctionDeclNode& node) = default;
	};


	class CASTFunctionCallNode : public CASTTypeNode
	{
		public:
			CASTFunctionCallNode(CASTUnaryExpressionNode* pIdentifier, CASTNode* pArgsList);
			virtual ~CASTFunctionCallNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTUnaryExpressionNode* GetIdentifier() const;

			CASTNode* GetArgs() const;
		protected:
			CASTFunctionCallNode() = default;
			CASTFunctionCallNode(const CASTFunctionCallNode& node) = default;
	};


	class CASTReturnStatementNode : public CASTNode
	{
		public:
			CASTReturnStatementNode(CASTExpressionNode* pExpression);
			virtual ~CASTReturnStatementNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetExpr() const;
		protected:
			CASTReturnStatementNode() = default;
			CASTReturnStatementNode(const CASTReturnStatementNode& node) = default;
	};


	class CASTDefinitionNode : public CASTNode
	{
		public:
			CASTDefinitionNode(CASTDeclarationNode* pDecl, CASTNode* pValue);
			virtual ~CASTDefinitionNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTDeclarationNode* GetDeclaration() const;

			CASTNode* GetValue() const;
		protected:
			CASTDefinitionNode() = default;
			CASTDefinitionNode(const CASTDefinitionNode& node) = default;
	};


	class CASTFuncDefinitionNode : public CASTDefinitionNode
	{
		public:
			CASTFuncDefinitionNode(CASTDeclarationNode* pDecl, CASTFunctionDeclNode* pLambdaType, CASTNode* pBody);
			virtual ~CASTFuncDefinitionNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTFunctionDeclNode* GetLambdaTypeInfo() const;
		protected:
			CASTFuncDefinitionNode() = default;
			CASTFuncDefinitionNode(const CASTFuncDefinitionNode& node) = default;
	};
	

	class CASTEnumDeclNode : public CASTTypeNode
	{
		public:
			CASTEnumDeclNode(CASTIdentifierNode* pEnumName);
			virtual ~CASTEnumDeclNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			// get values
			CASTIdentifierNode* GetEnumName() const;
		protected:
			CASTEnumDeclNode(const CASTEnumDeclNode& node) = default;
	};


	class CASTStructDeclNode : public CASTTypeNode
	{
		public:
			CASTStructDeclNode(CASTIdentifierNode* pStructName, CASTBlockNode* pStructFields);
			virtual ~CASTStructDeclNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			// get values
			CASTIdentifierNode* GetStructName() const;

			CASTBlockNode* GetFieldsDeclarations() const;
		protected:
			CASTStructDeclNode(const CASTStructDeclNode& node) = default;
	};


	class CASTNamedTypeNode : public CASTTypeNode
	{
		public:
			CASTNamedTypeNode(CASTIdentifierNode* pIdentifier);
			virtual ~CASTNamedTypeNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTIdentifierNode* GetTypeInfo() const;
		protected:
			CASTNamedTypeNode() = default;
			CASTNamedTypeNode(const CASTNamedTypeNode& node) = default;
	};


	class CASTBreakOperatorNode : public CASTNode
	{
		public:
			CASTBreakOperatorNode();
			virtual ~CASTBreakOperatorNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;
		protected:
			CASTBreakOperatorNode(const CASTBreakOperatorNode& node) = default;
	};


	class CASTContinueOperatorNode : public CASTNode
	{
		public:
			CASTContinueOperatorNode();
			virtual ~CASTContinueOperatorNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;
		protected:
			CASTContinueOperatorNode(const CASTContinueOperatorNode& node) = default;
	};


	class CASTAccessOperatorNode : public CASTExpressionNode
	{
		public:
			CASTAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pMemberName);
			virtual ~CASTAccessOperatorNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTExpressionNode* GetExpression() const;

			CASTExpressionNode* GetMemberName() const;
		protected:
			CASTAccessOperatorNode() = default;
			CASTAccessOperatorNode(const CASTAccessOperatorNode& node) = default;
	};


	class CASTArrayTypeNode : public CASTTypeNode
	{
		public:
			CASTArrayTypeNode(CASTNode* pTypeInfo, CASTExpressionNode* pSizeExpr);
			virtual ~CASTArrayTypeNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			virtual CType* Resolve(ITypeResolver* pResolver);

			CASTNode* GetTypeInfo() const;

			CASTExpressionNode* GetSizeExpr() const;
		protected:
			CASTArrayTypeNode(const CASTArrayTypeNode& node) = default;
	};

	
	class CASTIndexedAccessOperatorNode : public CASTExpressionNode
	{
		public:
			CASTIndexedAccessOperatorNode(CASTExpressionNode* pExpression, CASTExpressionNode* pIndexExpr, U32 attributes);
			virtual ~CASTIndexedAccessOperatorNode();

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTExpressionNode* GetExpression() const;

			CASTExpressionNode* GetIndexExpression() const;

			U32 GetAttributes() const;
		protected:
			CASTIndexedAccessOperatorNode() = default;
			CASTIndexedAccessOperatorNode(const CASTIndexedAccessOperatorNode& node) = default;
		protected:
			U32 mAttributes;
	};
}

#endif

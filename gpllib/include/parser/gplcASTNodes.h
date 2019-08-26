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
		NT_IMPORT,
		NT_DEFER_OPERATOR,
		NT_SIZEOF_OPERATOR,
		NT_TYPEID_OPERATOR,
		NT_MEMCPY32_INTRINSIC,
		NT_MEMCPY64_INTRINSIC,
		NT_MEMSET32_INTRINSIC,
		NT_MEMSET64_INTRINSIC,
		NT_ASSERT_INTRINSIC,
	};

	/*!
		\brief CASTNode class
	*/

	class CASTNode: public virtual IVisitable<std::string, IStringASTNodeVisitor>, public virtual IVisitable<bool, IBoolASTNodeVisitor>, 
					public virtual IVisitable<TLLVMIRData, IASTNodeVisitor<TLLVMIRData>>
	{
		public:
			CASTNode(E_NODE_TYPE type, U32 attributes = 0x0);
			virtual ~CASTNode() = default;
			
			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			Result AttachChild(CASTNode* node);
			
			Result AttachChildren(const std::vector<CASTNode*>& nodes);

			Result DettachChild(CASTNode** node);
						
			void SetAttribute(U32 attribute);

			const std::vector<CASTNode*> GetChildren() const;

			U32 GetChildrenCount() const;

			E_NODE_TYPE GetType() const;

			U32 GetAttributes() const;
		protected:
			CASTNode();
			CASTNode(const CASTNode& node);

			virtual void _removeNode(CASTNode** node);
		protected:
			E_NODE_TYPE            mType;

			std::vector<CASTNode*> mChildren;

			U32                    mAttributes;
	};
	

	class CASTSourceUnitNode: public CASTNode
	{
		public:
			CASTSourceUnitNode(const std::string& moduleName);
			virtual ~CASTSourceUnitNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			const std::vector<CASTNode*>& GetStatements() const;

			const std::string& GetModuleName() const;
		protected:
			CASTSourceUnitNode(const CASTSourceUnitNode& node) = default;
		protected:
			std::string mModuleName;
	};


	class CASTTypeNode : public CASTNode
	{
		public:
			CASTTypeNode(E_NODE_TYPE type, U32 attributes = 0x0);
			virtual ~CASTTypeNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			virtual CType* Resolve(ITypeResolver* pResolver);
		protected:
			CASTTypeNode(const CASTTypeNode& node) = default;
	};


	class CASTDeclarationNode : public CASTTypeNode
	{
		public:
			CASTDeclarationNode(CASTNode* pIdentifiers, CASTNode* pTypeInfo, U32 attributes = 0x0);
			virtual ~CASTDeclarationNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			void SetTypeInfo(CASTNode* pTypeInfo);

			CASTNode* GetIdentifiers() const;

			CASTTypeNode* GetTypeInfo() const;
		protected:
			CASTDeclarationNode() = default;
			CASTDeclarationNode(const CASTDeclarationNode& node) = default;
	};


	class CASTBlockNode : public CASTNode
	{
		public:
			CASTBlockNode();
			virtual ~CASTBlockNode() = default;

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
			virtual ~CASTIdentifierNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			const std::string& GetName() const;
		protected:
			CASTIdentifierNode() = default;
			CASTIdentifierNode(const CASTIdentifierNode& node) = default;
		protected:
			std::string mName;
	};


	class CASTLiteralNode : public CASTTypeNode
	{
		public:
			CASTLiteralNode(CBaseValue* pValue);
			virtual ~CASTLiteralNode() = default;

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
			CASTExpressionNode(E_NODE_TYPE type, U32 attributes = 0x0);
			virtual ~CASTExpressionNode() = default;
		protected:
			CASTExpressionNode() = default;
			CASTExpressionNode(const CASTExpressionNode& node) = default;
	};


	class CASTUnaryExpressionNode : public CASTExpressionNode
	{
		public:
			CASTUnaryExpressionNode(E_TOKEN_TYPE opType, CASTNode* pNode);
			virtual ~CASTUnaryExpressionNode() = default;

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
			virtual ~CASTBinaryExpressionNode() = default;

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
			virtual ~CASTAssignmentNode() = default;

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
			virtual ~CASTIfStatementNode() = default;

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
			virtual ~CASTLoopStatementNode() = default;

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
			virtual ~CASTWhileLoopStatementNode() = default;

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
			virtual ~CASTFunctionClosureNode() = default;

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
			virtual ~CASTFunctionArgsNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;
		protected:
			CASTFunctionArgsNode(const CASTFunctionArgsNode& node) = default;
	};
	

	class CASTFunctionDeclNode : public CASTTypeNode
	{
		public:
			CASTFunctionDeclNode(CASTFunctionClosureNode* pClosure, CASTFunctionArgsNode* pArgs, CASTNode* pReturnValue, U32 attributes = 0x0);
			virtual ~CASTFunctionDeclNode() = default;

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
			virtual ~CASTFunctionCallNode() = default;

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
			virtual ~CASTReturnStatementNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetExpr() const;
		protected:
			CASTReturnStatementNode() = default;
			CASTReturnStatementNode(const CASTReturnStatementNode& node) = default;
	};


	class CASTDefinitionNode : public CASTTypeNode
	{
		public:
			CASTDefinitionNode(CASTDeclarationNode* pDecl, CASTNode* pValue);
			virtual ~CASTDefinitionNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

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
			virtual ~CASTFuncDefinitionNode() = default;

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
			virtual ~CASTEnumDeclNode() = default;

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
			virtual ~CASTStructDeclNode() = default;

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
			CASTNamedTypeNode(CASTIdentifierNode* pIdentifier, U32 attributes = 0x0);
			virtual ~CASTNamedTypeNode() = default;

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
			virtual ~CASTBreakOperatorNode() = default;

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
			virtual ~CASTContinueOperatorNode() = default;

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
			virtual ~CASTAccessOperatorNode() = default;

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
			virtual ~CASTArrayTypeNode() = default;

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
			virtual ~CASTIndexedAccessOperatorNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTExpressionNode* GetExpression() const;

			CASTExpressionNode* GetIndexExpression() const;
		protected:
			CASTIndexedAccessOperatorNode() = default;
			CASTIndexedAccessOperatorNode(const CASTIndexedAccessOperatorNode& node) = default;
	};


	class CASTPointerTypeNode : public CASTTypeNode
	{
		public:
			CASTPointerTypeNode(CASTNode* pTypeInfo);
			virtual ~CASTPointerTypeNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			virtual CType* Resolve(ITypeResolver* pResolver);

			CASTNode* GetTypeInfo() const;
		protected:
			CASTPointerTypeNode(const CASTPointerTypeNode& node) = default;
	};


	class CASTImportDirectiveNode : public CASTTypeNode
	{
		public:
			CASTImportDirectiveNode(const std::string& modulePath, const std::string& moduleName);
			virtual ~CASTImportDirectiveNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			virtual CType* Resolve(ITypeResolver* pResolver);

			const std::string& GetModulePath() const;

			const std::string& GetImportedModuleName() const;
		protected:
			CASTImportDirectiveNode(const CASTImportDirectiveNode& node) = default;
		protected:
			std::string mModulePath;

			std::string mModuleName;
	};


	class CASTDeferOperatorNode : public CASTNode
	{
		public:
			CASTDeferOperatorNode(CASTExpressionNode* pExpr);
			virtual ~CASTDeferOperatorNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetExpr() const;
		protected:
			CASTDeferOperatorNode() = default;
			CASTDeferOperatorNode(const CASTDeferOperatorNode&) = default;
	};


	class CASTIntrinsicCallNode : public CASTTypeNode
	{
		public:
			CASTIntrinsicCallNode(E_NODE_TYPE intrinsicType, CASTNode* pArgsList);
			virtual ~CASTIntrinsicCallNode() = default;

			std::string Accept(IASTNodeVisitor<std::string>* pVisitor) override;
			bool Accept(IASTNodeVisitor<bool>* pVisitor) override;
			TLLVMIRData Accept(IASTNodeVisitor<TLLVMIRData>* pVisitor) override;

			CType* Resolve(ITypeResolver* pResolver) override;

			CASTNode* GetArgs() const;
		protected:
			CASTIntrinsicCallNode() = default;
			CASTIntrinsicCallNode(const CASTIntrinsicCallNode& node) = default;
	};
}

#endif

/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains types declaration

	\todo
*/

#ifndef GPLC_TYPE_SYSTEM_H
#define GPLC_TYPE_SYSTEM_H


#include "gplcTypes.h"
#include "parser/gplcASTNodes.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "common/gplcVisitor.h"


namespace gplc
{
	class CASTTypeNode;
	class ISymTable;
	class CType;
	class CBaseValue;
	class IConstExprInterpreter;
	class IASTNodesFactory;


	E_COMPILER_TYPES NodeToCompilerType(E_NODE_TYPE nodeType);


	/*!
		\brief The sizes of builtin types in bytes
	*/

	enum E_BUILTIN_TYPES_SIZES
	{
		BTS_INT8    = 1,
		BTS_INT16   = 2,
		BTS_INT32   = 4,
		BTS_INT64   = 8,
		BTS_UINT8   = 1,
		BTS_UINT16  = 2,
		BTS_UINT32  = 4,
		BTS_UINT64  = 8,
		BTS_FLOAT   = 4,
		BTS_DOUBLE  = 8,
		BTS_CHAR    = 2,
		BTS_VOID    = 4,
		BTS_BOOL    = 1,
		BTS_POINTER = 4,
		BTS_UNKNOWN,
	};


	/*!
		\brief The interface describes a functionality of a type
		resolver which is used within semantic analysys pass
	*/

	class ITypeResolver
	{
		public:
			ITypeResolver() = default;
			virtual ~ITypeResolver() = default;

			virtual Result Init(ISymTable* pSymTable, IConstExprInterpreter* pInterpreter) = 0;

			/*!
				\brief The method deduces type based on information's taken from pTypeNode and pSymTable
			*/

			virtual CType* Resolve(CASTTypeNode* pTypeNode) = 0;

			virtual CType* VisitBaseNode(CASTTypeNode* pNode) = 0;
			virtual CType* VisitIdentifier(CASTIdentifierNode* pNode) = 0;
			virtual CType* VisitLiteral(CASTLiteralNode* pNode) = 0;
			virtual CType* VisitUnaryExpression(CASTUnaryExpressionNode* pNode) = 0;
			virtual CType* VisitBinaryExpression(CASTBinaryExpressionNode* pNode) = 0;
			virtual CType* VisitDeclaration(CASTDeclarationNode* pNode) = 0;
			virtual CType* VisitDefinition(CASTDefinitionNode* pNode) = 0;
			virtual CType* VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) = 0;
			virtual CType* VisitFunctionCall(CASTFunctionCallNode* pNode) = 0;
			virtual CType* VisitStructDeclaration(CASTStructDeclNode* pNode) = 0;
			virtual CType* VisitNamedType(CASTNamedTypeNode* pNode) = 0;
			virtual CType* VisitArrayType(CASTArrayTypeNode* pNode) = 0;
			virtual CType* VisitAccessOperator(CASTAccessOperatorNode* pNode) = 0;
			virtual CType* VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode) = 0;
			virtual CType* VisitPointerType(CASTPointerTypeNode* pNode) = 0;
			virtual CType* VisitModuleType(CASTImportDirectiveNode* pNode) = 0;
	};


	class CTypeResolver: public ITypeResolver
	{
		public:
			CTypeResolver() = default;
			virtual ~CTypeResolver() = default;

			Result Init(ISymTable* pSymTable, IConstExprInterpreter* pInterpreter) override;

			/*!
				\brief The method deduces type based on information's taken from pTypeNode and pSymTable
			*/

			CType* Resolve(CASTTypeNode* pTypeNode) override;

			CType* VisitBaseNode(CASTTypeNode* pNode) override;
			CType* VisitIdentifier(CASTIdentifierNode* pNode) override;
			CType* VisitLiteral(CASTLiteralNode* pNode) override;
			CType* VisitUnaryExpression(CASTUnaryExpressionNode* pNode) override;
			CType* VisitBinaryExpression(CASTBinaryExpressionNode* pNode) override;
			CType* VisitDeclaration(CASTDeclarationNode* pNode) override;
			CType* VisitDefinition(CASTDefinitionNode* pNode) override;
			CType* VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) override;
			CType* VisitFunctionCall(CASTFunctionCallNode* pNode) override;
			CType* VisitStructDeclaration(CASTStructDeclNode* pNode) override; 
			CType* VisitNamedType(CASTNamedTypeNode* pNode) override;
			CType* VisitArrayType(CASTArrayTypeNode* pNode) override;
			CType* VisitAccessOperator(CASTAccessOperatorNode* pNode) override;
			CType* VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode) override; 
			CType* VisitPointerType(CASTPointerTypeNode* pNode) override;
			CType* VisitModuleType(CASTImportDirectiveNode* pNode) override;
		protected:
			CType* _deduceBuiltinType(E_NODE_TYPE type, U32 attributes = 0x0);

			CType* _deduceExprType(E_TOKEN_TYPE opType, E_COMPILER_TYPES leftType, E_COMPILER_TYPES rightType);
		protected:
			ISymTable*             mpSymTable;

			IConstExprInterpreter* mpConstExprInterpreter;
	};


	/*!
		\brief CType class
	*/

	class CType: public IVisitable<TLLVMIRData, ITypeVisitor<TLLVMIRData>>
	{
		protected:
			typedef std::unordered_map<E_COMPILER_TYPES, std::unordered_map<E_COMPILER_TYPES, bool>> TCastMap;
		public:
			CType(E_COMPILER_TYPES type, U32 size, U32 attributes, const std::string& name = "");
			virtual ~CType();

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			virtual bool IsBuiltIn() const;

			void SetAttribute(U32 attribute);

			virtual void SetName(const std::string& name);

			const std::vector<const CType*> GetChildTypes() const;

			virtual E_COMPILER_TYPES GetType() const;

			U32 GetChildTypesCount() const;

			virtual U32 GetSize() const;

			virtual U32 GetAttributes() const;

			virtual const std::string& GetName() const;

			virtual CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const;

			/*!
				\brief The operator of equality checks up whether two
				types are same or not. This implementation doesn't support
				implicit type castings
			*/

			virtual bool AreSame(const CType* pType) const;

			virtual bool AreConvertibleTo(const CType* pType) const;

			virtual std::string ToShortAliasString() const;
		protected:
			CType();
			CType(const CType& type);

			Result _addChildTypeDesc(const CType* type);

			Result _removeChildTypeDesc(CType** type);

			CBaseValue* _getBuiltinTypeDefaultValue(E_COMPILER_TYPES type) const;
		protected:
			static TCastMap     mCastMap;

			E_COMPILER_TYPES    mType;

			U32                 mSize;

			U32                 mAttributes;

			std::string         mName;

			std::vector<const CType*> mChildren;
	};


	/*!
		\brief CPointerType class
	*/

	class CPointerType : public CType
	{
		public:
			CPointerType(CType* pType);
			virtual ~CPointerType();

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const override;

			bool IsBuiltIn() const override;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;

			CType* GetBaseType() const;
		protected:
			CPointerType() = default;
			CPointerType(const CPointerType& type) = default;
		protected:
			CType* mpBaseType;
	};
	
	
	/*!
		\brief CStructureType class
	*/


	class CStructType : public CType
	{
		public:
			typedef std::vector<std::pair<std::string, CType*>> TFieldsArray;
		public:
			CStructType(const TFieldsArray& fieldsTypes, U32 attributes = 0x0);
			virtual ~CStructType() = default;

			void AddField(const std::string& fieldName, CType* pFieldType);

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			void SetAttributes(U32 attributes);

			const TFieldsArray& GetFieldsTypes() const;

			CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const override;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;
		protected:
			CStructType() = default;
			CStructType(const CStructType& structure) = default;
		protected:
			TFieldsArray mFieldsTypes;
	};

	
	/*!
		\brief CFunctionType's definition
	*/

	class CFunctionType : public CType
	{
		public:
			typedef std::vector<std::pair<std::string, CType*>> TArgsArray;
		public:
			CFunctionType(const TArgsArray& argsTypes, CType* pReturnValueType, U32 attributes = 0x0);
			virtual ~CFunctionType() = default;

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			void SetAttributes(U32 attributes);

			const TArgsArray& GetArgsTypes() const;

			CType* GetReturnValueType() const;

			CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const override;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;
		protected:
			CFunctionType() = default;
			CFunctionType(const CFunctionType& function) = default;
		protected:
			TArgsArray mArgsTypes;

			CType*     mpReturnValueType;
	};

	/*!
		\brief CEnumerationType class
	*/

	class CEnumType : public CType
	{		
		public:
			CEnumType(const ISymTable* pSymTable, const std::string& enumName);
			virtual ~CEnumType() = default;
			
			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const override;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;
		protected:
			CEnumType() = default;
			CEnumType(const CEnumType& enumType) = default;
		protected:
			const ISymTable* mpSymTable;
	};


	/*!
		\brief CDependentNamedType class
	*/

	class CDependentNamedType : public CType
	{
		public:
			CDependentNamedType(const ISymTable* pSymTable, const std::string& typeIdentifier);
			virtual ~CDependentNamedType() = default;

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			bool IsBuiltIn() const override;

			CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const override;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;

			E_COMPILER_TYPES GetType() const override;
			
			U32 GetSize() const override;

			CType* GetDependentType() const;

			U32 GetAttributes() const override;
		protected:
			CDependentNamedType() = default;
			CDependentNamedType(const CDependentNamedType& type) = default;

			const CType* _getDependentType();
		protected:
			const ISymTable* mpSymTable;

			const CType*     mpDependentType;
	};


	/*!
		\brief CArrayType class
	*/

	class CArrayType : public CType
	{
		public:
			CArrayType(CType* pBaseType, U32 elementsCount, U32 attribute = 0x0);
			virtual ~CArrayType();

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			void SetAttributes(U32 attributes);

			CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const override;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;

			CType* GetBaseType() const;

			U32 GetElementsCount() const;
		protected:
			CArrayType() = default;
			CArrayType(const CArrayType& arrayType) = default;
		protected:
			CType* mpBaseType;

			U32    mElementsCount;
	};


	/*!
		\brief CModuleType's definition
	*/

	class CModuleType : public CType
	{
		public:
			CModuleType(const std::string& moduleName, U32 attributes = 0x0);
			virtual ~CModuleType() = default;

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;
			
			CASTExpressionNode* GetDefaultValue(IASTNodesFactory* pNodesFactory) const override;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;
		protected:
			CModuleType() = default;
			CModuleType(const CModuleType& structure) = default;
		protected:
	};


	std::string CreateAnonymousLambdaName(const CFunctionType* pLambdaType);
}

#endif
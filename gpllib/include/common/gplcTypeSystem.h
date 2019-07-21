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

			/*!
				\brief The method deduces type based on information's taken from pTypeNode and pSymTable
			*/

			virtual CType* Resolve(CASTTypeNode* pTypeNode, ISymTable* pSymTable) = 0;

			virtual CType* VisitBaseNode(CASTTypeNode* pNode) = 0;
			virtual CType* VisitIdentifier(CASTIdentifierNode* pNode) = 0;
			virtual CType* VisitLiteral(CASTLiteralNode* pNode) = 0;
			virtual CType* VisitUnaryExpression(CASTUnaryExpressionNode* pNode) = 0;
			virtual CType* VisitBinaryExpression(CASTBinaryExpressionNode* pNode) = 0;
			virtual CType* VisitDeclaration(CASTDeclarationNode* pNode) = 0;
			virtual CType* VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) = 0;
			virtual CType* VisitFunctionCall(CASTFunctionCallNode* pNode) = 0;
	};


	class CTypeResolver: public ITypeResolver
	{
		public:
			CTypeResolver() = default;
			virtual ~CTypeResolver() = default;

			/*!
				\brief The method deduces type based on information's taken from pTypeNode and pSymTable
			*/

			CType* Resolve(CASTTypeNode* pTypeNode, ISymTable* pSymTable) override;

			CType* VisitBaseNode(CASTTypeNode* pNode) override;
			CType* VisitIdentifier(CASTIdentifierNode* pNode) override;
			CType* VisitLiteral(CASTLiteralNode* pNode) override;
			CType* VisitUnaryExpression(CASTUnaryExpressionNode* pNode) override;
			CType* VisitBinaryExpression(CASTBinaryExpressionNode* pNode) override;
			CType* VisitDeclaration(CASTDeclarationNode* pNode) override;
			CType* VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) override;
			CType* VisitFunctionCall(CASTFunctionCallNode* pNode) override;
		protected:
			CType* _deduceBuiltinType(E_NODE_TYPE type);

			CType* _deduceExprType(E_TOKEN_TYPE opType, E_COMPILER_TYPES leftType, E_COMPILER_TYPES rightType);
		protected:
			ISymTable* mpSymTable;
	};


	/*!
		\brief CType class
	*/

	class CType: public IVisitable<TLLVMIRData, ITypeVisitor<TLLVMIRData>>
	{
		protected:
			typedef std::unordered_map<E_COMPILER_TYPES, std::unordered_map<E_COMPILER_TYPES, bool>> TCastMap;
		public:
			CType(E_COMPILER_TYPES type, U32 size, U32 attributes);
			virtual ~CType();

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			bool IsBuiltIn() const;

			virtual void SetName(const std::string& name);

			const std::vector<const CType*> GetChildTypes() const;

			E_COMPILER_TYPES GetType() const;

			U32 GetChildTypesCount() const;

			U32 GetSize() const;

			U32 GetAttributes() const;

			virtual CBaseValue* GetDefaultValue() const;

			/*!
				\brief The operator of equality checks up whether two
				types are same or not. Type's attributes aren't considered
				within the comparison. This implementation doesn't support
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

			std::vector<const CType*> mChildren;
	};


	/*!
		\brief CPointerType class
	*/

	class CPointerType : public CType
	{
		public:
			CPointerType(const CType* type);
			virtual ~CPointerType();
		protected:
			CPointerType();
			CPointerType(const CPointerType& type);
	};
	
	/*!
		\brief CArrayType class
	*/

	
	/*!
		\brief CStructureType class
	*/

	
	/*!
		\brief CFunctionType's definition
	*/

	class CFunctionType : public CType
	{
		public:
			CFunctionType(const std::vector<CType*>& argsTypes, CType* pReturnValueType, U32 attributes = 0x0);
			virtual ~CFunctionType() = default;

			TLLVMIRData Accept(ITypeVisitor<TLLVMIRData>* pVisitor) override;

			void SetName(const std::string& name) override;

			void SetAttributes(U32 attributes);

			const std::vector<CType*>& GetArgsTypes() const;

			CType* GetReturnValueType() const;

			CBaseValue* GetDefaultValue() const override;

			const std::string& GetName() const;

			bool AreSame(const CType* pType) const override;

			std::string ToShortAliasString() const override;
		protected:
			CFunctionType() = default;
			CFunctionType(const CFunctionType& function) = default;
		protected:
			std::string         mName;

			std::vector<CType*> mArgsTypes;

			CType*              mpReturnValueType;
	};

	/*!
		\brief CEnumerationType class
	*/
}

#endif
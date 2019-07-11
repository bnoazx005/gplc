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
#include <unordered_map>


namespace gplc
{
	class CASTTypeNode;
	class ISymTable;
	class CType;
	class CBaseLiteral;


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
		protected:
			CType* _deduceBuiltinType(E_NODE_TYPE type);
		protected:
			ISymTable* mpSymTable;
	};


	/*!
		\brief CType class
	*/

	class CType
	{
		public:
			CType(E_COMPILER_TYPES type, U32 size, U32 attributes);
			virtual ~CType();
			
			bool IsBuiltIn() const;

			const std::vector<const CType*> GetChildTypes() const;

			E_COMPILER_TYPES GetType() const;

			U32 GetChildTypesCount() const;

			U32 GetSize() const;

			U32 GetAttributes() const;

			virtual CBaseLiteral* GetDefaultValue() const;
		protected:
			CType();
			CType(const CType& type);

			Result _addChildTypeDesc(const CType* type);

			Result _removeChildTypeDesc(CType** type);

			CBaseLiteral* _getBuiltinTypeDefaultValue(E_COMPILER_TYPES type) const;
		protected:
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
		\brief CFunctionType class
	*/


	/*!
		\brief CEnumerationType class
	*/
}

#endif
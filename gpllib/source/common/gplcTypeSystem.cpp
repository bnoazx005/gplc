/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains types defenition

	\todo
*/

#include "common\gplcTypeSystem.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcLiterals.h"


namespace gplc
{
	/*!
		\brief CTypeResolver's definition
	*/

	CType* CTypeResolver::Resolve(CASTTypeNode* pTypeNode, ISymTable* pSymTable)
	{
		return pTypeNode->Resolve(this, pSymTable);
	}

	CType* CTypeResolver::VisitBaseNode(CASTTypeNode* pNode)
	{
		return _deduceBuiltinType(pNode->GetType());
	}

	CType* CTypeResolver::_deduceBuiltinType(E_NODE_TYPE type)
	{
		switch (type)
		{
			case NT_INT8:
				return new CType(CT_INT8, BTS_INT8, 0x0);
			case NT_INT16:
				return new CType(CT_INT16,  BTS_INT16,  0x0);
			case NT_INT32:
				return new CType(CT_INT32, BTS_INT32, 0x0);
			case NT_INT64:
				return new CType(CT_INT64, BTS_INT64, 0x0);
			case NT_UINT8:
				return new CType(CT_UINT8, BTS_UINT8, 0x0);
			case NT_UINT16:
				return new CType(CT_UINT16, BTS_UINT16, 0x0);
			case NT_UINT32:
				return new CType(CT_UINT32, BTS_UINT32, 0x0);
			case NT_UINT64:
				return new CType(CT_UINT64, BTS_UINT64, 0x0);
			case NT_FLOAT:
				return new CType(CT_FLOAT, BTS_FLOAT, 0x0);
			case NT_DOUBLE:
				return new CType(CT_DOUBLE, BTS_DOUBLE, 0x0);
			case NT_STRING:
				return new CType(CT_STRING, BTS_DOUBLE, 0x0);
			case NT_CHAR:
				return new CType(CT_CHAR, BTS_CHAR, 0x0);
			case NT_BOOL:
				return new CType(CT_BOOL, BTS_BOOL, 0x0);
		}

		return nullptr;
	}


	/*!
		CType definition
	*/

	CType::CType() :
		mType(CT_INT32), mSize(4), mAttributes(0x0)
	{
	}

	CType::CType(const CType& type) :
		mType(type.mType), mSize(type.mSize), mAttributes(type.mAttributes)
	{
	}

	CType::CType(E_COMPILER_TYPES type, U32 size, U32 attributes):
		mType(type), mSize(size), mAttributes(attributes)
	{
	}

	CType::~CType()
	{
	}

	bool CType::IsBuiltIn() const
	{
		return mChildren.empty();
	}

	const std::vector<const CType*> CType::GetChildTypes() const
	{
		return mChildren;
	}

	E_COMPILER_TYPES CType::GetType() const
	{
		return mType;
	}

	U32 CType::GetChildTypesCount() const
	{
		return mChildren.size();
	}

	U32 CType::GetSize() const
	{
		return mSize;
	}

	U32 CType::GetAttributes() const
	{
		return mAttributes;
	}
	
	CBaseLiteral* CType::GetDefaultValue() const
	{
		if (IsBuiltIn())
		{
			return _getBuiltinTypeDefaultValue(mType);
		}

		return nullptr;
	}

	Result CType::_addChildTypeDesc(const CType* type)
	{
		if (type == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}

		mChildren.push_back(type);

		return RV_SUCCESS;
	}

	Result CType::_removeChildTypeDesc(CType** type)
	{
		if (type == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}

		std::vector<const CType*>::iterator currElement = std::find(mChildren.begin(), mChildren.end(), *type);

		if (currElement == mChildren.end())
		{
			return RV_FAIL;
		}

		mChildren.erase(currElement);

		delete *currElement;

		*currElement = nullptr;

		return RV_SUCCESS;
	}

	CBaseLiteral* CType::_getBuiltinTypeDefaultValue(E_COMPILER_TYPES type) const
	{
		/*
			\todo list of unimplemented types
			CT_VOID,
				CT_POINTER,
				CT_STRUCT,
				CT_FUNCTION,
				CT_ENUM,
				CT_ARRAY,
		*/

		switch (type)
		{
			case CT_INT8:
			case CT_INT16:
			case CT_INT32:
			case CT_INT64:
				return new CIntLiteral(0);

			case CT_UINT8:
			case CT_UINT16:
			case CT_UINT32:
			case CT_UINT64:
				return new CUIntLiteral(0);

			case CT_FLOAT:
				return new CFloatLiteral(0.0f);

			case CT_DOUBLE:
				return new CDoubleLiteral(0.0);

			case CT_CHAR:
				return new CCharLiteral("\0");

			case CT_BOOL:
				return new CBoolLiteral(true);
		}

		return nullptr; ///< unknown type
	}
}
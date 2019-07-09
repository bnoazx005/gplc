/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains types defenition

	\todo
*/

#include "common\gplcTypeSystem.h"


namespace gplc
{


	/*!
		CType defenition
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
}
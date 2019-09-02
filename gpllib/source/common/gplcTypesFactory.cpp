#include "common/gplcTypesFactory.h"
#include "common/gplcTypeSystem.h"
#include <cassert>


namespace gplc
{
	CTypesFactory::~CTypesFactory()
	{
		CType* pCurrType = nullptr;

		while (!mpCachedTypes.empty())
		{
			pCurrType = mpCachedTypes.front();

			if (pCurrType)
			{
				delete pCurrType;

				pCurrType = nullptr;
			}

			mpCachedTypes.erase(mpCachedTypes.begin());
		}
	}

	CType* CTypesFactory::CreateType(E_COMPILER_TYPES type, U32 size, U32 attributes, const std::string& name, CType* pParent)
	{
		return _insertValue(new CType(type, size, attributes, name, pParent));
	}

	CPointerType* CTypesFactory::CreatePointerType(CType* pType, CType* pParent)
	{
		return _insertValue(new CPointerType(pType, pParent));
	}

	CStructType* CTypesFactory::CreateStructType(const TSubTypesArray& fieldsTypes, U32 attributes, CType* pParent)
	{
		return _insertValue(new CStructType(fieldsTypes, attributes, pParent));
	}

	CEnumType* CTypesFactory::CreateEnumType(const ISymTable* pSymTable, const std::string& enumName, CType* pParent)
	{
		return _insertValue(new CEnumType(pSymTable, enumName, pParent));
	}

	CFunctionType* CTypesFactory::CreateFunctionType(const TSubTypesArray& argsTypes, CType* pReturnValueType, U32 attributes, CType* pParent)
	{
		return _insertValue(new CFunctionType(argsTypes, pReturnValueType, attributes, pParent));
	}

	CModuleType* CTypesFactory::CreateModuleType(const std::string& moduleName, U32 attributes, CType* pParent)
	{
		return _insertValue(new CModuleType(moduleName, attributes, pParent));
	}

	CDependentNamedType* CTypesFactory::CreateDependentNamedType(const ISymTable* pSymTable, const std::string& typeIdentifier, CType* pParent)
	{
		return _insertValue(new CDependentNamedType(pSymTable, typeIdentifier, pParent));
	}

	CArrayType* CTypesFactory::CreateArrayType(CType* pBaseType, U32 elementsCount, U32 attribute, CType* pParent)
	{
		return _insertValue(new CArrayType(pBaseType, elementsCount, attribute, pParent));
	}

	CVariantType* CTypesFactory::CreateVariantType(const TTypesArray& typesArray, const std::string& name, U32 attributes, CType* pParent)
	{
		return _insertValue(new CVariantType(typesArray, name, attributes, pParent));
	}
}
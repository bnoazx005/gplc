#include "common/gplcTypesFactory.h"
#include "common/gplcTypeSystem.h"


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

	CType* CTypesFactory::CreateType(E_COMPILER_TYPES type, U32 size, U32 attributes, const std::string& name) 
	{
		return _insertValue(new CType(type, size, attributes, name));
	}

	CPointerType* CTypesFactory::CreatePointerType(CType* pType) 
	{
		return _insertValue(new CPointerType(pType));
	}

	CStructType* CTypesFactory::CreateStructType(const TSubTypesArray& fieldsTypes, U32 attributes)
	{
		return _insertValue(new CStructType(fieldsTypes, attributes));
	}

	CEnumType* CTypesFactory::CreateEnumType(const ISymTable* pSymTable, const std::string& enumName) 
	{
		return _insertValue(new CEnumType(pSymTable, enumName));
	}

	CFunctionType* CTypesFactory::CreateFunctionType(const TSubTypesArray& argsTypes, CType* pReturnValueType, U32 attributes)
	{
		return _insertValue(new CFunctionType(argsTypes, pReturnValueType, attributes));
	}

	CModuleType* CTypesFactory::CreateModuleType(const std::string& moduleName, U32 attributes) 
	{
		return _insertValue(new CModuleType(moduleName, attributes));
	}

	CDependentNamedType* CTypesFactory::CreateDependentNamedType(const ISymTable* pSymTable, const std::string& typeIdentifier) 
	{
		return _insertValue(new CDependentNamedType(pSymTable, typeIdentifier));
	}

	CArrayType* CTypesFactory::CreateArrayType(CType* pBaseType, U32 elementsCount, U32 attribute) 
	{
		return _insertValue(new CArrayType(pBaseType, elementsCount, attribute));
	}
}
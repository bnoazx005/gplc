/*!
	\author Ildar Kasimov
	\date   12.08.2019
	\copyright

	\brief The file contains definition of an abstract factory for objects of CType type

	\todo
*/
#ifndef GPLC_TYPES_FACTORY_H
#define GPLC_TYPES_FACTORY_H


#include "gplcTypes.h"
#include <vector>
#include <string>


namespace gplc
{
	class CType;
	class CPointerType;
	class CStructType;
	class CEnumType;
	class CFunctionType;
	class CModuleType;
	class CDependentNamedType;
	class CArrayType;
	class ISymTable;


	class ITypesFactory
	{
		public:
			typedef std::vector<std::pair<std::string, CType*>> TSubTypesArray;
		public:
			ITypesFactory() = default;
			virtual ~ITypesFactory() = default;

			virtual CType* CreateType(E_COMPILER_TYPES type, U32 size, U32 attributes, const std::string& name = "", CType* pParent = nullptr) = 0;
			virtual CPointerType* CreatePointerType(CType* pType, CType* pParent = nullptr) = 0;
			virtual CStructType* CreateStructType(const TSubTypesArray& fieldsTypes, U32 attributes = 0x0, CType* pParent = nullptr) = 0;
			virtual CEnumType* CreateEnumType(const ISymTable* pSymTable, const std::string& enumName, CType* pParent = nullptr) = 0;
			virtual CFunctionType* CreateFunctionType(const TSubTypesArray& argsTypes, CType* pReturnValueType, U32 attributes = 0x0, CType* pParent = nullptr) = 0;
			virtual CModuleType* CreateModuleType(const std::string& moduleName, U32 attributes = 0x0, CType* pParent = nullptr) = 0;
			virtual CDependentNamedType* CreateDependentNamedType(const ISymTable* pSymTable, const std::string& typeIdentifier, CType* pParent = nullptr) = 0;
			virtual CArrayType* CreateArrayType(CType* pBaseType, U32 elementsCount, U32 attribute = 0x0, CType* pParent = nullptr) = 0;
		protected:
			ITypesFactory(const ITypesFactory&) = delete;
	};


	class CTypesFactory: public ITypesFactory
	{
		public:
			CTypesFactory() = default;
			virtual ~CTypesFactory();
			
			CType* CreateType(E_COMPILER_TYPES type, U32 size, U32 attributes, const std::string& name = "", CType* pParent = nullptr) override;
			CPointerType* CreatePointerType(CType* pType, CType* pParent = nullptr) override;
			CStructType* CreateStructType(const TSubTypesArray& fieldsTypes, U32 attributes = 0x0, CType* pParent = nullptr) override;
			CEnumType* CreateEnumType(const ISymTable* pSymTable, const std::string& enumName, CType* pParent = nullptr) override;
			CFunctionType* CreateFunctionType(const TSubTypesArray& argsTypes, CType* pReturnValueType, U32 attributes = 0x0, CType* pParent = nullptr) override;
			CModuleType* CreateModuleType(const std::string& moduleName, U32 attributes = 0x0, CType* pParent = nullptr) override;
			CDependentNamedType* CreateDependentNamedType(const ISymTable* pSymTable, const std::string& typeIdentifier, CType* pParent = nullptr) override;
			CArrayType* CreateArrayType(CType* pBaseType, U32 elementsCount, U32 attribute = 0x0, CType* pParent = nullptr) override;
		protected:
			CTypesFactory(const CTypesFactory&) = delete;

			template <typename T>
			inline T* _insertValue(T* pValue)
			{
				mpCachedTypes.push_back(pValue);

				return pValue;
			}
		protected:
			std::vector<CType*> mpCachedTypes;
	};
}

#endif
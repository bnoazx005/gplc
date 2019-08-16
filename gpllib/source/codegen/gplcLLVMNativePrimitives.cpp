#include "codegen/gplcLLVMNativePrimitives.h"
#include "codegen/gplcLLVMCodegen.h"
#include "codegen/gplcLLVMTypeVisitor.h"
#include "common/gplcTypesFactory.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcSymTable.h"
#include "common/gplcValues.h"
#include "parser/gplcASTNodes.h"
#include "parser/gplcASTNodesFactory.h"


namespace gplc
{
	Result CLLVMNativeModules::InitModules(ISymTable* pSymTable, IASTNodesFactory* pNodesFactory, ITypesFactory* pTypesFactory)
	{
		if (mIsInitialized)
		{
			return RV_SUCCESS;
		}

		_initStringType(pSymTable, pNodesFactory, pTypesFactory);

		mIsInitialized = true;

		return RV_SUCCESS;
	}

	Result CLLVMNativeModules::_initStringType(ISymTable* pSymTable, IASTNodesFactory* pNodesFactory, ITypesFactory* pTypesFactory)
	{
		const std::string& typeName = "String";

		const std::string fieldsNames[] =
		{
			"length", "pData"
		};

		CStructType* pStringType = pTypesFactory->CreateStructType({}, 0x0, nullptr);
		pStringType->SetName(typeName);

		CType* pLengthType = pTypesFactory->CreateType(CT_UINT64, BTS_UINT64, 0x0, "", pStringType);
		CType* pDataType   = pTypesFactory->CreatePointerType(pTypesFactory->CreateType(CT_INT8, BTS_INT8, 0x0), pStringType);

		pStringType->AddField(fieldsNames[0], pLengthType);
		pStringType->AddField(fieldsNames[1], pDataType);

		pSymTable->CreateNamedScope(typeName);
		pSymTable->AddVariable({ fieldsNames[0], pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CUIntValue(0))), pLengthType });
		pSymTable->AddVariable({ fieldsNames[1], pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0))), pDataType });
		pSymTable->LeaveScope();
		
		auto pStringSymbolEntry = pSymTable->LookUpNamedScope(typeName);
		pStringSymbolEntry->mpType = pStringType;
		
		return RV_SUCCESS;
	}
}
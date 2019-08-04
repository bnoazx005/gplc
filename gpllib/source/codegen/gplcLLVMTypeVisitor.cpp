#include "codegen/gplcLLVMTypeVisitor.h"
#include "common/gplcTypeSystem.h"
#include <vector>


namespace gplc
{
	CLLVMTypeVisitor::CLLVMTypeVisitor(llvm::LLVMContext& context):
		mpContext(&context)
	{
	}

	CLLVMTypeVisitor::~CLLVMTypeVisitor()
	{
	}

	TLLVMIRData CLLVMTypeVisitor::VisitBasicType(const CType* pType)
	{
		switch (pType->GetType())
		{
			case CT_INT8:
			case CT_UINT8:
				return llvm::Type::getInt8Ty(*mpContext);
			case CT_INT16:
			case CT_UINT16:
				return llvm::Type::getInt16Ty(*mpContext);
			case CT_INT32:
			case CT_UINT32:
				return llvm::Type::getInt32Ty(*mpContext);
			case CT_INT64:
			case CT_UINT64:
				return llvm::Type::getInt64Ty(*mpContext);
			case CT_FLOAT:
				return llvm::Type::getFloatTy(*mpContext);
			case CT_DOUBLE:
				return llvm::Type::getDoubleTy(*mpContext);
			case CT_STRING:
				return llvm::Type::getInt8PtrTy(*mpContext);
			case CT_CHAR:
				return llvm::Type::getInt8PtrTy(*mpContext);
			case CT_BOOL:
				return llvm::Type::getInt1Ty(*mpContext);
		}

		return {};
	}

	TLLVMIRData CLLVMTypeVisitor::VisitFunctionType(const CFunctionType* pFuncType)
	{
		std::vector<llvm::Type*> args;

		auto pArgs = pFuncType->GetArgsTypes();

		for (auto pCurrArgType : pArgs)
		{
			args.push_back(std::get<llvm::Type*>(pCurrArgType.second->Accept(this)));
		}
		
		auto pFunctionType = llvm::FunctionType::get(std::get<llvm::Type*>(pFuncType->GetReturnValueType()->Accept(this)), args, false);

		U32 attributes = pFuncType->GetAttributes();

		if (attributes & AV_STATIC || attributes & AV_NATIVE_FUNC)
		{
			return pFunctionType;
		}

		return llvm::PointerType::get(pFunctionType, 0); // \todo return a function poitner
	}

	TLLVMIRData CLLVMTypeVisitor::VisitStructType(const CStructType* pStructType)
	{
		std::vector<llvm::Type*> structFields;

		auto fields = pStructType->GetFieldsTypes();

		for (auto currField : fields)
		{
			structFields.push_back(std::get<llvm::Type*>(currField.second->Accept(this)));
		}

		return llvm::StructType::create(structFields, pStructType->GetName());
	}

	TLLVMIRData CLLVMTypeVisitor::VisitNamedType(const CDependentNamedType* pNamedType)
	{
		//switch (pNamedType->GetType())
		//{
		//	case CT_STRUCT:
		//		return pNamedType->Get;
		//	case CT_ENUM:
		//		return llvm::Type::getInt32Ty(*mpContext);
		//		//.return llvm::StructType::create(*mpContext, pNamedType->GetName());
		//}

		assert(pNamedType && pNamedType->GetDependentType());

		return pNamedType->GetDependentType()->Accept(this);
	}

	TLLVMIRData CLLVMTypeVisitor::VisitEnumType(const CEnumType* pEnumType)
	{
		/*!
			\note an enumeration is converted into integer value
		*/

		return llvm::Type::getInt32Ty(*mpContext);// llvm::StructType::create({ llvm::Type::getInt32Ty(*mpContext) }, pEnumType->GetName());
	}

	TLLVMIRData CLLVMTypeVisitor::VisitStaticSizedArray(const CArrayType* pArrayType)
	{
		return llvm::ArrayType::get(std::get<llvm::Type*>(pArrayType->GetBaseType()->Accept(this)), pArrayType->GetElementsCount());
	}
}
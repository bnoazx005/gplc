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
				return {};
			case CT_CHAR:
				return {};
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
			args.push_back(std::get<llvm::Type*>(pCurrArgType->Accept(this)));
		}

		return llvm::FunctionType::get(std::get<llvm::Type*>(pFuncType->GetReturnValueType()->Accept(this)), args, false);
	}
}
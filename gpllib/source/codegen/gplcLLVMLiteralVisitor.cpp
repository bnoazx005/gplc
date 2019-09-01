#include "codegen/gplcLLVMLiteralVisitor.h"
#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcSymTable.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "codegen/gplcLLVMCodegen.h"
#include "utils/Utils.h"


namespace gplc
{
	CLLVMLiteralVisitor::CLLVMLiteralVisitor(llvm::LLVMContext& context, CLLVMCodeGenerator* pCodeGenerator):
		mpCodeGenerator(pCodeGenerator)
	{
		mContext = &context;
	}

	CLLVMLiteralVisitor::~CLLVMLiteralVisitor()
	{
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitIntLiteral(const CIntValue* pLiteral)
	{
		llvm::Type* pType = pLiteral->IsLong() ? llvm::Type::getInt64Ty(*mContext) : llvm::Type::getInt32Ty(*mContext);

		// \todo replace first argument with deduced one
		return llvm::ConstantInt::get(pType, pLiteral->GetValue(), true);
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitUIntLiteral(const CUIntValue* pLiteral)
	{
		llvm::Type* pType = pLiteral->IsLong() ? llvm::Type::getInt64Ty(*mContext) : llvm::Type::getInt32Ty(*mContext);

		// \todo replace first argument with deduced one
		return llvm::ConstantInt::get(pType, pLiteral->GetValue(), false);
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitFloatLiteral(const CFloatValue* pLiteral)
	{
		return llvm::ConstantFP::get(llvm::Type::getFloatTy(*mContext), pLiteral->GetValue());
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitDoubleLiteral(const CDoubleValue* pLiteral)
	{
		return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*mContext), pLiteral->GetValue());
	}

	TLLVMIRData CLLVMLiteralVisitor::VititCharLiteral(const CCharValue* pLiteral)
	{
		I32 runeValue = 0x0;

		const std::string& value = pLiteral->GetValue();

		const U32 size = value.length();

		for (U8 i = 0; i < size; ++i)
		{
			runeValue |= static_cast<U8>(value[size - i - 1]) << 8 * i;
		}

		static llvm::Type* pInt32Type = llvm::Type::getInt32Ty(*mContext);

		return llvm::ConstantInt::get(pInt32Type, runeValue);
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitStringLiteral(const CStringValue* pLiteral)
	{
		llvm::IRBuilder<>* pCurrIRBuilder = mpCodeGenerator->GetCurrIRBuilder();

		auto pStringLiteral = pCurrIRBuilder->CreateGlobalString(pLiteral->GetValue());

		return llvm::ConstantExpr::getBitCast(pStringLiteral, llvm::Type::getInt8PtrTy(*mContext));
/*
		llvm::ConstantInt* pInitialIndex = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*mpContext), 0);
		auto  t = llvm::cast<llvm::PointerType>(pStringLiteral->getType()->getScalarType())->getElementType();
		t->dump();
		std::vector<llvm::Constant*> args
		{
			 pInitialIndex, 
			 pInitialIndex
		};

		auto pExpr = llvm::ConstantExpr::getGetElementPtr(pStringLiteral->getInitializer()->getType(), pStringLiteral->getInitializer(), args);
		pExpr->dump();
		return {};*/
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitBoolLiteral(const CBoolValue* pLiteral)
	{
		return pLiteral->GetValue() ? llvm::ConstantInt::getTrue(*mContext) : llvm::ConstantInt::getFalse(*mContext);
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitNullLiteral(const CPointerValue* pLiteral)
	{
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*mContext), 0);
		//return llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(*mpContext));
	}
}
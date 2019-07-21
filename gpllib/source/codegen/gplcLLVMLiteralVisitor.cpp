#include "codegen/gplcLLVMLiteralVisitor.h"
#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"


namespace gplc
{
	CLLVMLiteralVisitor::CLLVMLiteralVisitor(llvm::LLVMContext& context)
	{
		mpContext = &context;
	}

	CLLVMLiteralVisitor::~CLLVMLiteralVisitor()
	{
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitIntLiteral(const CIntValue* pLiteral)
	{
		// \todo replace first argument with deduced one
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*mpContext), pLiteral->GetValue(), true);
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitUIntLiteral(const CUIntValue* pLiteral)
	{
		// \todo replace first argument with deduced one
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*mpContext), pLiteral->GetValue(), false);
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitFloatLiteral(const CFloatValue* pLiteral)
	{
		return llvm::ConstantFP::get(llvm::Type::getFloatTy(*mpContext), pLiteral->GetValue());
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitDoubleLiteral(const CDoubleValue* pLiteral)
	{
		return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*mpContext), pLiteral->GetValue());
	}

	TLLVMIRData CLLVMLiteralVisitor::VititCharLiteral(const CCharValue* pLiteral)
	{
		// \todo implement char literal
		return {};
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitStringLiteral(const CStringValue* pLiteral)
	{
		// \todo implement string literal
		return {};
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitBoolLiteral(const CBoolValue* pLiteral)
	{
		return pLiteral->GetValue() ? llvm::ConstantInt::getTrue(*mpContext) : llvm::ConstantInt::getFalse(*mpContext);
	}

	TLLVMIRData CLLVMLiteralVisitor::VisitNullLiteral(const CNullLiteral* pLiteral)
	{
		return {};
	}
}
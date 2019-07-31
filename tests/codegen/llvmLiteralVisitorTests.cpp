#include <catch2/catch.hpp>
#include <gplc.h>
#include "llvm/IR/Constants.h"


using namespace gplc;

//
//TEST_CASE("CLLVMLiteralVisitor tests")
//{
//	llvm::LLVMContext context;
//
//	ILiteralVisitor<TLLVMIRData>* pLiteralVisitor = new CLLVMLiteralVisitor(context);
//
//	auto checkAsserts = [](auto pConstant, auto expectedValue)
//	{
//		REQUIRE(pConstant != nullptr);
//		REQUIRE(pConstant->getValue() == expectedValue);
//	};
//
//	SECTION("TestVisitIntLiteral_PassIntLiteral_ReturnsLLVMIRValue")
//	{
//		checkAsserts(llvm::dyn_cast<llvm::ConstantInt>(std::get<llvm::Value*>(pLiteralVisitor->VisitIntLiteral(new CIntValue(42)))), 42);
//
//		checkAsserts(llvm::dyn_cast<llvm::ConstantInt>(std::get<llvm::Value*>(pLiteralVisitor->VisitBoolLiteral(new CBoolValue(true)))), true);
//		checkAsserts(llvm::dyn_cast<llvm::ConstantInt>(std::get<llvm::Value*>(pLiteralVisitor->VisitBoolLiteral(new CBoolValue(false)))), false);
//	}
//
//	delete pLiteralVisitor;
//}
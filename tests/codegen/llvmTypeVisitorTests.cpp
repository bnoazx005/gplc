#include <catch2/catch.hpp>
#include <gplc.h>
#include "llvm/IR/Constants.h"


using namespace gplc;


TEST_CASE("CLLVMTypeVisitor tests")
{
	llvm::LLVMContext context;

	ITypeVisitor<TLLVMIRData>* pTypeVisitor = new CLLVMTypeVisitor(context);
	
	SECTION("TestVisitBasicType_PassTypeDef_ReturnsCorrectLLVMIRType")
	{
		REQUIRE(std::get<llvm::Type*>(pTypeVisitor->VisitBasicType(new CType(CT_INT8, BTS_INT32, 0x0)))->isIntegerTy(8));
		REQUIRE(std::get<llvm::Type*>(pTypeVisitor->VisitBasicType(new CType(CT_INT16, BTS_INT32, 0x0)))->isIntegerTy(16));
		REQUIRE(std::get<llvm::Type*>(pTypeVisitor->VisitBasicType(new CType(CT_INT32, BTS_INT32, 0x0)))->isIntegerTy(32));
		REQUIRE(std::get<llvm::Type*>(pTypeVisitor->VisitBasicType(new CType(CT_INT64, BTS_INT32, 0x0)))->isIntegerTy(64));

		REQUIRE(std::get<llvm::Type*>(pTypeVisitor->VisitBasicType(new CType(CT_FLOAT, BTS_FLOAT, 0x0)))->isFloatTy());
		REQUIRE(std::get<llvm::Type*>(pTypeVisitor->VisitBasicType(new CType(CT_DOUBLE, BTS_DOUBLE, 0x0)))->isDoubleTy());

		REQUIRE(std::get<llvm::Type*>(pTypeVisitor->VisitBasicType(new CType(CT_BOOL, BTS_BOOL, 0x0)))->isIntegerTy(1));

	}

	SECTION("TestVisitFunctionType_PassFunctionTypeDef_ReturnsCorrectLLVMIRType")
	{
		auto result = pTypeVisitor->VisitFunctionType(new CFunctionType({ new CType(CT_INT32, BTS_INT32, 0x0), new CType(CT_INT32, BTS_INT32, 0x0) }, new CType(CT_INT32, BTS_INT32, 0x0), 0x0));
	}

	delete pTypeVisitor;
}
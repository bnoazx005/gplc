/*!
	\author Ildar Kasimov
	\date   17.07.2019
	\copyright

	\brief The file contains definition of the generator of LLVM IR for types

	\todo
*/

#ifndef GPLC_LLVM_TYPE_VISITOR_H
#define GPLC_LLVM_TYPE_VISITOR_H


#include "common/gplcTypes.h"
#include "common/gplcVisitor.h"
#include "llvm/IR/Module.h"


namespace gplc
{
	/*!
		\brief The class implements a fucntionality of LLVM IR code generator
		for all types
	*/

	class CLLVMTypeVisitor : public ITypeVisitor<TLLVMIRData>
	{
		public:
			CLLVMTypeVisitor(llvm::LLVMContext& context);
			virtual ~CLLVMTypeVisitor();

			TLLVMIRData VisitBasicType(const CType* pType) override;
			TLLVMIRData VisitFunctionType(const CFunctionType* pFuncType) override;
			TLLVMIRData VisitStructType(const CStructType* pStructType) override;
			TLLVMIRData VisitNamedType(const CDependentNamedType* pNamedType) override;
			TLLVMIRData VisitEnumType(const CEnumType* pEnumType) override;
			TLLVMIRData VisitStaticSizedArray(const CArrayType* pArrayType) override; 
			TLLVMIRData VisitPointerType(const CPointerType* pPointerType) override;
		protected:
			CLLVMTypeVisitor() = default;
			CLLVMTypeVisitor(const CLLVMTypeVisitor& visitor) = default;
		protected:
			llvm::LLVMContext* mpContext;
	};
}

#endif
/*!
	\author Ildar Kasimov
	\date   16.07.2019
	\copyright

	\brief The file contains definition of the generator of LLVM IR for literals

	\todo
*/

#ifndef GPLC_LLVM_LITERAL_VISITOR_H
#define GPLC_LLVM_LITERAL_VISITOR_H


#include "common/gplcTypes.h"
#include "common/gplcVisitor.h"
#include "llvm/IR/Module.h"


namespace gplc
{
	class CLLVMCodeGenerator;


	/*!
		\brief The class implements a fucntionality of LLVM IR code generator
		for all literals
	*/

	class CLLVMLiteralVisitor : public ILiteralVisitor<TLLVMIRData>
	{
		public:
			CLLVMLiteralVisitor(llvm::LLVMContext& context, CLLVMCodeGenerator* pCodeGenerator);
			virtual ~CLLVMLiteralVisitor();

			TLLVMIRData VisitIntLiteral(const CIntValue* pLiteral) override;
			TLLVMIRData VisitUIntLiteral(const CUIntValue* pLiteral) override;
			TLLVMIRData VisitFloatLiteral(const CFloatValue* pLiteral) override;
			TLLVMIRData VisitDoubleLiteral(const CDoubleValue* pLiteral) override;
			TLLVMIRData VititCharLiteral(const CCharValue* pLiteral) override;
			TLLVMIRData VisitStringLiteral(const CStringValue* pLiteral) override;
			TLLVMIRData VisitBoolLiteral(const CBoolValue* pLiteral) override;
			TLLVMIRData VisitNullLiteral(const CPointerValue* pLiteral) override;
		protected:
			CLLVMLiteralVisitor() = default;
			CLLVMLiteralVisitor(const CLLVMLiteralVisitor& visitor) = default;
		protected:
			CLLVMCodeGenerator* mpCodeGenerator;

			llvm::LLVMContext*  mpContext;
	};
}

#endif
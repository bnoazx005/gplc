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
	/*!
		\brief The class implements a fucntionality of LLVM IR code generator
		for all literals
	*/

	class CLLVMLiteralVisitor : public ILiteralVisitor<TLLVMIRData>
	{
		public:
			CLLVMLiteralVisitor(llvm::LLVMContext& context);
			virtual ~CLLVMLiteralVisitor();

			TLLVMIRData VisitIntLiteral(const CIntLiteral* pLiteral) override;
			TLLVMIRData VisitUIntLiteral(const CUIntLiteral* pLiteral) override;
			TLLVMIRData VisitFloatLiteral(const CFloatLiteral* pLiteral) override;
			TLLVMIRData VisitDoubleLiteral(const CDoubleLiteral* pLiteral) override;
			TLLVMIRData VititCharLiteral(const CCharLiteral* pLiteral) override;
			TLLVMIRData VisitStringLiteral(const CStringLiteral* pLiteral) override;
			TLLVMIRData VisitBoolLiteral(const CBoolLiteral* pLiteral) override;
		protected:
			CLLVMLiteralVisitor() = default;
			CLLVMLiteralVisitor(const CLLVMLiteralVisitor& visitor) = default;
		protected:
			llvm::LLVMContext* mpContext;
	};
}

#endif
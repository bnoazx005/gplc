/*!
	\author Ildar Kasimov
	\date   17.07.2019
	\copyright

	\brief The file contains definition of the generator of C literals

	\todo
*/

#ifndef GPLC_C_LITERAL_VISITOR_H
#define GPLC_C_LITERAL_VISITOR_H


#include "common/gplcTypes.h"
#include "common/gplcVisitor.h"


namespace gplc
{
	class CCLiteralVisitor : public ILiteralVisitor<TLLVMIRData>
	{
		public:
			CCLiteralVisitor() = default;
			virtual ~CCLiteralVisitor() = default;

			TLLVMIRData VisitIntLiteral(const CIntLiteral* pLiteral) override;
			TLLVMIRData VisitUIntLiteral(const CUIntLiteral* pLiteral) override;
			TLLVMIRData VisitFloatLiteral(const CFloatLiteral* pLiteral) override;
			TLLVMIRData VisitDoubleLiteral(const CDoubleLiteral* pLiteral) override;
			TLLVMIRData VititCharLiteral(const CCharLiteral* pLiteral) override;
			TLLVMIRData VisitStringLiteral(const CStringLiteral* pLiteral) override;
			TLLVMIRData VisitBoolLiteral(const CBoolLiteral* pLiteral) override;
			TLLVMIRData VisitNullLiteral(const CNullLiteral* pLiteral) override;
		protected:
			CCLiteralVisitor(const CCLiteralVisitor& visitor) = default;
	};
}

#endif
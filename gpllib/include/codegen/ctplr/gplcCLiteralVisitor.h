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

			TLLVMIRData VisitIntLiteral(const CIntValue* pLiteral) override;
			TLLVMIRData VisitUIntLiteral(const CUIntValue* pLiteral) override;
			TLLVMIRData VisitFloatLiteral(const CFloatValue* pLiteral) override;
			TLLVMIRData VisitDoubleLiteral(const CDoubleValue* pLiteral) override;
			TLLVMIRData VititCharLiteral(const CCharValue* pLiteral) override;
			TLLVMIRData VisitStringLiteral(const CStringValue* pLiteral) override;
			TLLVMIRData VisitBoolLiteral(const CBoolValue* pLiteral) override;
			TLLVMIRData VisitNullLiteral(const CPointerValue* pLiteral) override;
		protected:
			CCLiteralVisitor(const CCLiteralVisitor& visitor) = default;
	};
}

#endif
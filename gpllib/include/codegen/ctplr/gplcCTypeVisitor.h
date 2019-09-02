/*!
	\author Ildar Kasimov
	\date   17.07.2019
	\copyright

	\brief The file contains definition of the generator of C type

	\todo
*/

#ifndef GPLC_C_TYPE_VISITOR_H
#define GPLC_C_TYPE_VISITOR_H


#include "common/gplcTypes.h"
#include "common/gplcVisitor.h"


namespace gplc
{
	class CCTypeVisitor : public ITypeVisitor<TLLVMIRData>
	{
		public:
			CCTypeVisitor() = default;
			virtual ~CCTypeVisitor() = default;

			TLLVMIRData VisitBasicType(const CType* pType) override;
			TLLVMIRData VisitFunctionType(const CFunctionType* pFuncType) override; 
			TLLVMIRData VisitStructType(const CStructType* pStructType) override;
			TLLVMIRData VisitNamedType(const CDependentNamedType* pNamedType) override;
			TLLVMIRData VisitEnumType(const CEnumType* pEnumType) override;
			TLLVMIRData VisitStaticSizedArray(const CArrayType* pArrayType) override;
			TLLVMIRData VisitPointerType(const CPointerType* pPointerType) override;
			TLLVMIRData VisitVariantType(const CVariantType* pVariantType) override;
		protected:
			CCTypeVisitor(const CCTypeVisitor& visitor) = default;
	};
}

#endif
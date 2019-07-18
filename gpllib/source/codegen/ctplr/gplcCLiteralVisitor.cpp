#include "codegen/ctplr/gplcCLiteralVisitor.h"
#include "common/gplcLiterals.h"
#include "common/gplcTypeSystem.h"


namespace gplc
{
	TLLVMIRData CCLiteralVisitor::VisitIntLiteral(const CIntLiteral* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitUIntLiteral(const CUIntLiteral* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitFloatLiteral(const CFloatLiteral* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitDoubleLiteral(const CDoubleLiteral* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VititCharLiteral(const CCharLiteral* pLiteral)
	{
		return pLiteral->GetValue();
	}

	TLLVMIRData CCLiteralVisitor::VisitStringLiteral(const CStringLiteral* pLiteral)
	{
		return pLiteral->GetValue();
	}

	TLLVMIRData CCLiteralVisitor::VisitBoolLiteral(const CBoolLiteral* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitNullLiteral(const CNullLiteral* pLiteral)
	{
		return "NULL";
	}
}
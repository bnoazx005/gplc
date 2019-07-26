#include "codegen/ctplr/gplcCLiteralVisitor.h"
#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"


namespace gplc
{
	TLLVMIRData CCLiteralVisitor::VisitIntLiteral(const CIntValue* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitUIntLiteral(const CUIntValue* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitFloatLiteral(const CFloatValue* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitDoubleLiteral(const CDoubleValue* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VititCharLiteral(const CCharValue* pLiteral)
	{
		return "\'" + pLiteral->GetValue() + "\'";
	}

	TLLVMIRData CCLiteralVisitor::VisitStringLiteral(const CStringValue* pLiteral)
	{
		return "\"" + pLiteral->GetValue() + "\"";
	}

	TLLVMIRData CCLiteralVisitor::VisitBoolLiteral(const CBoolValue* pLiteral)
	{
		return std::to_string(pLiteral->GetValue());
	}

	TLLVMIRData CCLiteralVisitor::VisitNullLiteral(const CPointerValue* pLiteral)
	{
		return "NULL";
	}
}
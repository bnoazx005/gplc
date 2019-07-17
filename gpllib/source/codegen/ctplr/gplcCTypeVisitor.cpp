#include "codegen/ctplr/gplcCTypeVisitor.h"
#include "common/gplcTypeSystem.h"
#include <vector>
#include <string>


namespace gplc
{
	TLLVMIRData CCTypeVisitor::VisitBasicType(const CType* pType)
	{
		switch (pType->GetType())
		{
			case CT_INT8:
				return "char";
			case CT_UINT8:
				return "unsigned char";
			case CT_INT16:
				return "short";
			case CT_UINT16:
				return "unsigned short";
			case CT_INT32:
				return "int";
			case CT_UINT32:
				return "unsigned int";
			case CT_INT64:
				return "long";
			case CT_UINT64:
				return "unsigned long";
			case CT_FLOAT:
				return "float";
			case CT_DOUBLE:
				return "double";
			case CT_STRING:
				return "char*";
			case CT_CHAR:
				return "char";
			case CT_BOOL:
				return "bool";
		}

		return "void*";
	}

	TLLVMIRData CCTypeVisitor::VisitFunctionType(const CFunctionType* pFuncType)
	{
		std::vector<std::string> args;

		auto pArgs = pFuncType->GetArgsTypes();

		for (auto pCurrArgType : pArgs)
		{
			args.push_back(std::get<std::string>(pCurrArgType->Accept(this)));
		}

		return "";
	}
}
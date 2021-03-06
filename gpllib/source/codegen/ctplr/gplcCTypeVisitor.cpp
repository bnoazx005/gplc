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
		std::string args;

		auto pArgs = pFuncType->GetArgsTypes();

		for (auto iter = pArgs.cbegin(); iter != pArgs.cend(); ++iter)
		{
			args.append(std::get<std::string>((*iter).second->Accept(this))).append(" ").append((*iter).first);

			if (iter + 1 < pArgs.cend())
			{
				args.append(", ");
			}
		}

		std::string result = std::get<std::string>(pFuncType->GetReturnValueType()->Accept(this));
												
		if ((pFuncType->GetAttributes() & AV_STATIC) != AV_STATIC)
		{
			result
				.append(" (*")
				.append(pFuncType->GetName())
				.append(")");
		}
		else
		{
			result.append(" ").append(pFuncType->GetName());
		}

		result.append("(").append(args).append(")");

		return result;
	}

	TLLVMIRData CCTypeVisitor::VisitStructType(const CStructType* pStructType)
	{
		std::string structBody;

		auto structFields = pStructType->GetFieldsTypes();

		for (U32 i = 0; i < structFields.size(); ++i)
		{
			structBody.append(std::get<std::string>(structFields[i].second->Accept(this))).append(" ").append(structFields[i].first).append(";\n");
		}

		return std::string("struct ")
					.append(pStructType->GetName())
					.append(" {\n")
					.append(structBody)
					.append("};\n");
	}

	TLLVMIRData CCTypeVisitor::VisitNamedType(const CDependentNamedType* pNamedType)
	{
		return pNamedType->GetName();
	}

	TLLVMIRData CCTypeVisitor::VisitEnumType(const CEnumType* pEnumType)
	{
		return {};
	}

	TLLVMIRData CCTypeVisitor::VisitStaticSizedArray(const CArrayType* pArrayType)
	{
		return {};
	}

	TLLVMIRData CCTypeVisitor::VisitPointerType(const CPointerType* pPointerType)
	{
		return {};
	}

	TLLVMIRData CCTypeVisitor::VisitVariantType(const CVariantType* pVariantType)
	{
		return {};
	}
}
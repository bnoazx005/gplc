#include "codegen/gplcLLVMTypeVisitor.h"
#include "common/gplcTypeSystem.h"
#include <vector>


namespace gplc
{
	CLLVMTypeVisitor::CLLVMTypeVisitor(llvm::LLVMContext& context):
		mContext(&context)
	{
	}

	CLLVMTypeVisitor::~CLLVMTypeVisitor()
	{
	}

	TLLVMIRData CLLVMTypeVisitor::VisitBasicType(const CType* pType)
	{
		switch (pType->GetType())
		{
			case CT_INT8:
			case CT_UINT8:
				return llvm::Type::getInt8Ty(*mContext);
			case CT_INT16:
			case CT_UINT16:
				return llvm::Type::getInt16Ty(*mContext);
			case CT_INT32:
			case CT_UINT32:
				return llvm::Type::getInt32Ty(*mContext);
			case CT_INT64:
			case CT_UINT64:
				return llvm::Type::getInt64Ty(*mContext);
			case CT_FLOAT:
				return llvm::Type::getFloatTy(*mContext);
			case CT_DOUBLE:
				return llvm::Type::getDoubleTy(*mContext);
			case CT_STRING:
				return llvm::Type::getInt8PtrTy(*mContext);
			case CT_CHAR:
				return llvm::StructType::get(llvm::Type::getInt32Ty(*mContext));
			case CT_BOOL:
				return llvm::Type::getInt1Ty(*mContext);
			case CT_VOID:
				return llvm::Type::getVoidTy(*mContext);
		}

		return {};
	}

	TLLVMIRData CLLVMTypeVisitor::VisitFunctionType(const CFunctionType* pFuncType)
	{
		std::vector<llvm::Type*> args;

		auto pArgs = pFuncType->GetArgsTypes();

		for (auto pCurrArgType : pArgs)
		{
			args.push_back(std::get<llvm::Type*>(pCurrArgType.second->Accept(this)));
		}
		
		auto pFunctionType = llvm::FunctionType::get(std::get<llvm::Type*>(pFuncType->GetReturnValueType()->Accept(this)), args, false);

		U32 attributes = pFuncType->GetAttributes();

		if (attributes & AV_STATIC || attributes & AV_NATIVE_FUNC)
		{
			return pFunctionType;
		}

		return llvm::PointerType::get(pFunctionType, 0); // \todo return a function poitner
	}

	TLLVMIRData CLLVMTypeVisitor::VisitStructType(const CStructType* pStructType)
	{
		std::vector<llvm::Type*> structFields;

		auto fields = pStructType->GetFieldsTypes();

		// define an opaque structure
		const std::string& structName = pStructType->GetName();

		llvm::StructType* pInferredType = nullptr;

		if (mTypesTable.find(structName) == mTypesTable.cend())
		{
			pInferredType = llvm::StructType::create(*mContext, pStructType->GetMangledName()); // use mangled name if format Module$Type

			mTypesTable[structName] = pInferredType;
		}
		else
		{
			pInferredType = llvm::dyn_cast<llvm::StructType>(mTypesTable[structName]);
		}

		for (auto currField : fields)
		{
			structFields.push_back(std::get<llvm::Type*>(currField.second->Accept(this)));
		}

		// if the structure was forwardly declared then define its body
		if (pInferredType->isOpaque())
		{
			pInferredType->setBody(structFields);
		}

		return pInferredType ? pInferredType : mTypesTable[structName];
	}

	TLLVMIRData CLLVMTypeVisitor::VisitNamedType(const CDependentNamedType* pNamedType)
	{
		assert(pNamedType && pNamedType->GetDependentType());

		switch (pNamedType->GetType())
		{
			case CT_STRUCT:
				{
					auto typeName = pNamedType->GetName();

					if (mTypesTable.find(typeName) != mTypesTable.cend())
					{
						return mTypesTable[typeName];
					}

					return pNamedType->GetDependentType()->Accept(this);
				}
			case CT_ENUM:
				return llvm::Type::getInt32Ty(*mContext);
		}

		return pNamedType->GetDependentType()->Accept(this);
	}

	TLLVMIRData CLLVMTypeVisitor::VisitEnumType(const CEnumType* pEnumType)
	{
		/*!
			\note an enumeration is converted into integer value
		*/

		return llvm::Type::getInt32Ty(*mContext);// llvm::StructType::create({ llvm::Type::getInt32Ty(*mpContext) }, pEnumType->GetName());
	}

	TLLVMIRData CLLVMTypeVisitor::VisitStaticSizedArray(const CArrayType* pArrayType)
	{
		std::string arrayTypeName = "StaticArray$" + pArrayType->GetBaseType()->ToShortAliasString();

		if (mTypesTable.find(arrayTypeName) != mTypesTable.cend())
		{
			return mTypesTable[arrayTypeName];
		}

		// \note fat static array is a structure that consists of an array pointer and a field that stores its size
		auto pArrayFatType = llvm::StructType::create(*mContext,
								{
									llvm::ArrayType::get(std::get<llvm::Type*>(pArrayType->GetBaseType()->Accept(this)), pArrayType->GetElementsCount()),
									llvm::Type::getInt64Ty(*mContext)
								}, arrayTypeName);

		mTypesTable[arrayTypeName] = pArrayFatType;

		return pArrayFatType;
	}

	TLLVMIRData CLLVMTypeVisitor::VisitPointerType(const CPointerType* pPointerType)
	{
		return llvm::PointerType::get(std::get<llvm::Type*>(pPointerType->GetBaseType()->Accept(this)), 0);
	}

	TLLVMIRData CLLVMTypeVisitor::VisitVariantType(const CVariantType* pVariantType)
	{
		const std::string& variantName = pVariantType->GetName();

		if (mTypesTable.find(variantName) != mTypesTable.cend())
		{
			return mTypesTable[variantName];
		}

		std::vector<llvm::Type*> altTypes;

		for (auto pCurrAltType : pVariantType->GetFieldsTypes())
		{
			altTypes.push_back(std::get<llvm::Type*>(pCurrAltType->Accept(this)));
		}

		auto pTaggedUnionType = llvm::StructType::create(*mContext, 
														{
															llvm::Type::getInt16Ty(*mContext), ///< tag
															llvm::StructType::create(*mContext, altTypes, "", true) ///< actual data
														}, variantName);

		mTypesTable[variantName] = pTaggedUnionType;

		return pTaggedUnionType;
	}
}
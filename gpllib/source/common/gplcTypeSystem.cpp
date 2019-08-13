/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains types defenition

	\todo
*/

#include "common/gplcTypeSystem.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcValues.h"
#include "common/gplcSymTable.h"
#include "common/gplcConstExprInterpreter.h"
#include "parser/gplcASTNodesFactory.h"
#include "common/gplcTypesFactory.h"
#include "utils/CResult.h"
#include <algorithm>


namespace gplc
{
	E_NODE_TYPE NodeToCompilerType(E_COMPILER_TYPES nodeType)
	{
		switch (nodeType)
		{
			case CT_INT8:
				return NT_INT8;
			case CT_INT16:
				return NT_INT16;
			case CT_INT32:
				return NT_INT32;
			case CT_INT64:
				return NT_INT64;
			case CT_UINT8:
				return NT_UINT8;
			case CT_UINT16:
				return NT_UINT16;
			case CT_UINT32:
				return NT_UINT32;
			case CT_UINT64:
				return NT_UINT64;
			case CT_FLOAT:
				return NT_FLOAT;
			case CT_DOUBLE:
				return NT_DOUBLE;
			case CT_STRING:
				return NT_STRING;
			case CT_CHAR:
				return NT_CHAR;
			case CT_BOOL:
				return NT_BOOL;
		}

		return NT_VOID;
	}

	/*!
		\brief CTypeResolver's definition
	*/

	Result CTypeResolver::Init(ISymTable* pSymTable, IConstExprInterpreter* pInterpreter, ITypesFactory* pTypesFactory)
	{
		if (!pSymTable || !pInterpreter || !pTypesFactory)
		{
			return RV_FAIL;
		}

		mpSymTable             = pSymTable;
		mpConstExprInterpreter = pInterpreter;
		mpTypesFactory         = pTypesFactory;

		return RV_SUCCESS;
	}

	CType* CTypeResolver::Resolve(CASTTypeNode* pTypeNode)
	{
		return pTypeNode->Resolve(this);
	}

	CType* CTypeResolver::VisitBaseNode(CASTTypeNode* pNode)
	{
		return _deduceBuiltinType(pNode->GetType());
	}

	CType* CTypeResolver::VisitIdentifier(CASTIdentifierNode* pNode)
	{
		const std::string& identifier = pNode->GetName();

		const TSymbolDesc* pSymbolDesc = mpSymTable->LookUp(identifier);
		auto pSymbolEntryDesc = mpSymTable->LookUpNamedScope(identifier);

		return pSymbolDesc ? pSymbolDesc->mpType : (pSymbolEntryDesc ? pSymbolEntryDesc->mpType : nullptr);
	}

	CType* CTypeResolver::VisitLiteral(CASTLiteralNode* pNode)
	{
		return pNode->GetValue()->GetTypeInfo(mpTypesFactory, mpSymTable);
	}

	CType* CTypeResolver::VisitUnaryExpression(CASTUnaryExpressionNode* pNode)
	{
		auto pOperandNode = pNode->GetData();

		CType* pBaseType = pOperandNode ? dynamic_cast<CASTTypeNode*>(pOperandNode)->Resolve(this) : nullptr;

		switch (pNode->GetOpType())
		{
			case TT_AMPERSAND:
				return mpTypesFactory->CreatePointerType(pBaseType, mpSymTable->GetCurrentScopeType());
			case TT_STAR:
				{
					// the type above should be a pointer
					auto pPointerBaseType = dynamic_cast<CPointerType*>(pBaseType);

					return pPointerBaseType ? pPointerBaseType->GetBaseType() : nullptr;
				}
		}

		return pBaseType;
	}

	CType* CTypeResolver::VisitBinaryExpression(CASTBinaryExpressionNode* pNode)
	{
		auto pLeftExprNode = pNode->GetLeft();
		auto pRightExprNode = pNode->GetRight();

		CType* pLeftExprTypeInfo = pLeftExprNode->Resolve(this);
		CType* pRightExprTypeInfo = pRightExprNode->Resolve(this);

		return _deduceExprType(pNode->GetOpType(), pLeftExprTypeInfo->GetType(), pRightExprTypeInfo->GetType());
	}

	CType* CTypeResolver::VisitDeclaration(CASTDeclarationNode* pNode)
	{
		return pNode->GetTypeInfo()->Resolve(this);
	}
	
	CType* CTypeResolver::VisitDefinition(CASTDefinitionNode* pNode)
	{
		return pNode->GetDeclaration()->Resolve(this);
	}

	CType* CTypeResolver::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode)
	{
		auto args = pNode->GetArgs()->GetChildren();

		CFunctionType::TArgsArray argsTypes;

		CASTIdentifierNode* pCurrArgDecl = nullptr;

		for (auto pCurrArgNode : args)
		{
			pCurrArgDecl = dynamic_cast<CASTIdentifierNode*>((dynamic_cast<CASTDeclarationNode*>(pCurrArgNode))->GetIdentifiers()->GetChildren()[0]);
			
			auto pCurrArgType = Resolve(dynamic_cast<CASTTypeNode*>(pCurrArgNode));

			pCurrArgType->SetAttribute(pCurrArgDecl->GetAttributes());

			argsTypes.push_back({ pCurrArgDecl->GetName(), pCurrArgType });
		}

		return new CFunctionType(argsTypes, Resolve(dynamic_cast<CASTTypeNode*>(pNode->GetReturnValueType())), 0x0, mpSymTable->GetCurrentScopeType());
	}

	CType* CTypeResolver::VisitFunctionCall(CASTFunctionCallNode* pNode)
	{
		CASTIdentifierNode* pFuncIdentifierNode = dynamic_cast<CASTIdentifierNode*>(pNode->GetIdentifier()->GetData());

		const TSymbolDesc* pFunctionSymbolDesc = mpSymTable->LookUp(pFuncIdentifierNode->GetName());

		if (!pFunctionSymbolDesc)
		{
			return nullptr;
		}

		CFunctionType* pFuncTypeInfo = dynamic_cast<CFunctionType*>(pFunctionSymbolDesc->mpType);

		return pFuncTypeInfo->GetReturnValueType();
	}

	CType* CTypeResolver::VisitStructDeclaration(CASTStructDeclNode* pNode)
	{
		CStructType::TFieldsArray fieldsTypes;

		auto pStructBody = pNode->GetFieldsDeclarations();

		CStructType* pStructType = mpTypesFactory->CreateStructType({}, AV_AGGREGATE_TYPE, mpSymTable->GetCurrentScopeType());

		CType* pFieldType = nullptr;

		CASTNode* pIdentifiers = nullptr;

		for (auto pCurrField : pStructBody->GetStatements())
		{
			pFieldType = Resolve(dynamic_cast<CASTTypeNode*>(pCurrField));
			
			pIdentifiers = (pCurrField->GetType() == NT_DECL) ? 
										dynamic_cast<CASTDeclarationNode*>(pCurrField)->GetIdentifiers() :
										dynamic_cast<CASTDefinitionNode*>(pCurrField)->GetDeclaration()->GetIdentifiers();

			for (auto pCurrIdentifier : pIdentifiers->GetChildren())
			{
				pStructType->AddField(dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName(), pFieldType);
			}
		}

		return pStructType;
	}

	CType* CTypeResolver::VisitNamedType(CASTNamedTypeNode* pNode)
	{
		return mpTypesFactory->CreateDependentNamedType(mpSymTable, pNode->GetTypeInfo()->GetName(), mpSymTable->GetCurrentScopeType());
	}

	CType* CTypeResolver::VisitArrayType(CASTArrayTypeNode* pNode)
	{
		if (!mpConstExprInterpreter)
		{
			return nullptr;
		}

		TResult<U32> evaluatedArraySize = mpConstExprInterpreter->Eval(pNode->GetSizeExpr(), mpSymTable);

		if (evaluatedArraySize.HasError())
		{
			return nullptr;
		}

		return mpTypesFactory->CreateArrayType(Resolve(dynamic_cast<CASTTypeNode*>(pNode->GetTypeInfo())), evaluatedArraySize.Get(), AV_AGGREGATE_TYPE, mpSymTable->GetCurrentScopeType());
	}

	CType* CTypeResolver::VisitAccessOperator(CASTAccessOperatorNode* pNode)
	{
		CType* pExprType = pNode->GetExpression()->Resolve(this);

		if (!pExprType)
		{
			return nullptr;
		}

		// \note for now we suppose that right part after '.' is an identifier
		const std::string& identifierName = dynamic_cast<CASTIdentifierNode*>(dynamic_cast<CASTUnaryExpressionNode*>(pNode->GetMemberName())->GetData())->GetName();

		switch (pExprType->GetType())
		{
			case CT_ENUM:
				return pExprType;
			case CT_STRUCT:
				return mpSymTable->LookUp(mpSymTable->LookUpNamedScope(pExprType->GetName())->mVariables[identifierName])->mpType;
			case CT_MODULE:
				{
					mpSymTable->VisitNamedScope(pExprType->GetName());

					auto pVarDesc   = mpSymTable->LookUp(identifierName);
					auto pTypeEntry = mpSymTable->LookUpNamedScope(identifierName);

					mpSymTable->LeaveScope();

					return pVarDesc ? pVarDesc->mpType : pTypeEntry->mpType;
				}
		}

		return nullptr;
	}

	CType* CTypeResolver::VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode)
	{
		CType* pExprType = pNode->GetExpression()->Resolve(this);

		if (!pExprType)
		{
			return nullptr;
		}

		CArrayType* pArrayType = dynamic_cast<CArrayType*>(pExprType);

		return pArrayType->GetBaseType();
	}

	CType* CTypeResolver::VisitPointerType(CASTPointerTypeNode* pNode)
	{
		return mpTypesFactory->CreatePointerType(Resolve(dynamic_cast<CASTTypeNode*>(pNode->GetTypeInfo())), mpSymTable->GetCurrentScopeType());
	}

	CType* CTypeResolver::VisitModuleType(CASTImportDirectiveNode* pNode)
	{
		return mpTypesFactory->CreateModuleType(pNode->GetImportedModuleName(), 0x0, mpSymTable->GetCurrentScopeType());
	}

	CType* CTypeResolver::_deduceBuiltinType(E_NODE_TYPE type, U32 attributes)
	{
		switch (type)
		{
			case NT_INT8:
				return mpTypesFactory->CreateType(CT_INT8, BTS_INT8, attributes, "", mpSymTable->GetCurrentScopeType());
			case NT_INT16:
				return mpTypesFactory->CreateType(CT_INT16,  BTS_INT16,  attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_INT32:
				return mpTypesFactory->CreateType(CT_INT32, BTS_INT32, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_INT64:
				return mpTypesFactory->CreateType(CT_INT64, BTS_INT64, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_UINT8:
				return mpTypesFactory->CreateType(CT_UINT8, BTS_UINT8, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_UINT16:
				return mpTypesFactory->CreateType(CT_UINT16, BTS_UINT16, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_UINT32:
				return mpTypesFactory->CreateType(CT_UINT32, BTS_UINT32, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_UINT64:
				return mpTypesFactory->CreateType(CT_UINT64, BTS_UINT64, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_FLOAT:
				return mpTypesFactory->CreateType(CT_FLOAT, BTS_FLOAT, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_DOUBLE:
				return mpTypesFactory->CreateType(CT_DOUBLE, BTS_DOUBLE, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_STRING:
				return mpTypesFactory->CreateType(CT_STRING, BTS_POINTER, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_CHAR:
				return mpTypesFactory->CreateType(CT_CHAR, BTS_CHAR, attributes , "", mpSymTable->GetCurrentScopeType());
			case NT_BOOL:
				return mpTypesFactory->CreateType(CT_BOOL, BTS_BOOL, attributes , "", mpSymTable->GetCurrentScopeType());
		}

		return nullptr;
	}

	CType* CTypeResolver::_deduceExprType(E_TOKEN_TYPE opType, E_COMPILER_TYPES leftType, E_COMPILER_TYPES rightType)
	{
		// here the type inference table is placed
		switch (opType)
		{
			case TT_PLUS:
			case TT_MINUS:
			case TT_STAR:
			case TT_SLASH:
				if (leftType == rightType)
				{
					return _deduceBuiltinType(NodeToCompilerType(leftType));
				}

				// compiler-time error
				return nullptr;
			case TT_LE:
			case TT_LT:
			case TT_GE:
			case TT_GT:
			case TT_EQ:
			case TT_NE:
				if (leftType == rightType)
				{
					return _deduceBuiltinType(NT_BOOL);
				}

				return nullptr;
		}

		return nullptr;
	}


	/*!
		CType definition
	*/

	CType::TCastMap CType::mCastMap
	{
		{ CT_INT8, { { CT_INT16, true }, { CT_INT32, true }, { CT_INT64, true },  {CT_UINT8, true}, { CT_UINT16, true }, { CT_UINT32, true }, { CT_UINT64, true } } },
		{ CT_INT16, { { CT_INT8, true }, { CT_INT32, true }, { CT_INT64, true },  {CT_UINT8, true}, { CT_UINT16, true }, { CT_UINT32, true }, { CT_UINT64, true } } },
		{ CT_INT32, { { CT_INT8, true }, { CT_INT16, true }, { CT_INT64, true },  {CT_UINT8, true}, { CT_UINT16, true }, { CT_UINT32, true }, { CT_UINT64, true } } },
		{ CT_INT64, { { CT_INT16, true }, { CT_INT32, true }, { CT_INT8, true },  {CT_UINT8, true}, { CT_UINT16, true }, { CT_UINT32, true }, { CT_UINT64, true } } },
		{ CT_UINT8, { { CT_INT16, true }, { CT_INT32, true }, { CT_INT64, true },  {CT_INT8, true}, { CT_UINT16, true }, { CT_UINT32, true }, { CT_UINT64, true } } },
		{ CT_UINT16, { { CT_INT16, true }, { CT_INT32, true }, { CT_INT64, true },  {CT_UINT8, true}, { CT_INT16, true }, { CT_UINT32, true }, { CT_UINT64, true } } },
		{ CT_UINT32, { { CT_INT16, true }, { CT_INT32, true }, { CT_INT64, true },  {CT_UINT8, true}, { CT_UINT16, true }, { CT_INT32, true }, { CT_UINT64, true } } },
		{ CT_UINT64, { { CT_INT16, true }, { CT_INT32, true }, { CT_INT64, true },  {CT_UINT8, true}, { CT_UINT16, true }, { CT_UINT32, true }, { CT_INT64, true } } },
		{ CT_FLOAT, { { CT_DOUBLE, true } } },
		{ CT_DOUBLE, { { CT_FLOAT, true } } },
		{ CT_CHAR, { { CT_STRING, true } } },
	};

	CType::CType() :
		mType(CT_INT32), mSize(4), mAttributes(0x0), mName(ToShortAliasString())
	{
	}

	CType::CType(const CType& type) :
		mType(type.mType), mSize(type.mSize), mAttributes(type.mAttributes)
	{
	}

	CType::CType(E_COMPILER_TYPES type, U32 size, U32 attributes, const std::string& name, CType* pParent):
		mType(type), mSize(size), mAttributes(attributes), mName(name), mpParent(pParent)
	{
	}

	CType::~CType()
	{
	}

	TLLVMIRData CType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitBasicType(this);
	}

	bool CType::IsBuiltIn() const
	{
		return mType != CT_ENUM && mType != CT_ARRAY && mType != CT_FUNCTION && mType != CT_STRUCT;
	}

	void CType::SetAttribute(U32 attribute)
	{
		mAttributes |= attribute;
	}

	void CType::SetName(const std::string& name)
	{
		mName = name;
	}

	const std::string& CType::GetName() const
	{
		return mName;
	}

	std::string CType::GetMangledName() const
	{
		CType* pCurrScopeType = mpParent;

		std::string mangledName = mName;
		
		do
		{
			mangledName = pCurrScopeType->GetName() + "$" + mangledName;

			pCurrScopeType = pCurrScopeType->GetParent();
		} 
		while (pCurrScopeType && pCurrScopeType->GetType() != CT_MODULE);

		return mangledName;
	}

	const std::vector<const CType*> CType::GetChildTypes() const
	{
		return mChildren;
	}

	E_COMPILER_TYPES CType::GetType() const
	{
		return mType;
	}

	U32 CType::GetChildTypesCount() const
	{
		return mChildren.size();
	}

	U32 CType::GetSize() const
	{
		return mSize;
	}

	U32 CType::GetAttributes() const
	{
		return mAttributes;
	}
	
	CASTExpressionNode* CType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		if (IsBuiltIn())
		{
			return pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(_getBuiltinTypeDefaultValue(mType)));
		}

		return nullptr;
	}

	CType* CType::GetParent() const
	{
		return mpParent;
	}

	bool CType::AreSame(const CType* pType) const
	{
		if (!pType)
		{
			return false;
		}

		U32 lattr = mAttributes & SignificantAttributesMask;
		U32 rattr = pType->mAttributes & SignificantAttributesMask;

		return (mType == pType->mType) && (mSize == pType->mSize) && (lattr == rattr);
	}

	bool CType::AreConvertibleTo(const CType* pType) const
	{
		if (!pType)
		{
			return false;
		}

		if (IsBuiltIn() && pType->mType == CT_STRING)
		{
			return true; // \note each built-in type can be converted to a string literal automatically
		}

		return mCastMap[mType][pType->mType];
	}

	std::string CType::ToShortAliasString() const
	{
		switch (mType)
		{
			case CT_INT8:
				return "i8";
			case CT_INT16:
				return "i16";
			case CT_INT32:
				return "i32";
			case CT_INT64:
				return "i64";
			case CT_UINT8:
				return "u8";
			case CT_UINT16:
				return "u16";
			case CT_UINT32:
				return "u32";
			case CT_UINT64:
				return "u64";
			case CT_FLOAT:
				return "f32";
			case CT_DOUBLE:
				return "f64";
			case CT_STRING:
				return "str";
			case CT_CHAR:
				return "c8";
			case CT_BOOL:
				return "b";
		}

		return "unknown";
	}

	Result CType::_addChildTypeDesc(const CType* type)
	{
		if (type == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}

		mChildren.push_back(type);

		return RV_SUCCESS;
	}

	Result CType::_removeChildTypeDesc(CType** type)
	{
		if (type == nullptr)
		{
			return RV_INVALID_ARGUMENTS;
		}

		std::vector<const CType*>::iterator currElement = std::find(mChildren.begin(), mChildren.end(), *type);

		if (currElement == mChildren.end())
		{
			return RV_FAIL;
		}

		mChildren.erase(currElement);

		delete *currElement;

		*currElement = nullptr;

		return RV_SUCCESS;
	}

	CBaseValue* CType::_getBuiltinTypeDefaultValue(E_COMPILER_TYPES type) const
	{
		/*
			\todo list of unimplemented types
			CT_VOID,
				CT_POINTER,
				CT_STRUCT,
				CT_FUNCTION,
				CT_ENUM,
				CT_ARRAY,
		*/

		switch (type)
		{
			case CT_INT8:
			case CT_INT16:
			case CT_INT32:
			case CT_INT64:
				return new CIntValue(0);

			case CT_UINT8:
			case CT_UINT16:
			case CT_UINT32:
			case CT_UINT64:
				return new CUIntValue(0);

			case CT_FLOAT:
				return new CFloatValue(0.0f);

			case CT_DOUBLE:
				return new CDoubleValue(0.0);

			case CT_CHAR:
				return new CCharValue("\0");

			case CT_STRING:
				return new CStringValue("");

			case CT_BOOL:
				return new CBoolValue(true);
		}

		return nullptr; ///< unknown type
	}


	/*!
		\brief CPointerType's definition
	*/

	CPointerType::CPointerType(CType* pType, CType* pParent):
		CType(CT_POINTER, BTS_POINTER, AV_POINTER, "", pParent), mpBaseType(pType)
	{
		mName = (mpBaseType ? mpBaseType->GetName() : "void") + "*";
	}

	CPointerType::~CPointerType()
	{
	}

	TLLVMIRData CPointerType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitPointerType(this);
	}

	CASTExpressionNode* CPointerType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		// \todo temprorary solution, reimplement this later with CPointerValue type
		return pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0)));
		//return pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CPointerValue()));
	}

	bool CPointerType::IsBuiltIn() const
	{
		return mpBaseType ? mpBaseType->IsBuiltIn() : false;
	}

	bool CPointerType::AreSame(const CType* pType) const
	{
		E_COMPILER_TYPES otherType = pType->GetType();

		if (otherType != CT_POINTER && otherType != CT_ARRAY)
		{
			return false;
		}

		CType* pBaseType = nullptr;

		switch (otherType)
		{
			case CT_POINTER:
				pBaseType = dynamic_cast<const CPointerType*>(pType)->GetBaseType();

				if (!pBaseType)
				{
					return true;
				}
				break;
			case CT_ARRAY:
				pBaseType = dynamic_cast<const CArrayType*>(pType)->GetBaseType();
				break;
		}

		return mpBaseType->AreSame(pBaseType);
	}

	std::string CPointerType::ToShortAliasString() const
	{
		return "ptr_" + mpBaseType->ToShortAliasString();
	}

	CType* CPointerType::GetBaseType() const
	{
		return mpBaseType;
	}


	/*!
		CStructType's definition
	*/

	CStructType::CStructType(const TFieldsArray& fieldsTypes, U32 attributes, CType* pParent):
		CType(CT_STRUCT, BTS_POINTER, attributes, "", pParent)
	{
		std::copy(fieldsTypes.begin(), fieldsTypes.end(), std::back_inserter(mFieldsTypes));
	}

	void CStructType::AddField(const std::string& fieldName, CType* pFieldType)
	{
		mFieldsTypes.push_back({ fieldName, pFieldType });
	}

	TLLVMIRData CStructType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitStructType(this);
	}

	void CStructType::SetAttributes(U32 attributes)
	{
		mAttributes = attributes;
	}

	const CStructType::TFieldsArray& CStructType::GetFieldsTypes() const
	{
		return mFieldsTypes;
	}

	CASTExpressionNode* CStructType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		return pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0)));
	}

	bool CStructType::AreSame(const CType* pType) const
	{
		if (!pType || pType->GetType() != CT_STRUCT)
		{
			return false;
		}

		const CStructType* pStructType = dynamic_cast<const CStructType*>(pType);

		auto& thisFields  = mFieldsTypes;
		auto& otherFields = pStructType->mFieldsTypes;

		if (thisFields.size() != otherFields.size())
		{
			return false;
		}

		for (U32 i = 0; i < thisFields.size(); ++i)
		{
			if (!thisFields[i].second->AreSame(otherFields[i].second))
			{
				return false;
			}
		}

		return true;
	}

	std::string CStructType::ToShortAliasString() const
	{
		return "struct";
	}

	
	/*!
		CFunctionType's definition
	*/

	CFunctionType::CFunctionType(const TArgsArray& argsTypes, CType* pReturnValueType, U32 attributes, CType* pParent):
		CType(CT_FUNCTION, CT_POINTER, attributes, "", pParent), mpReturnValueType(pReturnValueType)
	{
		std::copy(argsTypes.begin(), argsTypes.end(), std::back_inserter(mArgsTypes));
	}

	TLLVMIRData CFunctionType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFunctionType(this);
	}

	void CFunctionType::SetAttributes(U32 attributes)
	{
		mAttributes = attributes;
	}

	const CFunctionType::TArgsArray& CFunctionType::GetArgsTypes() const
	{
		return mArgsTypes;
	}

	CType* CFunctionType::GetReturnValueType() const
	{
		return mpReturnValueType;
	}

	CASTExpressionNode* CFunctionType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		return pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CPointerValue()));
	}

	bool CFunctionType::AreSame(const CType* pType) const
	{
		E_COMPILER_TYPES type = pType->GetType();

		if (type != CT_FUNCTION)
		{
			return false;
		}

		const CFunctionType* pFunctionType = dynamic_cast<const CFunctionType*>(pType);

		auto pArgsTypes = pFunctionType->GetArgsTypes();

		// different arities
		if (pArgsTypes.size() != mArgsTypes.size())
		{
			return false;
		}

		CType* pCurrFuncArg  = nullptr;
		CType* pOtherFuncArg = nullptr;

		for (I32 i = 0; i < pArgsTypes.size(); ++i)
		{
			pOtherFuncArg = pFunctionType->mArgsTypes[i].second;
			pCurrFuncArg  = mArgsTypes[i].second;

			if (!pOtherFuncArg || !pCurrFuncArg || !pOtherFuncArg->AreSame(pCurrFuncArg))
			{
				return false;
			}
		}

		return mpReturnValueType->AreSame(pFunctionType->GetReturnValueType());
	}

	std::string CFunctionType::ToShortAliasString() const
	{
		// \todo implement the method
		return "fnPtr";
	}


	CEnumType::CEnumType(const ISymTable* pSymTable, const std::string& enumName, CType* pParent):
		CType(CT_ENUM, BTS_INT32, 0x0, enumName, pParent), mpSymTable(pSymTable)
	{
		mChildren.push_back(nullptr); // \note this is a trick to make IsBuiltin work correct for this type
	}

	TLLVMIRData CEnumType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitEnumType(this);
	}

	CASTExpressionNode* CEnumType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		auto pEnumDesc = mpSymTable->LookUpNamedScope(mName);
		
		// if the enumeration doesn't contain any enumerator return 0
		if (pEnumDesc->mVariables.empty())
		{
			return pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0)));
		}

		// return value of a first enumerator
		return mpSymTable->LookUp(pEnumDesc->mVariables.begin()->second)->mpValue;
	}
	
	bool CEnumType::AreSame(const CType* pType) const
	{
		const CEnumType* pEnumType = dynamic_cast<const CEnumType*>(pType);

		return pEnumType && (pType->GetType() == mType) && (pEnumType->mName == mName);
	}

	std::string CEnumType::ToShortAliasString() const
	{
		return "enum";
	}


	/*!
		\brief CDependentNamedType's definition
	*/

	CDependentNamedType::CDependentNamedType(const ISymTable* pSymTable, const std::string& typeIdentifier, CType* pParent):
		CType(CT_ALIAS, BTS_UNKNOWN, 0x0, typeIdentifier, pParent), mpSymTable(pSymTable), mpDependentType(nullptr)
	{
	}

	TLLVMIRData CDependentNamedType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitNamedType(this);
	}

	bool CDependentNamedType::IsBuiltIn() const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->IsBuiltIn();
	}

	CASTExpressionNode* CDependentNamedType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->GetDefaultValue(pNodesFactory);
	}

	bool CDependentNamedType::AreSame(const CType* pType) const
	{
		// \note the simple case when dependent type can be same with another one is same names of both of them
		if (mName == pType->GetName())
		{
			return true;
		}

		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->AreSame(pType);
	}

	std::string CDependentNamedType::ToShortAliasString() const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->ToShortAliasString();
	}

	E_COMPILER_TYPES CDependentNamedType::GetType() const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->GetType();
	}

	U32 CDependentNamedType::GetSize() const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->GetSize();
	}

	CType* CDependentNamedType::GetDependentType() const
	{
		return mpSymTable->LookUpNamedScope(mName)->mpType;
	}

	U32 CDependentNamedType::GetAttributes() const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->GetAttributes();
	}

	std::string CDependentNamedType::GetMangledName() const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->GetMangledName();
	}

	
	/*!
		\brief CArrayType's definition
	*/

	CArrayType::CArrayType(CType* pBaseType, U32 elementsCount, U32 attribute, CType* pParent):
		CType(CT_ARRAY, BTS_POINTER, attribute, "", pParent), mElementsCount(elementsCount), mpBaseType(pBaseType)
	{
	}

	CArrayType::~CArrayType()
	{
	}

	TLLVMIRData CArrayType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitStaticSizedArray(this);
	}
	
	void CArrayType::SetAttributes(U32 attributes)
	{
		mAttributes = attributes;
	}

	CASTExpressionNode* CArrayType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		return pNodesFactory->CreateUnaryExpr(TT_DEFAULT, pNodesFactory->CreateLiteralNode(new CIntValue(0)));
	}

	bool CArrayType::AreSame(const CType* pType) const
	{
		const CArrayType* pArrayType = dynamic_cast<const CArrayType*>(pType);

		return pArrayType && mType == pArrayType->mType;
	}

	std::string CArrayType::ToShortAliasString() const
	{
		return "array";
	}

	CType* CArrayType::GetBaseType() const
	{
		return mpBaseType;
	}

	U32 CArrayType::GetElementsCount() const
	{
		return mElementsCount;
	}


	/*!
		\brief CModuleType's definition
	*/
	
	CModuleType::CModuleType(const std::string& moduleName, U32 attributes, CType* pParent):
		CType(CT_MODULE, BTS_UNKNOWN, attributes, moduleName, pParent)
	{
	}

	TLLVMIRData CModuleType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	CASTExpressionNode* CModuleType::GetDefaultValue(IASTNodesFactory* pNodesFactory) const
	{
		return nullptr;
	}

	bool CModuleType::AreSame(const CType* pType) const
	{
		if (pType->GetType() != CT_MODULE)
		{
			return false;
		}

		return mName == pType->GetName();
	}

	std::string CModuleType::ToShortAliasString() const
	{
		return "module";
	}

	std::string CModuleType::GetMangledName() const
	{
		return mName + "$";
	}


	std::string CreateAnonymousLambdaName(const CFunctionType* pLambdaType)
	{
		std::string name = std::string("lambda").append(pLambdaType->GetReturnValueType()->ToShortAliasString());

		for (auto pCurrArgType : pLambdaType->GetArgsTypes())
		{
			name.append(pCurrArgType.second->ToShortAliasString());
		}

		// random salt
		return name.append("_").append(std::to_string(rand()));
	}
}
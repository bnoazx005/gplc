/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains types defenition

	\todo
*/

#include "common\gplcTypeSystem.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcValues.h"
#include "common/gplcSymTable.h"
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
	}

	/*!
		\brief CTypeResolver's definition
	*/

	CType* CTypeResolver::Resolve(CASTTypeNode* pTypeNode, ISymTable* pSymTable)
	{
		if (!pSymTable)
		{
			return nullptr;
		}

		mpSymTable = pSymTable;

		return pTypeNode->Resolve(this, pSymTable);
	}

	CType* CTypeResolver::VisitBaseNode(CASTTypeNode* pNode)
	{
		E_NODE_TYPE basicType = (pNode->GetType() == NT_POINTER) ? pNode->GetChildren()[0]->GetType() : pNode->GetType();

		return _deduceBuiltinType(basicType, (pNode->GetType() == NT_POINTER) ? AV_POINTER : 0x0);
	}

	CType* CTypeResolver::VisitIdentifier(CASTIdentifierNode* pNode)
	{
		const TSymbolDesc* pSymbolDesc = mpSymTable->LookUp(pNode->GetName());

		return pSymbolDesc ? pSymbolDesc->mpType : nullptr;
	}

	CType* CTypeResolver::VisitLiteral(CASTLiteralNode* pNode)
	{
		return pNode->GetValue()->GetTypeInfo();
	}

	CType* CTypeResolver::VisitUnaryExpression(CASTUnaryExpressionNode* pNode)
	{
		auto pOperandNode = pNode->GetData();

		return pOperandNode ? dynamic_cast<CASTTypeNode*>(pOperandNode)->Resolve(this, mpSymTable) : nullptr;
	}
	
	CType* CTypeResolver::VisitBinaryExpression(CASTBinaryExpressionNode* pNode)
	{
		auto pLeftExprNode  = pNode->GetLeft();
		auto pRightExprNode = pNode->GetRight();

		CType* pLeftExprTypeInfo  = pLeftExprNode->Resolve(this, mpSymTable);
		CType* pRightExprTypeInfo = pRightExprNode->Resolve(this, mpSymTable);

		return _deduceExprType(pNode->GetOpType(), pLeftExprTypeInfo->GetType(), pRightExprTypeInfo->GetType());
	}

	CType* CTypeResolver::VisitDeclaration(CASTDeclarationNode* pNode)
	{
		return pNode->GetTypeInfo()->Resolve(this, mpSymTable);
	}

	CType* CTypeResolver::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode)
	{
		auto args = pNode->GetArgs()->GetChildren();

		CFunctionType::TArgsArray argsTypes;

		CASTIdentifierNode* pCurrArgDecl = nullptr;

		for (auto pCurrArgNode : args)
		{
			pCurrArgDecl = dynamic_cast<CASTIdentifierNode*>((dynamic_cast<CASTDeclarationNode*>(pCurrArgNode))->GetIdentifiers()->GetChildren()[0]);
			
			auto pCurrArgType = Resolve(dynamic_cast<CASTTypeNode*>(pCurrArgNode), mpSymTable);

			pCurrArgType->SetAttribute(pCurrArgDecl->GetAttributes());

			argsTypes.push_back({ pCurrArgDecl->GetName(), pCurrArgType });
		}

		return new CFunctionType(argsTypes, Resolve(dynamic_cast<CASTTypeNode*>(pNode->GetReturnValueType()), mpSymTable), 0x0);
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

		CStructType* pStructType = new CStructType({}, 0x0);

		CType* pFieldType = nullptr;

		for (auto pCurrField : pStructBody->GetStatements())
		{
			pFieldType = Resolve(dynamic_cast<CASTTypeNode*>(pCurrField), mpSymTable);

			auto identifiers = dynamic_cast<CASTDeclarationNode*>(pCurrField)->GetIdentifiers();

			for (auto pCurrIdentifier : identifiers->GetChildren())
			{
				pStructType->AddField(dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName(), pFieldType);
			}
		}

		return pStructType;
	}

	CType* CTypeResolver::VisitNamedType(CASTNamedTypeNode* pNode)
	{
		return new CDependentNamedType(mpSymTable, pNode->GetTypeInfo()->GetName());
	}

	CType* CTypeResolver::_deduceBuiltinType(E_NODE_TYPE type, U32 attributes)
	{
		switch (type)
		{
			case NT_INT8:
				return new CType(CT_INT8, BTS_INT8, attributes);
			case NT_INT16:
				return new CType(CT_INT16,  BTS_INT16,  attributes);
			case NT_INT32:
				return new CType(CT_INT32, BTS_INT32, attributes);
			case NT_INT64:
				return new CType(CT_INT64, BTS_INT64, attributes);
			case NT_UINT8:
				return new CType(CT_UINT8, BTS_UINT8, attributes);
			case NT_UINT16:
				return new CType(CT_UINT16, BTS_UINT16, attributes);
			case NT_UINT32:
				return new CType(CT_UINT32, BTS_UINT32, attributes);
			case NT_UINT64:
				return new CType(CT_UINT64, BTS_UINT64, attributes);
			case NT_FLOAT:
				return new CType(CT_FLOAT, BTS_FLOAT, attributes);
			case NT_DOUBLE:
				return new CType(CT_DOUBLE, BTS_DOUBLE, attributes);
			case NT_STRING:
				return new CType(CT_STRING, BTS_POINTER, attributes);
			case NT_CHAR:
				return new CType(CT_CHAR, BTS_CHAR, attributes);
			case NT_BOOL:
				return new CType(CT_BOOL, BTS_BOOL, attributes);
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
		mType(CT_INT32), mSize(4), mAttributes(0x0)
	{
	}

	CType::CType(const CType& type) :
		mType(type.mType), mSize(type.mSize), mAttributes(type.mAttributes)
	{
	}

	CType::CType(E_COMPILER_TYPES type, U32 size, U32 attributes):
		mType(type), mSize(size), mAttributes(attributes)
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
		return mChildren.empty();
	}

	void CType::SetAttribute(U32 attribute)
	{
		mAttributes |= attribute;
	}

	void CType::SetName(const std::string& name)
	{
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
	
	CBaseValue* CType::GetDefaultValue() const
	{
		if (IsBuiltIn())
		{
			return _getBuiltinTypeDefaultValue(mType);
		}

		return nullptr;
	}

	bool CType::AreSame(const CType* pType) const
	{
		if (!pType)
		{
			return false;
		}

		return (mType == pType->mType) && (mSize == pType->mSize) && (mAttributes == pType->mAttributes);
	}

	bool CType::AreConvertibleTo(const CType* pType) const
	{
		if (!pType)
		{
			return false;
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
		CStructType's definition
	*/

	CStructType::CStructType(const TFieldsArray& fieldsTypes, U32 attributes):
		CType(CT_STRUCT, BTS_POINTER, attributes)
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

	void CStructType::SetName(const std::string& name)
	{
		mName = name;
	}

	void CStructType::SetAttributes(U32 attributes)
	{
		mAttributes = attributes;
	}

	const CStructType::TFieldsArray& CStructType::GetFieldsTypes() const
	{
		return mFieldsTypes;
	}

	CBaseValue* CStructType::GetDefaultValue() const
	{
		return new CPointerValue();
	}

	const std::string& CStructType::GetName() const
	{
		return mName;
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

	CFunctionType::CFunctionType(const TArgsArray& argsTypes, CType* pReturnValueType, U32 attributes):
		mName(), CType(CT_FUNCTION, CT_POINTER, attributes), mpReturnValueType(pReturnValueType)
	{
		std::copy(argsTypes.begin(), argsTypes.end(), std::back_inserter(mArgsTypes));
	}

	TLLVMIRData CFunctionType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitFunctionType(this);
	}

	void CFunctionType::SetName(const std::string& name)
	{
		mName = name;
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

	CBaseValue* CFunctionType::GetDefaultValue() const
	{
		return new CPointerValue();
	}

	const std::string& CFunctionType::GetName() const
	{
		return mName;
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


	CEnumType::CEnumType(const std::string& enumName):
		CType(CT_ENUM, BTS_INT32, 0x0)
	{
	}

	TLLVMIRData CEnumType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return {};
	}

	void CEnumType::SetName(const std::string& name)
	{
		mName = name;
	}

	CBaseValue* CEnumType::GetDefaultValue() const
	{
		return nullptr;
	}

	const std::string& CEnumType::GetName() const
	{
		return mName;
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

	CDependentNamedType::CDependentNamedType(const ISymTable* pSymTable, const std::string& typeIdentifier):
		CType(CT_ALIAS, BTS_UNKNOWN, 0x0), mpSymTable(pSymTable), mName(typeIdentifier), mpDependentType(nullptr)
	{
	}

	TLLVMIRData CDependentNamedType::Accept(ITypeVisitor<TLLVMIRData>* pVisitor)
	{
		return pVisitor->VisitNamedType(this);
	}

	void CDependentNamedType::SetName(const std::string& name)
	{
		mName = name;
	}

	CBaseValue* CDependentNamedType::GetDefaultValue() const
	{
		const CType* pDependentType = mpSymTable->LookUpNamedScope(mName)->mpType;

		return pDependentType->GetDefaultValue();
	}

	const std::string& CDependentNamedType::GetName() const
	{
		return mName;
	}

	bool CDependentNamedType::AreSame(const CType* pType) const
	{
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
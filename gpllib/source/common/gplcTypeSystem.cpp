/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains types defenition

	\todo
*/

#include "common\gplcTypeSystem.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcLiterals.h"
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
		return _deduceBuiltinType(pNode->GetType());
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

		std::vector<CType*> argsTypes;

		for (auto pCurrArgNode : args)
		{
			argsTypes.push_back(Resolve(dynamic_cast<CASTTypeNode*>(pCurrArgNode), mpSymTable));
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

	CType* CTypeResolver::_deduceBuiltinType(E_NODE_TYPE type)
	{
		switch (type)
		{
			case NT_INT8:
				return new CType(CT_INT8, BTS_INT8, 0x0);
			case NT_INT16:
				return new CType(CT_INT16,  BTS_INT16,  0x0);
			case NT_INT32:
				return new CType(CT_INT32, BTS_INT32, 0x0);
			case NT_INT64:
				return new CType(CT_INT64, BTS_INT64, 0x0);
			case NT_UINT8:
				return new CType(CT_UINT8, BTS_UINT8, 0x0);
			case NT_UINT16:
				return new CType(CT_UINT16, BTS_UINT16, 0x0);
			case NT_UINT32:
				return new CType(CT_UINT32, BTS_UINT32, 0x0);
			case NT_UINT64:
				return new CType(CT_UINT64, BTS_UINT64, 0x0);
			case NT_FLOAT:
				return new CType(CT_FLOAT, BTS_FLOAT, 0x0);
			case NT_DOUBLE:
				return new CType(CT_DOUBLE, BTS_DOUBLE, 0x0);
			case NT_STRING:
				return new CType(CT_STRING, BTS_DOUBLE, 0x0);
			case NT_CHAR:
				return new CType(CT_CHAR, BTS_CHAR, 0x0);
			case NT_BOOL:
				return new CType(CT_BOOL, BTS_BOOL, 0x0);
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
	
	CBaseLiteral* CType::GetDefaultValue() const
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

		return (mType == pType->mType) && (mSize == pType->mSize);
	}

	bool CType::AreConvertibleTo(const CType* pType) const
	{
		if (!pType)
		{
			return false;
		}

		return mCastMap[mType][pType->mType];
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

	CBaseLiteral* CType::_getBuiltinTypeDefaultValue(E_COMPILER_TYPES type) const
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
				return new CIntLiteral(0);

			case CT_UINT8:
			case CT_UINT16:
			case CT_UINT32:
			case CT_UINT64:
				return new CUIntLiteral(0);

			case CT_FLOAT:
				return new CFloatLiteral(0.0f);

			case CT_DOUBLE:
				return new CDoubleLiteral(0.0);

			case CT_CHAR:
				return new CCharLiteral("\0");

			case CT_BOOL:
				return new CBoolLiteral(true);
		}

		return nullptr; ///< unknown type
	}
	
	/*!
		CFunctionType's definition
	*/

	CFunctionType::CFunctionType(const std::vector<CType*>& argsTypes, CType* pReturnValueType, U32 attributes):
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

	const std::vector<CType*>& CFunctionType::GetArgsTypes() const
	{
		return mArgsTypes;
	}

	CType* CFunctionType::GetReturnValueType() const
	{
		return mpReturnValueType;
	}

	CBaseLiteral* CFunctionType::GetDefaultValue() const
	{
		return new CNullLiteral();
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
			pOtherFuncArg = pFunctionType->mArgsTypes[i];
			pCurrFuncArg  = mArgsTypes[i];

			if (!pOtherFuncArg || !pCurrFuncArg || !pOtherFuncArg->AreSame(pCurrFuncArg))
			{
				return false;
			}
		}

		return mpReturnValueType->AreSame(pFunctionType->GetReturnValueType());
	}
}
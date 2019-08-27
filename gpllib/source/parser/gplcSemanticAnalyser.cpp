#include "parser/gplcSemanticAnalyser.h"
#include "common/gplcSymTable.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcTypeSystem.h"
#include "parser/gplcASTNodesFactory.h"
#include "utils/Utils.h"


namespace gplc
{
	bool CSemanticAnalyser::Analyze(CASTNode* pInput, ITypeResolver* pTypeResolver, ISymTable* pSymTable, IASTNodesFactory* pNodesFactory)
	{
		if (!pSymTable || !pInput || !pTypeResolver || !pNodesFactory)
		{
			return false;
		}

		mpTypeResolver = pTypeResolver;
		mpSymTable     = pSymTable;
		mpNodesFactory = pNodesFactory;

		mLockSymbolTable = false;
		mStayWithinLoop  = false;

		return pInput->Accept(this);
	}

	bool CSemanticAnalyser::VisitProgramUnit(CASTSourceUnitNode* pProgramNode) 
	{
		if (pProgramNode->GetChildrenCount() < 1)
		{
			return true;
		}

		auto pStatements = pProgramNode->GetStatements();

		for (auto pCurrStatement : pStatements)
		{
			if (!pCurrStatement->Accept(this))
			{
				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitDeclaration(CASTDeclarationNode* pNode)
	{
		CASTNode* pIdentifiersList = pNode->GetIdentifiers();

		// multiple variable per single function argument's declaration are now allowed
		if (pIdentifiersList->GetChildrenCount() > 1 && (pNode->GetAttributes() & AV_FUNC_ARG_DECL) == AV_FUNC_ARG_DECL)
		{
			_notifyError(SAE_FUNC_MULTIPLE_PARAM_PER_DECL_ARE_NOT_ALLOWED);

			return false;
		}

		CType* pTypeInfo = nullptr;
		
		if (!pNode->GetTypeInfo()->Accept(this) || !(pTypeInfo = mpTypeResolver->Resolve(pNode->GetTypeInfo()))) // visit node and deduce the type's info
		{
			return false;
		}

		CASTIdentifierNode* pCurrIdentifierNode = nullptr;
		
		U32 currAttributes = 0x0;

		TSymbolDesc* pCurrSymbolDesc = nullptr;

		for (CASTNode* pCurrChild : pIdentifiersList->GetChildren())
		{
			pCurrIdentifierNode = dynamic_cast<CASTIdentifierNode*>(pCurrChild);
			
			const std::string& currIdentifier = pCurrIdentifierNode->GetName();

			currAttributes = pCurrIdentifierNode->GetAttributes();

			pTypeInfo->SetAttribute(currAttributes);

			auto pDefaultValueExpr = pTypeInfo->GetDefaultValue(mpNodesFactory);

			// do not register variable if it belongs to some structure, because it's already there
			if (currAttributes & AV_STRUCT_FIELD_DECL)
			{
				pCurrSymbolDesc = mpSymTable->LookUp(mpSymTable->GetSymbolHandleByName(currIdentifier));

				// we just need to resolve its type and value
				pCurrSymbolDesc->mpType  = pTypeInfo;
				pCurrSymbolDesc->mpValue = pDefaultValueExpr;
			}
			else if (!mpSymTable->IsLocked() && 
					 mpSymTable->AddVariable({ currIdentifier, pDefaultValueExpr, pTypeInfo }) == InvalidSymbolHandle)
			{
				_notifyError(SAE_IDENTIFIER_ALREADY_DECLARED);

				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitIdentifier(CASTIdentifierNode* pNode) 
	{
		const std::string& identifier = pNode->GetName();

		if (!mpSymTable->LookUp(identifier))
		{
			bool isTypeDefined = mpSymTable->LookUpNamedScope(identifier) != nullptr;

			if (isTypeDefined)
			{
				return true;
			}
			
			_notifyError(SAE_UNDECLARED_IDENTIFIER);

			return false;
		}

		return true;
	}

	bool CSemanticAnalyser::VisitLiteral(CASTLiteralNode* pNode) 
	{
		return true; // for now all literals are assumed as valid 
	}

	bool CSemanticAnalyser::VisitUnaryExpression(CASTUnaryExpressionNode* pNode) 
	{
		return pNode->GetData()->Accept(this);
	}

	bool CSemanticAnalyser::VisitBinaryExpression(CASTBinaryExpressionNode* pNode) 
	{
		CASTExpressionNode* pLeftExpr  = pNode->GetLeft();
		CASTExpressionNode* pRightExpr = pNode->GetRight();

		// check left expression
		CType* pLeftValueType = nullptr;

		if (!pLeftExpr->Accept(this) ||
			!(pLeftValueType = pLeftExpr->Resolve(mpTypeResolver)))
		{
			_notifyError(SAE_INCOMPATIBLE_TYPES_INSIDE_EXPR);

			return false;
		}

		// check right expression
		CType* pRightValueType = nullptr;

		if (!pRightExpr->Accept(this) ||
			!(pRightValueType = pRightExpr->Resolve(mpTypeResolver)))
		{
			_notifyError(SAE_INCOMPATIBLE_TYPES_INSIDE_EXPR);

			return false;
		}

		// \todo check whether an operator is implemented for both these types

		return pLeftValueType->AreSame(pRightValueType);
	}

	bool CSemanticAnalyser::VisitAssignment(CASTAssignmentNode* pNode) 
	{
		CASTExpressionNode* pLeftExpr  = pNode->GetLeft();
		CASTExpressionNode* pRightExpr = pNode->GetRight();

		// check left side
		CType* pLeftValueType = nullptr;

		if (!pLeftExpr->Accept(this) ||
			!(pLeftValueType = pLeftExpr->Resolve(mpTypeResolver)))
		{
			return false;
		}

		// check right side
		CType* pRightValueType = nullptr;

		if (!pRightExpr->Accept(this) ||
			!(pRightValueType = pRightExpr->Resolve(mpTypeResolver)))
		{
			_notifyError(SAE_INCOMPATIBLE_TYPES_INSIDE_EXPR);

			return false;
		}

		// check their compatibility
		return pLeftValueType->AreSame(pRightValueType) /* second condition is for tests purposes only, delete later */ || pRightValueType->AreConvertibleTo(pLeftValueType);
	}

	bool CSemanticAnalyser::VisitStatementsBlock(CASTBlockNode* pNode) 
	{
		auto pStatements = pNode->GetStatements();
		
		for (auto pCurrStatement : pStatements)
		{
			if (!pCurrStatement->Accept(this))
			{
				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitIfStatement(CASTIfStatementNode* pNode) 
	{
		auto pCondition = pNode->GetCondition();
		auto pThenBlock = pNode->GetThenBlock();
		auto pElseBlock = pNode->GetElseBlock();

		// check condition
		CType* pConditionType = nullptr;

		if (!pCondition->Accept(this) || !(pConditionType = pCondition->Resolve(mpTypeResolver)))
		{
			return false;
		}

		if (pConditionType->GetType() != CT_BOOL)
		{
			_notifyError(SAE_LOGIC_EXPR_IS_EXPECTED);

			return false;
		}

		if (!_enterScope(pThenBlock, mpSymTable) || (pElseBlock && !_enterScope(pElseBlock, mpSymTable)))
		{
			return false;
		}

		return true;
	}

	bool CSemanticAnalyser::VisitLoopStatement(CASTLoopStatementNode* pNode) 
	{
		if (pNode->GetBody()->GetChildrenCount() < 1)
		{
			_notifyWarning(SAE_REDUNDANT_LOOP_STATEMENT);
		}

		auto pLoopBody = pNode->GetBody();

		return _containsBreak(pLoopBody) && _enterLoopScope(pLoopBody, mpSymTable);
	}

	bool CSemanticAnalyser::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) 
	{
		if (pNode->GetBody()->GetChildrenCount() < 1)
		{
			_notifyWarning(SAE_REDUNDANT_LOOP_STATEMENT);
		}

		auto pCondition = pNode->GetCondition();
		auto pLoopBody  = pNode->GetBody();

		// check condition
		CType* pConditionType = nullptr;

		if (!pCondition->Accept(this) || !(pConditionType = pCondition->Resolve(mpTypeResolver)))
		{
			return false;
		}

		if (pConditionType->GetType() != CT_BOOL)
		{
			_notifyError(SAE_LOGIC_EXPR_IS_EXPECTED);

			return false;
		}

		return _enterLoopScope(pLoopBody, mpSymTable);
	}

	bool CSemanticAnalyser::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) 
	{
		auto pClosureDecl     = pNode->GetClosure();
		auto pArgs            = pNode->GetArgs();
		auto pReturnValueType = pNode->GetReturnValueType();
		
		bool isCorrect = (pClosureDecl ? pClosureDecl->Accept(this) : true);
		
		_lockSymbolTable([&isCorrect, this, &pArgs]()
		{
			isCorrect = isCorrect && pArgs->Accept(this);
		}, mLockSymbolTable);

		isCorrect = isCorrect && pReturnValueType->Accept(this);

		return isCorrect;
	}

	bool CSemanticAnalyser::VisitFunctionClosure(CASTFunctionClosureNode* pNode) 
	{
		auto pChildren = pNode->GetChildren();

		for (auto pCurrChild : pChildren)
		{
			if (!pCurrChild->Accept(this))
			{
				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitFunctionArgs(CASTFunctionArgsNode* pNode) 
	{
		auto pChildren = pNode->GetChildren();

		for (auto pCurrChild : pChildren)
		{
			if (!pCurrChild->Accept(this))
			{
				return false;
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitFunctionCall(CASTFunctionCallNode* pNode) 
	{
		if (!pNode->GetIdentifier()->Accept(this))
		{
			return false;
		}

		CFunctionType* pCalleeType = dynamic_cast<CFunctionType*>(pNode->GetIdentifier()->Resolve(mpTypeResolver));
		
		if (!pCalleeType)
		{
			// \todo callee's type couldn't be resolved
			return false;
		}

		auto actualArgsCount = pNode->GetArgs() ? pNode->GetArgs()->GetChildrenCount() : 0;

		auto pCalleeArgsTypes = pCalleeType->GetArgsTypes();

		if (pCalleeArgsTypes.size() != actualArgsCount)
		{
			// \todo number of arguments are mismatched between function's type and its usage
			return false;
		}

		// \todo implement checks up of arguments with signatures
		CASTTypeNode* pCurrArgNode = nullptr;

		CType* pExpectedArgType = nullptr;
		CType* pActualArgType   = nullptr;

		for (U32 argIndex = 0; argIndex < actualArgsCount; ++argIndex)
		{
			pCurrArgNode = dynamic_cast<CASTTypeNode*>(pNode->GetArgs()->GetChildren()[argIndex]);

			pExpectedArgType = pCalleeArgsTypes[argIndex].second;

			assert(pCurrArgNode && pExpectedArgType);

			// this conditions check whether actual arguments are valid
			if (!pCurrArgNode->Accept(this) || !(pActualArgType = pCurrArgNode->Resolve(mpTypeResolver)))
			{
				return false;
			}

			bool areSameTypes    = pActualArgType->AreSame(pExpectedArgType);
			bool isConvertibleTo = pActualArgType->AreConvertibleTo(pExpectedArgType);

			if (!areSameTypes)
			{
				switch (isConvertibleTo)
				{
					case false:
						// \todo send a message
						return false;
					/*case true:
						*/
				}
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitReturnStatement(CASTReturnStatementNode* pNode) 
	{
		return true;
	}

	bool CSemanticAnalyser::VisitDefinitionNode(CASTDefinitionNode* pNode) 
	{
		auto pDeclNode  = pNode->GetDeclaration();
		auto pValueNode = dynamic_cast<CASTExpressionNode*>(pNode->GetValue());

		// check a declaration
		CType* pDeclType = nullptr;

		if (!pDeclNode->Accept(this) || !(pDeclType = pDeclNode->Resolve(mpTypeResolver)))
		{
			return false;
		}

		// check value
		CType* pValueType = nullptr;

		if (!pValueNode->Accept(this) || !(pValueType = pValueNode->Resolve(mpTypeResolver)))
		{
			return false;
		}

		// assign values to struct's fields
		CASTIdentifierNode* pCurrField = nullptr;

		TSymbolDesc* pCurrSymbolDesc = nullptr;

		if (pDeclNode->GetAttributes() & AV_STRUCT_FIELD_DECL)
		{
			for (auto pCurrIdentifier : pDeclNode->GetIdentifiers()->GetChildren())
			{
				pCurrField = dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier);

				pCurrSymbolDesc = mpSymTable->LookUp(mpSymTable->GetSymbolHandleByName(pCurrField->GetName()));

				pCurrSymbolDesc->mpValue = pValueNode;
			}
		}

		// check their compatibility

		return pDeclType->AreSame(pValueType) || pValueType->AreConvertibleTo(pDeclType);
	}

	bool CSemanticAnalyser::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) 
	{
		auto pDeclaration = pNode->GetDeclaration();
		auto pLambdaType  = pNode->GetLambdaTypeInfo();
		auto pLambdaBody  = pNode->GetValue();
		
		// only single variable can be defined at once
		if (pDeclaration->GetIdentifiers()->GetChildrenCount() > 1)
		{
			_notifyError(SAE_SINGLE_FUNC_IDENTIFIER_IS_EXPECTED);

			return false;
		}

		mLockSymbolTable = true;

		// check left side
		CType* pDeclFuncType = nullptr;

		if (!pDeclaration->Accept(this) || !(pDeclFuncType = mpTypeResolver->Resolve(pDeclaration->GetTypeInfo())))
		{
			return false;
		}
		
		mpSymTable->CreateScope();

		mLockSymbolTable = false;

		// check lambda type
		CType* pAssignedLambdaType = nullptr;

		if (!pLambdaType->Accept(this) || !(pAssignedLambdaType = mpTypeResolver->Resolve(pLambdaType)))
		{
			return false;
		}

		// check whether the left type compatible with right one or not
		if (!pDeclFuncType->AreSame(pAssignedLambdaType))
		{
			_notifyError(SAE_INCOMPATIBLE_TYPE_OF_ASSIGNED_LAMBDA);

			return false;
		}

		// check the lambda's body
		bool isLambdaBodyValid = pLambdaBody->Accept(this);

		mpSymTable->LeaveScope();

		return isLambdaBodyValid;
	}

	bool CSemanticAnalyser::VisitEnumDeclaration(CASTEnumDeclNode* pNode)
	{
		auto res = mpSymTable->LookUpNamedScope(pNode->GetEnumName()->GetName());

		// \todo check types of values

		return res;
	}

	bool CSemanticAnalyser::VisitStructDeclaration(CASTStructDeclNode* pNode)
	{
		std::string structName = pNode->GetStructName()->GetName();

		auto structTableEntry = mpSymTable->LookUpNamedScope(structName);

		// resolve struct's type
		if (!(structTableEntry->mpType = mpTypeResolver->Resolve(pNode)))
		{
			return false;
		}

		structTableEntry->mpType->SetName(structName);

		// resolve types of struct's fields
		mpSymTable->VisitNamedScope(structName);

		if (!pNode->GetFieldsDeclarations()->Accept(this))
		{
			return false;
		}

		mpSymTable->LeaveScope();

		return structTableEntry;
	}
	
	bool CSemanticAnalyser::VisitBreakOperator(CASTBreakOperatorNode* pNode)
	{
		return _isLoopInterruptionAllowed();
	}

	bool CSemanticAnalyser::VisitContinueOperator(CASTContinueOperatorNode* pNode)
	{
		return _isLoopInterruptionAllowed();
	}

	bool CSemanticAnalyser::VisitAccessOperator(CASTAccessOperatorNode* pNode)
	{
		auto pPrimary   = pNode->GetExpression();
		auto pFieldExpr = pNode->GetMemberName();

		// resolve the expression
		CType* pType = nullptr;

		if (!pPrimary->Accept(this) || !(pType = mpTypeResolver->Resolve(pPrimary)))
		{
			return false;
		}

		// check whether the given field exist for the object of the specified type
		auto pSymbolDesc = mpSymTable->LookUpNamedScope(pType->GetName());

		if (!pSymbolDesc || !pSymbolDesc->mpType)
		{
			_notifyError(SAE_UNDEFINED_TYPE);

			return false;
		}

		CASTIdentifierNode* pIdentifier = dynamic_cast<CASTIdentifierNode*>(dynamic_cast<CASTUnaryExpressionNode*>(pFieldExpr)->GetData());

		if (pIdentifier)
		{
			const std::string& identifier = pIdentifier->GetName();

			if (pSymbolDesc->mVariables.find(identifier) == pSymbolDesc->mVariables.cend())
			{
				if (pSymbolDesc->mNamedScopes.find(identifier) == pSymbolDesc->mNamedScopes.cend())
				{
					_notifyError(SAE_TRY_TO_ACCESS_UNDEFINED_FIELD);

					return false;
				}
			}
		}

		return true;
	}

	bool CSemanticAnalyser::VisitArrayTypeNode(CASTArrayTypeNode* pNode)
	{
		// \todo implement the method
		return true;
	}

	bool CSemanticAnalyser::VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode)
	{
		// \todo implement the method
		return true;
	}

	bool CSemanticAnalyser::VisitPointerTypeNode(CASTPointerTypeNode* pNode)
	{
		return true;
	}

	bool CSemanticAnalyser::VisitImportDirectiveNode(CASTImportDirectiveNode* pNode)
	{
		auto pModuleEntry = mpSymTable->LookUpNamedScope(pNode->GetImportedModuleName());

		CType* pModuleType = mpTypeResolver->Resolve(pNode);

		pModuleEntry->mpType = pModuleType;

		return true;
	}

	bool CSemanticAnalyser::VisitDeferOperatorNode(CASTDeferOperatorNode* pNode)
	{
		auto pExpr = pNode->GetExpr();

		return pExpr && pExpr->Accept(this);
	}

	bool CSemanticAnalyser::VisitIntrinsicCall(CASTIntrinsicCallNode* pNode)
	{
		auto pArgs = pNode->GetArgs();

		switch (pNode->GetType())
		{
			case NT_SIZEOF_OPERATOR:
			case NT_TYPEID_OPERATOR:
				if (pArgs->GetChildrenCount() != 1)
				{
					_notifyError(SAE_INVALID_NUMBER_OF_ARGUMENTS);

					return false;
				}

				break;
			case NT_MEMCPY32_INTRINSIC:
			case NT_MEMCPY64_INTRINSIC:
				if (pArgs->GetChildrenCount() != 3)
				{
					_notifyError(SAE_INVALID_NUMBER_OF_ARGUMENTS);

					return false;
				}

				// \todo add checking types of arguments here
				break;
			case NT_CAST_INTRINSIC:
				if (pArgs->GetChildrenCount() != 2)
				{
					_notifyError(SAE_INVALID_NUMBER_OF_ARGUMENTS);
					
					return false;
				}
				break;
			default:
				UNIMPLEMENTED();
				break;
		}

		return true;
	}

	bool CSemanticAnalyser::_enterScope(CASTBlockNode* pNode, ISymTable* pSymTable)
	{
		pSymTable->CreateScope();

		bool result = pNode->Accept(this);

		pSymTable->LeaveScope();

		return result;
	}

	void CSemanticAnalyser::_lockSymbolTable(const std::function<void()>& action, bool lockSymTable)
	{
		if (lockSymTable)
		{
			mpSymTable->Lock();
		}

		action();

		if (lockSymTable)
		{
			mpSymTable->Unlock();
		}
	}

	bool CSemanticAnalyser::_enterLoopScope(CASTBlockNode* pNode, ISymTable* pSymTable)
	{
		mStayWithinLoop = true;

		bool result = _enterScope(pNode, pSymTable);

		mStayWithinLoop = false;

		return result;
	}

	bool CSemanticAnalyser::_isLoopInterruptionAllowed() const
	{
		if (!mStayWithinLoop)
		{
			_notifyError(SAE_INTERRUPT_STATEMENT_OUTSIDE_LOOP_IS_NOT_ALLOWED);
		}

		return mStayWithinLoop;
	}

	bool CSemanticAnalyser::_containsBreak(CASTBlockNode* pLoopBody) const
	{
		bool containsBreak = false;

		for (auto pCurrStatement : pLoopBody->GetStatements())
		{
			if (pCurrStatement->GetType() == NT_BREAK_OPERATOR)
			{
				containsBreak = true;

				break;
			}
		}

		if (!containsBreak)
		{
			_notifyWarning(SAE_BLOCKING_LOOP);
		}

		return true; // this rule is not an error just a warning for a user
	}

	void CSemanticAnalyser::_notifyWarning(E_SEMANTIC_ANALYSER_MESSAGE message) const
	{
		OnErrorOutput.Invoke({ message, E_MESSAGE_TYPE::MT_WARNING });
	}

	void CSemanticAnalyser::_notifyError(E_SEMANTIC_ANALYSER_MESSAGE message) const
	{
		OnErrorOutput.Invoke({ message, E_MESSAGE_TYPE::MT_ERROR });
	}
}
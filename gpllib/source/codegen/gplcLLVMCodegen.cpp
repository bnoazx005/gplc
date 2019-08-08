#include "codegen/gplcLLVMCodegen.h"
#include "common/gplcSymTable.h"
#include "codegen/gplcLLVMLiteralVisitor.h"
#include "codegen/gplcLLVMTypeVisitor.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcConstExprInterpreter.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>


namespace gplc
{
	TLLVMIRData CLLVMCodeGenerator::Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable, ITypeResolver* pTypeResolver, IConstExprInterpreter* pInterpreter)
	{
		if (!pSymTable || !pTypeResolver || !pInterpreter)
		{
			return {};
		}

		mpCurrActiveFunction = nullptr;

		llvm::LLVMContext llvmContext;

		mpLiteralIRGenerator = new CLLVMLiteralVisitor(llvmContext, this);

		mpTypeGenerator = new CLLVMTypeVisitor(llvmContext);

		mpTypeResolver = pTypeResolver;

		mpConstExprInterpreter = pInterpreter;

		mpContext  = &llvmContext;
		mpSymTable = pSymTable;

		mIRBuildersStack.push(llvm::IRBuilder<>(llvmContext)); // module's builder

		mpGlobalIRBuilder = &mIRBuildersStack.top();

		mpModule = new llvm::Module(pNode->GetModuleName(), llvmContext);

		_defineInitModuleGlobalsFunction();

		pNode->Accept(this);

		mpInitModuleGlobalsIRBuilder->CreateRetVoid();

		// \note FOR DEBUG PURPOSE ONLY
		mpModule->dump();
		//llvm::WriteBitcodeToFile(*mpModule, llvm::outs());
		
		delete mpLiteralIRGenerator;

		delete mpTypeGenerator;

		delete mpInitModuleGlobalsIRBuilder;

		delete mpModule;

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitProgramUnit(CASTSourceUnitNode* pProgramNode)
	{
		if (pProgramNode->GetChildrenCount() < 1)
		{
			return {};
		}

		auto pStatements = pProgramNode->GetStatements();

		for (auto pCurrStatement : pStatements)
		{
			// \todo replace with proper accumulation of statements IR codes
			pCurrStatement->Accept(this);
		}

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitDeclaration(CASTDeclarationNode* pNode)
	{
		auto pIdentifiers = pNode->GetIdentifiers();
		auto pTypeInfo    = pNode->GetTypeInfo();

		assert(pIdentifiers && pTypeInfo);

		llvm::Type* pIdentifiersType = nullptr;

		const TSymbolDesc* pCurrSymbolDesc = nullptr;
		
		CType* pType = nullptr;

		auto& currIRBuidler = mIRBuildersStack.top();

		llvm::Value* pIdentifiersValue = nullptr;

		for (auto pCurrIdentifier : pIdentifiers->GetChildren())
		{
			const std::string& identifier = dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName();
			
			pCurrSymbolDesc = mpSymTable->LookUp(identifier);

			pType = pCurrSymbolDesc->mpType;

			// compute only once, because all identifiers are the same type
			if (!pIdentifiersType)
			{
				// if some type was already defined we use it, in other cases we infer it again based on pType's description
				pIdentifiersType = (mTypesTable.find(pType->GetName()) != mTypesTable.cend()) ? mTypesTable[pType->GetName()] : std::get<llvm::Type*>(pType->Accept(mpTypeGenerator));
			}

			auto pCurrVariableAllocation = currIRBuidler.CreateAlloca(pIdentifiersType, nullptr, dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName());
			
			// retrieve initial value 
			pIdentifiersValue = pIdentifiersValue ? pIdentifiersValue : std::get<llvm::Value*>(pCurrSymbolDesc->mpValue->Accept(this));

			mVariablesTable[mpSymTable->GetSymbolHandleByName(identifier)] = pCurrVariableAllocation;

			// \todo replace this with visitor which generates initializing code per type, something like ITypeInitializer
			switch (pType->GetType())
			{
				case CT_ENUM:
					currIRBuidler.CreateStore(pIdentifiersValue, currIRBuidler.CreateBitCast(pCurrVariableAllocation, llvm::Type::getInt32PtrTy(*mpContext)));
					break;
				case CT_ARRAY:
					// \todo invalid initialization, but it's enough for current tests, REIMPLEMENT THIS LATER
					currIRBuidler.CreateStore(pIdentifiersValue, currIRBuidler.CreateBitCast(pCurrVariableAllocation, llvm::Type::getInt32PtrTy(*mpContext), "arr_cast"));
					break;
				case CT_STRUCT:
					currIRBuidler.CreateStore(pIdentifiersValue, currIRBuidler.CreateBitCast(pCurrVariableAllocation, llvm::Type::getInt32PtrTy(*mpContext)));
					break;
				case CT_POINTER:
					currIRBuidler.CreateStore(pIdentifiersValue, currIRBuidler.CreateBitCast(pCurrVariableAllocation, pIdentifiersType));
					break;
				default:
					currIRBuidler.CreateStore(pIdentifiersValue, pCurrVariableAllocation); // for built-in types only
					break;
			}
		}

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitIdentifier(CASTIdentifierNode* pNode)
	{
		const std::string& name = pNode->GetName();

		const TSymbolDesc* pSymbolDesc = mpSymTable->LookUp(name);

		assert(pSymbolDesc);

		U32 attributes = pSymbolDesc->mpType->GetAttributes() | pNode->GetAttributes();

		if (attributes & AV_FUNC_ARG_DECL)
		{
			return _getIdentifierValue(name);
		}

		if (attributes & AV_RVALUE)
		{
			auto pValueInstruction = _getIdentifierValue(name);

			// this means that an access to the type is more sophisticated than just load it into a register
			if (attributes & AV_AGGREGATE_TYPE)
			{
				return pValueInstruction; 
			}

			return mIRBuildersStack.top().CreateLoad(pValueInstruction, name);
		}

		// \note if it's a function and its a native function we should apply CSymTable::RenameReservedIdentifiers
		if (attributes & AV_NATIVE_FUNC)
		{
			return _declareNativeFunction(pSymbolDesc);
		}

		return (mVariablesTable.find(mpSymTable->GetSymbolHandleByName(name)) != mVariablesTable.cend()) ? _getIdentifierValue(name) : _allocateVariableOnStack(name);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitLiteral(CASTLiteralNode* pNode)
	{
		return pNode->GetValue()->Accept(mpLiteralIRGenerator);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitUnaryExpression(CASTUnaryExpressionNode* pNode)
	{
		llvm::Value* pBaseExpr = std::get<llvm::Value*>(pNode->GetData()->Accept(this));

		auto& currIRBuidler = mIRBuildersStack.top();

		switch (pNode->GetOpType())
		{
			case TT_STAR:	// \note dereference pointer value
				return currIRBuidler.CreateLoad(pBaseExpr);
		}

		return pBaseExpr;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitBinaryExpression(CASTBinaryExpressionNode* pNode)
	{
		llvm::Value* pLeftExprValue  = std::get<llvm::Value*>(pNode->GetLeft()->Accept(this));
		llvm::Value* pRightExprValue = std::get<llvm::Value*>(pNode->GetRight()->Accept(this));

		E_TOKEN_TYPE opType = pNode->GetOpType();

		auto& currIRBuidler = mIRBuildersStack.top();

		CType* pLeftExprType = mpTypeResolver->Resolve(pNode->GetLeft());

		bool isFloatingPoint = pLeftExprType->GetType() == CT_FLOAT || pLeftExprType->GetType() == CT_DOUBLE;

		// arithmetic operations
		if (opType == TT_PLUS || opType == TT_MINUS || opType == TT_STAR || opType == TT_SLASH)
		{
			return currIRBuidler.CreateBinOp(_convertOpTypeToLLVM(opType, isFloatingPoint), pLeftExprValue, pRightExprValue, "tmpexpr");
		}

		// boolean operations
		auto lopPredicate = _convertLogicOpTypeToLLVM(opType, isFloatingPoint);

		if (isFloatingPoint)
		{
			return currIRBuidler.CreateFCmp(lopPredicate, pLeftExprValue, pRightExprValue, "tmplexpr");
		}
		else
		{
			return currIRBuidler.CreateICmp(lopPredicate, pLeftExprValue, pRightExprValue, "tmplexpr");
		}

		return { };
	}

	TLLVMIRData CLLVMCodeGenerator::VisitAssignment(CASTAssignmentNode* pNode)
	{
		// generate l-value's part
		auto leftIRCode = std::get<llvm::Value*>(pNode->GetLeft()->Accept(this));

		// generate r-value's part
		auto rightIRCode = std::get<llvm::Value*>(pNode->GetRight()->Accept(this));

		auto& currIRBuilder = mIRBuildersStack.top();

		return currIRBuilder.CreateStore(rightIRCode, leftIRCode, false);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitStatementsBlock(CASTBlockNode* pNode)
	{
		mpSymTable->VisitScope();
	
		for (auto& currArg : mpCurrActiveFunction->args())
		{
			_allocateVariableOnStack(currArg.getName(), true);
		}

		auto pBlock = llvm::BasicBlock::Create(*mpContext, "entry", mpCurrActiveFunction);
		
		mIRBuildersStack.push(llvm::IRBuilder<>(pBlock));

		for (auto pCurrStatement : pNode->GetStatements())
		{
			pCurrStatement->Accept(this);
		}

		mIRBuildersStack.pop();

		mpSymTable->LeaveScope();

		return pBlock;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitIfStatement(CASTIfStatementNode* pNode)
	{
		llvm::IRBuilder<>& currIRBuilder = mIRBuildersStack.top();

		llvm::BasicBlock* pConditionBlock = llvm::BasicBlock::Create(*mpContext, "cond", mpCurrActiveFunction);

		mIRBuildersStack.push(llvm::IRBuilder<>(pConditionBlock));
		llvm::Value* pConditifon = std::get<llvm::Value*>(pNode->GetCondition()->Accept(this));
		mIRBuildersStack.pop();

		currIRBuilder.CreateBr(pConditionBlock);

		llvm::BasicBlock* pEndBlock = llvm::BasicBlock::Create(*mpContext, "end", mpCurrActiveFunction);

		llvm::BasicBlock* pThenBlock = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetThenBlock()->Accept(this)));
		
		// add end point for then branch
		llvm::IRBuilder<> thenBlockIRBuilder{ pThenBlock };
		thenBlockIRBuilder.CreateBr(pEndBlock);

		llvm::BasicBlock* pElseBlock = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetElseBlock()->Accept(this)));

		// add end point for else branch
		llvm::IRBuilder<> elseBlockIRBuilder{ pElseBlock };
		elseBlockIRBuilder.CreateBr(pEndBlock);

		currIRBuilder.SetInsertPoint(pConditionBlock);
		llvm::Value* pBRInstruction = currIRBuilder.CreateCondBr(pConditifon, pThenBlock, pElseBlock);

		currIRBuilder.SetInsertPoint(pEndBlock);

		return pBRInstruction;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitLoopStatement(CASTLoopStatementNode* pNode)
	{
		llvm::IRBuilder<>& currIRBuilder = mIRBuildersStack.top();

		llvm::BasicBlock* pConditionBlock = llvm::BasicBlock::Create(*mpContext, "cond", mpCurrActiveFunction);
		llvm::BasicBlock* pEndBlock       = llvm::BasicBlock::Create(*mpContext, "end", mpCurrActiveFunction);

		mpLoopConditionBlock = pConditionBlock; // the member is used for break and continue operators
		mpLoopEndBlock       = pEndBlock;

		llvm::BasicBlock* pLoopBody = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetBody()->Accept(this)));

		// link loop with its parent block

		currIRBuilder.CreateBr(pLoopBody);

		llvm::IRBuilder<> loopIRBuilder { pLoopBody };
		loopIRBuilder.CreateBr(pLoopBody);

		currIRBuilder.SetInsertPoint(pConditionBlock);
		currIRBuilder.CreateBr(pLoopBody);

		currIRBuilder.SetInsertPoint(pEndBlock);

		return pLoopBody;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode)
	{
		llvm::IRBuilder<>& currIRBuilder = mIRBuildersStack.top();

		llvm::BasicBlock* pConditionBlock = llvm::BasicBlock::Create(*mpContext, "cond", mpCurrActiveFunction);

		mIRBuildersStack.push(llvm::IRBuilder<>(pConditionBlock));
		llvm::Value* pLoopCondition = std::get<llvm::Value*>(pNode->GetCondition()->Accept(this));
		mIRBuildersStack.pop();

		llvm::BasicBlock* pEndBlock = llvm::BasicBlock::Create(*mpContext, "end", mpCurrActiveFunction);

		mpLoopConditionBlock = pConditionBlock; // the member is used for break and continue operators
		mpLoopEndBlock       = pEndBlock;

		currIRBuilder.CreateBr(pConditionBlock);

		llvm::BasicBlock* pLoopBody = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetBody()->Accept(this)));

		// create loop to condition's check up
		llvm::IRBuilder<> loopIRBuilder { pLoopBody };
		loopIRBuilder.CreateBr(pConditionBlock);

		currIRBuilder.SetInsertPoint(pConditionBlock);
		llvm::Value* pBRInstruction = currIRBuilder.CreateCondBr(pLoopCondition, pLoopBody, pEndBlock);

		currIRBuilder.SetInsertPoint(pEndBlock);

		return pBRInstruction;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitFunctionClosure(CASTFunctionClosureNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitFunctionArgs(CASTFunctionArgsNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitFunctionCall(CASTFunctionCallNode* pNode)
	{
		llvm::IRBuilder<>& currIRBuilder = mIRBuildersStack.top();

		llvm::Value* pCallee = std::get<llvm::Value*>(pNode->GetIdentifier()->Accept(this));
		
		std::vector<llvm::Value*> args;

		auto pNodeArgs = pNode->GetArgs();

		for (auto pCurrArg : pNodeArgs->GetChildren())
		{
			args.push_back(std::get<llvm::Value*>(pCurrArg->Accept(this)));
		}

		return currIRBuilder.CreateCall(pCallee, args, "tmpfncall");
	}

	TLLVMIRData CLLVMCodeGenerator::VisitReturnStatement(CASTReturnStatementNode* pNode)
	{
		return  mIRBuildersStack.top().CreateRet(std::get<llvm::Value*>(pNode->GetExpr()->Accept(this)));
	}

	TLLVMIRData CLLVMCodeGenerator::VisitDefinitionNode(CASTDefinitionNode* pNode)
	{
		//get list of identifiers
		auto pIdentifiers = pNode->GetDeclaration()->GetIdentifiers()->GetChildren();
		
		const TSymbolDesc* pCurrSymbolDesc = nullptr;

		CType* pType = nullptr;

		llvm::Type* pIdentifiersType   = nullptr;
		llvm::Value* pIdentifiersValue = nullptr;

		auto& irBuilder = mIRBuildersStack.top();
		
		for (auto pCurrIdentifier : pIdentifiers)
		{
			const std::string& currIdentifierName = dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName();

			pCurrSymbolDesc = mpSymTable->LookUp(currIdentifierName);
			
			assert(pCurrSymbolDesc && pCurrSymbolDesc->mpType);

			pType = pCurrSymbolDesc->mpType;

			// compute only once, because all identifiers are the same type
			pIdentifiersType = pIdentifiersType ? pIdentifiersType : std::get<llvm::Type*>(pType->Accept(mpTypeGenerator));

			auto pCurrVariableAllocation = irBuilder.CreateAlloca(pIdentifiersType, nullptr, dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName());

			pIdentifiersValue = pIdentifiersValue ? pIdentifiersValue : std::get<llvm::Value*>(pNode->GetValue()->Accept(this));

			mVariablesTable[mpSymTable->GetSymbolHandleByName(currIdentifierName)] = pCurrVariableAllocation;

			irBuilder.CreateStore(pIdentifiersValue, pCurrVariableAllocation);
		}

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode)
	{
		bool isGlobalScope = mpCurrActiveFunction == nullptr;

		// mark left part, which is an anonymus lambda, as static global function
		// generate name for it
		CFunctionType* pInternalLambdaType = dynamic_cast<CFunctionType*>(mpTypeResolver->Resolve(pNode->GetLambdaTypeInfo()));
		
		// generate function pointer type for the left part
		auto pLValueFnType = llvm::dyn_cast<llvm::PointerType>(std::get<llvm::Type*>(pInternalLambdaType->Accept(mpTypeGenerator)));

		std::string lambdaName = CreateAnonymousLambdaName(pInternalLambdaType);

		pInternalLambdaType->SetAttributes(AV_STATIC);
		pInternalLambdaType->SetName(lambdaName);

		auto pLambdaFunctionType = llvm::dyn_cast<llvm::FunctionType>(std::get<llvm::Type*>(pInternalLambdaType->Accept(mpTypeGenerator)));

		llvm::Function* pPrevActiveFunction = mpCurrActiveFunction;

		mpCurrActiveFunction = llvm::Function::Create(pLambdaFunctionType, llvm::Function::ExternalLinkage, lambdaName, mpModule);

		// set names for function's arguments
		auto funcArgsTypes = pInternalLambdaType->GetArgsTypes();

		U32 currArgId = 0;

		for (auto& currArg : mpCurrActiveFunction->args())
		{
			currArg.setName(funcArgsTypes[currArgId++].first);
		}

		auto pFuncIdentifierNode = dynamic_cast<CASTIdentifierNode*>(pNode->GetDeclaration()->GetIdentifiers()->GetChildren()[0]);

		const std::string lValueIdentifier = mpSymTable->RenameReservedIdentifier(dynamic_cast<CASTIdentifierNode*>(pFuncIdentifierNode)->GetName());
		
		llvm::IRBuilder<>* pCurrIRBuilder = isGlobalScope ? mpInitModuleGlobalsIRBuilder : &mIRBuildersStack.top();

		// assign address of the lambda to the function pointer
		llvm::Value* pLValueFuncPointer = nullptr;
		
		if (isGlobalScope)
		{
			pLValueFuncPointer = mpModule->getOrInsertGlobal(lValueIdentifier, pLValueFnType);

			mpModule->getGlobalVariable(lValueIdentifier)->setInitializer(mpCurrActiveFunction);
		}
		else
		{
			pLValueFuncPointer = pCurrIRBuilder->CreateAlloca(pLValueFnType, nullptr, lValueIdentifier);
		}

		llvm::Value* pLValueAssignInstruction = pCurrIRBuilder->CreateStore(pCurrIRBuilder->CreateBitOrPointerCast(mpCurrActiveFunction, pLValueFnType), pLValueFuncPointer);

		mVariablesTable[mpSymTable->GetSymbolHandleByName(pFuncIdentifierNode->GetName())] = pLValueFuncPointer;

		// generate its definition
		auto pBlock = std::get<llvm::Value*>(pNode->GetValue()->Accept(this));

		mpCurrActiveFunction = pPrevActiveFunction; // restore previous value

		const TSymbolDesc* pFuncDesc = mpSymTable->LookUp(pFuncIdentifierNode->GetName());

		if (pFuncDesc->mpType->GetAttributes() & AV_ENTRY_POINT)
		{
			_defineEntryPoint();
		}

		return pLValueAssignInstruction;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitEnumDeclaration(CASTEnumDeclNode* pNode)
	{
		auto pEnumSymbolDesc = mpSymTable->LookUpNamedScope(pNode->GetEnumName()->GetName());

		assert(pEnumSymbolDesc && pEnumSymbolDesc->mpType);

		return pEnumSymbolDesc->mpType->Accept(mpTypeGenerator);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitStructDeclaration(CASTStructDeclNode* pNode)
	{
		std::string name = pNode->GetStructName()->GetName();

		auto pStructSymbolDesc = mpSymTable->LookUpNamedScope(name);
		
		assert(pStructSymbolDesc && pStructSymbolDesc->mpType);
		
		llvm::Type* pStructType = std::get<llvm::Type*>(pStructSymbolDesc->mpType->Accept(mpTypeGenerator));

		if (mTypesTable.find(name) == mTypesTable.cend())
		{
			mTypesTable.insert({ name, pStructType });
		}

		return pStructType;
	}

	llvm::IRBuilder<>* CLLVMCodeGenerator::GetCurrIRBuilder()
	{
		return &mIRBuildersStack.top();
	}

	TLLVMIRData CLLVMCodeGenerator::VisitBreakOperator(CASTBreakOperatorNode* pNode)
	{
		auto& currIRBuilder = mIRBuildersStack.top();

		return currIRBuilder.CreateBr(mpLoopEndBlock);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitContinueOperator(CASTContinueOperatorNode* pNode)
	{
		auto& currIRBuilder = mIRBuildersStack.top();

		return currIRBuilder.CreateBr(mpLoopConditionBlock);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitAccessOperator(CASTAccessOperatorNode* pNode)
	{
		CType* pExprType = pNode->GetExpression()->Resolve(mpTypeResolver);

		CASTIdentifierNode* pFieldNode = dynamic_cast<CASTIdentifierNode*>(dynamic_cast<CASTUnaryExpressionNode*>(pNode->GetMemberName())->GetData());

		// \note for now we suppose that right part after '.' is an identifier
		const std::string& identifierName = pFieldNode->GetName();

		auto& currIRBuilder = mIRBuildersStack.top();

		// get type's description
		auto pTypeDesc = mpSymTable->LookUpNamedScope(pExprType->GetName());

		assert(pTypeDesc && pTypeDesc->mpType && (pTypeDesc->mVariables.find(identifierName) != pTypeDesc->mVariables.cend()));

		TSymbolHandle firstFieldId = pTypeDesc->mVariables.begin()->second;
		TSymbolHandle currFieldId  = 0x0;

		TSymbolDesc* pFieldValue = nullptr;

		llvm::Value* pCurrValue = nullptr;

		switch (pTypeDesc->mpType->GetType())
		{
			case CT_ENUM:
				{
					// retrieve value of the field
					currFieldId = pTypeDesc->mVariables[identifierName];

					pFieldValue = mpSymTable->LookUp(currFieldId);

					auto enumeratorValue = mpConstExprInterpreter->Eval(pFieldValue->mpValue, mpSymTable);

					if (enumeratorValue.HasError())
					{
						assert(false);
						return {};
					}

					if (mVariablesTable.find(currFieldId) == mVariablesTable.cend())
					{
						mVariablesTable[currFieldId] = llvm::ConstantInt::get(std::get<llvm::Type*>(pTypeDesc->mpType->Accept(mpTypeGenerator)), enumeratorValue.Get());
					}
					
					return mVariablesTable[currFieldId];
				}
			case CT_STRUCT:
				// retrieve value of the field
				currFieldId = pTypeDesc->mVariables[identifierName];

				pFieldValue = mpSymTable->LookUp(currFieldId);

				pCurrValue = currIRBuilder.CreateGEP(std::get<llvm::Value*>(pNode->GetExpression()->Accept(this)),
												{
													llvm::ConstantInt::get(llvm::Type::getInt32Ty(*mpContext), 0),
													llvm::ConstantInt::get(llvm::Type::getInt32Ty(*mpContext), currFieldId - firstFieldId)
												});

				if (pFieldNode->GetAttributes() & AV_RVALUE)
				{
					return currIRBuilder.CreateLoad(pCurrValue);
				}

				return pCurrValue;
		}

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitArrayTypeNode(CASTArrayTypeNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode)
	{
		llvm::Value* pPrimaryExprCode = std::get<llvm::Value*>(pNode->GetExpression()->Accept(this));
		llvm::Value* pIndexExprCode   = std::get<llvm::Value*>(pNode->GetIndexExpression()->Accept(this));

		auto& currIRBuilder = mIRBuildersStack.top();

		U32 attributes = pNode->GetAttributes();

		auto pAccessInstruction = currIRBuilder.CreateGEP(pPrimaryExprCode, { llvm::ConstantInt::get(llvm::Type::getInt32Ty(*mpContext), 0), pIndexExprCode }, "arr_access");

		if (attributes & AV_RVALUE)
		{
			return currIRBuilder.CreateLoad(pAccessInstruction, "get_arr_elem");
		}

		return pAccessInstruction;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitPointerTypeNode(CASTPointerTypeNode* pNode)
	{
		return {};
	}

	llvm::Instruction::BinaryOps CLLVMCodeGenerator::_convertOpTypeToLLVM(E_TOKEN_TYPE opType, bool isFloatingPointOp) const
	{
		switch (opType)
		{
			// math ops
			case TT_PLUS:
				return isFloatingPointOp ? llvm::Instruction::FAdd : llvm::Instruction::Add;
			case TT_MINUS:
				return isFloatingPointOp ? llvm::Instruction::FSub : llvm::Instruction::Sub;
			case TT_STAR:
				return isFloatingPointOp ? llvm::Instruction::FMul : llvm::Instruction::Mul;
			case TT_SLASH:
				return isFloatingPointOp ? llvm::Instruction::FDiv : llvm::Instruction::SDiv;
		}

		return {};
	}

	llvm::CmpInst::Predicate CLLVMCodeGenerator::_convertLogicOpTypeToLLVM(E_TOKEN_TYPE opType, bool isFloatingPointOp) const
	{
		switch (opType)
		{
			case TT_LT:
				return isFloatingPointOp ? llvm::CmpInst::Predicate::FCMP_OLT : llvm::CmpInst::Predicate::ICMP_SLT;
			case TT_LE:
				return isFloatingPointOp ? llvm::CmpInst::Predicate::FCMP_OLE : llvm::CmpInst::Predicate::ICMP_SLE;
			case TT_GT:
				return isFloatingPointOp ? llvm::CmpInst::Predicate::FCMP_OGT : llvm::CmpInst::Predicate::ICMP_SGT;
			case TT_GE:
				return isFloatingPointOp ? llvm::CmpInst::Predicate::FCMP_OGE : llvm::CmpInst::Predicate::ICMP_SGT;
			case TT_NE:
				return isFloatingPointOp ? llvm::CmpInst::Predicate::FCMP_ONE : llvm::CmpInst::Predicate::ICMP_NE;
			case TT_EQ:
				return isFloatingPointOp ? llvm::CmpInst::Predicate::FCMP_OEQ : llvm::CmpInst::Predicate::ICMP_EQ;
		}

		return llvm::CmpInst::Predicate::FCMP_FALSE;
	}

	llvm::Value* CLLVMCodeGenerator::_getIdentifierValue(const std::string& identifier) const
	{
		TSymbolHandle symbolHandle = mpSymTable->GetSymbolHandleByName(identifier);

		assert(symbolHandle != InvalidSymbolHandle);

		return mVariablesTable.at(symbolHandle);
	}

	llvm::Value* CLLVMCodeGenerator::_allocateVariableOnStack(const std::string& identifier, bool isFuncArg)
	{
		TSymbolHandle symbolHandle = mpSymTable->GetSymbolHandleByName(identifier);

		assert(symbolHandle != InvalidSymbolHandle);

		// extract identifier's info
		auto symbolDesc = mpSymTable->LookUp(symbolHandle);

		if (isFuncArg)
		{
			llvm::Argument* pCurrActiveArgument = nullptr;

			for (auto& currArg : mpCurrActiveFunction->args())
			{
				if (currArg.getName() == identifier)
				{
					pCurrActiveArgument = &currArg;

					break;
				}
			}

			mVariablesTable[symbolHandle] = pCurrActiveArgument;

			return pCurrActiveArgument;
		}

		// resolve type into LLVM IR
		llvm::Type* pIdentifierType = std::get<llvm::Type*>(symbolDesc->mpType->Accept(mpTypeGenerator));

		auto pAllocateInstruction = mIRBuildersStack.top().CreateAlloca(pIdentifierType, nullptr, identifier);

		 mVariablesTable[symbolHandle] = pAllocateInstruction;

		return pAllocateInstruction;
	}

	void CLLVMCodeGenerator::_defineEntryPoint()
	{
		auto pMainFuncType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*mpContext), 
													{
														llvm::Type::getInt32Ty(*mpContext),
														llvm::PointerType::get(llvm::Type::getInt8PtrTy(*mpContext), 0)
													}, false);

		auto pMainFunctionDef = llvm::Function::Create(pMainFuncType, llvm::Function::InternalLinkage, "main", mpModule);

		llvm::BasicBlock* pMainFuncBody = llvm::BasicBlock::Create(*mpContext, "entry", pMainFunctionDef);

		llvm::IRBuilder<> mainFuncIRBuidler(pMainFuncBody);

		// initialize all global variables of the module
		mainFuncIRBuidler.CreateCall(mpInitModuleGlobalsFunction, {});

		// call _lang_main_entry function
		auto pLangEntryFuncType = mVariablesTable[mpSymTable->GetSymbolHandleByName("_lang_main_entry")];

		auto pDerreferencedFnPtr = mainFuncIRBuidler.CreateLoad(pLangEntryFuncType, "_lang_main_entry");

		mainFuncIRBuidler.CreateRet(mainFuncIRBuidler.CreateCall(pDerreferencedFnPtr, {}));
	}

	void CLLVMCodeGenerator::_defineInitModuleGlobalsFunction()
	{
		auto pInitModuleGlobalsFuncType = llvm::FunctionType::get(llvm::Type::getVoidTy(*mpContext), false);
		
		std::string moduleName = mpModule->getName();
		moduleName = moduleName.substr(0, moduleName.find_first_of('.'));

		mpInitModuleGlobalsFunction = llvm::Function::Create(pInitModuleGlobalsFuncType, llvm::Function::ExternalLinkage, moduleName + "$initModuleGlobals", mpModule);
		
		llvm::BasicBlock* pInitModuleGlobalsFuncBody = llvm::BasicBlock::Create(*mpContext, "entry", mpInitModuleGlobalsFunction);

		mpInitModuleGlobalsIRBuilder = new llvm::IRBuilder<>(pInitModuleGlobalsFuncBody);
	}

	llvm::Value* CLLVMCodeGenerator::_declareNativeFunction(const TSymbolDesc* pFuncDesc)
	{
		TSymbolHandle funcHandle = mpSymTable->GetSymbolHandleByName(pFuncDesc->mName);

		if (mVariablesTable.find(funcHandle) != mVariablesTable.cend())
		{
			return mVariablesTable[funcHandle];
		}

		auto pFunctionType = llvm::dyn_cast<llvm::FunctionType>(std::get<llvm::Type*>(pFuncDesc->mpType->Accept(mpTypeGenerator)));

		mVariablesTable[funcHandle] = llvm::Function::Create(pFunctionType, llvm::Function::ExternalLinkage, pFuncDesc->mName, *mpModule);

		return mVariablesTable[funcHandle];
	}
}
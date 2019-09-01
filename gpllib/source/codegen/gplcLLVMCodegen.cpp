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
#include "llvm/Support/FileSystem.h"
#include "utils/Utils.h"
#include <cassert>


namespace gplc
{
	TLLVMIRData CLLVMCodeGenerator::Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable, ITypeResolver* pTypeResolver, IConstExprInterpreter* pInterpreter,
											 const TOnPreGenerateCallback& onPreGenerateCallback)
	{
		if (!pSymTable || !pTypeResolver || !pInterpreter)
		{
			return {};
		}

		mpCurrActiveFunction = nullptr;

		mpLastVisitedEndBlock = nullptr;

		mpLiteralIRGenerator = new CLLVMLiteralVisitor(mContext, this);

		mpTypeGenerator = new CLLVMTypeVisitor(mContext);

		mpTypeResolver = pTypeResolver;

		mpConstExprInterpreter = pInterpreter;

		mVariablesTable.clear();

		mpSymTable = pSymTable;

		mIRBuildersStack.push(llvm::IRBuilder<>(mContext)); // module's builder

		mpGlobalIRBuilder = &mIRBuildersStack.top();

		mDefferedExpressionsStack = {};

		mpModule = new llvm::Module(pNode->GetModuleName(), mContext);

		onPreGenerateCallback(this);

		_defineInitModuleGlobalsFunction();

		mShouldSkipLoopTail = false;

#if !defined(NDEBUG)
		auto pCurrScopeEntry = mpSymTable->GetCurrentScopeType();
#endif

		pNode->Accept(this);

#if !defined(NDEBUG)
		assert(mpSymTable->GetCurrentScopeType() == pCurrScopeEntry);
#endif

		mpInitModuleGlobalsIRBuilder->CreateRetVoid();

		// \note FOR DEBUG PURPOSE ONLY
#if !defined(NDEBUG)
		//mpModule->dump();
#endif

		delete mpLiteralIRGenerator;

		delete mpTypeGenerator;

		delete mpInitModuleGlobalsIRBuilder;

		return mpModule;
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
		bool isGlobalScope = _isGlobalScope();

		auto pIdentifiers = pNode->GetIdentifiers();
		auto pTypeInfo    = pNode->GetTypeInfo();

		assert(pIdentifiers && pTypeInfo);

		llvm::Type* pIdentifiersType = nullptr;

		const TSymbolDesc* pCurrSymbolDesc = nullptr;
		
		CType* pType = nullptr;

		auto& currIRBuidler = isGlobalScope ? *mpInitModuleGlobalsIRBuilder : mIRBuildersStack.top();

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
				pIdentifiersType = std::get<llvm::Type*>(pType->Accept(mpTypeGenerator));
			}

			// retrieve initial value 
			E_COMPILER_TYPES currType = pType->GetType();

			if (currType != CT_STRUCT /*&& currType != CT_ARRAY*/)
			{
				pIdentifiersValue = pIdentifiersValue ? pIdentifiersValue : std::get<llvm::Value*>(pCurrSymbolDesc->mpValue->Accept(this));
			}

			llvm::Value* pCurrVariableAllocation = nullptr;

			if (isGlobalScope)
			{
				switch (pType->GetType())
				{
					case CT_FUNCTION:
						pCurrVariableAllocation = _declareNativeFunction(pCurrSymbolDesc);

						/*pCurrVariableAllocation = mpModule->getOrInsertFunction(_mangleGlobalModuleIdentifier(pType, identifier), 
																				llvm::dyn_cast<llvm::FunctionType>(pIdentifiersType)).getCallee();

						if (!(pType->GetAttributes() & AV_NATIVE_FUNC))
						{
							llvm::dyn_cast<llvm::GlobalVariable>(pCurrVariableAllocation)->setInitializer(llvm::Constant::getNullValue(pIdentifiersType));
						}*/
						break;
					default:
						pCurrVariableAllocation = mpModule->getOrInsertGlobal(_mangleGlobalModuleIdentifier(pType, identifier), pIdentifiersType);

						llvm::dyn_cast<llvm::GlobalVariable>(pCurrVariableAllocation)->setInitializer(llvm::Constant::getNullValue(pIdentifiersType));

						break;
				}
			}
			else
			{
				pCurrVariableAllocation = currIRBuidler.CreateAlloca(pIdentifiersType, nullptr, dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName());
			}

			mVariablesTable[mpSymTable->GetSymbolHandleByName(identifier)] = pCurrVariableAllocation;

			// \note we don't need to do any additional stuff when work with global function declarations
			if ((isGlobalScope && currType == CT_FUNCTION) ||
				(pType->GetAttributes() & AV_KEEP_UNINITIALIZED))
			{
				return {};
			}
			
			// \todo replace this with visitor which generates initializing code per type, something like ITypeInitializer
			switch (currType)
			{
				case CT_ENUM:
					currIRBuidler.CreateStore(pIdentifiersValue, currIRBuidler.CreateBitCast(pCurrVariableAllocation, llvm::Type::getInt32PtrTy(mContext)));
					break;
				case CT_ARRAY:
					// \todo invalid initialization, but it's enough for current tests, REIMPLEMENT THIS LATER

					//currIRBuidler.CreateStore(pIdentifiersValue, currIRBuidler.CreateBitCast(pCurrVariableAllocation, llvm::Type::getInt32PtrTy(mContext), "arr_cast"));
					
					currIRBuidler.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt64Ty(mContext), dynamic_cast<CArrayType*>(pType)->GetElementsCount()), 
											  _getStructElementValue(currIRBuidler, pCurrVariableAllocation, 1)); // 1 is an index of length field
					break;
				case CT_STRUCT:
					{
						// \todo if we call a constructor from other module first should declare it as external
						std::string constructorName = pType->GetMangledName() + "$ctor";
						
						auto pInputArgType = llvm::PointerType::get(std::get<llvm::Type*>(pType->Accept(mpTypeGenerator)), 0);

						auto pConstructor = mpModule->getOrInsertFunction(constructorName, llvm::FunctionType::get(pInputArgType, { pInputArgType }, false));

						currIRBuidler.CreateCall(pConstructor, { pCurrVariableAllocation });
					}
					break;
				case CT_POINTER:
					currIRBuidler.CreateStore(llvm::ConstantPointerNull::get(llvm::dyn_cast<llvm::PointerType>(pIdentifiersType)), pCurrVariableAllocation, "ptr_init");
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

		TSymbolHandle identifierHandle = mpSymTable->GetSymbolHandleByName(name);

		assert(pSymbolDesc);

		U32 attributes = pSymbolDesc->mpType->GetAttributes() | pNode->GetAttributes();

		if (attributes & AV_FUNC_ARG_DECL)
		{
			return _getIdentifierValue(name);
		}

		// \note if it's a function and its a native function we should apply CSymTable::RenameReservedIdentifiers
		if (attributes & AV_NATIVE_FUNC)
		{
			return _declareNativeFunction(pSymbolDesc);
		}

		auto& irBuilder = mIRBuildersStack.top();

		// \note the second case is true when identifier is a user-defined function pointer
		if ((pSymbolDesc->mpType->GetType() == CT_FUNCTION && !(attributes & AV_NATIVE_FUNC)))
		{
			llvm::Value* pValueInstruction = (mVariablesTable.find(identifierHandle) == mVariablesTable.cend()) ? _declareImportedFunction(pSymbolDesc) : _getIdentifierValue(name);
			
			return irBuilder.CreateLoad(pValueInstruction, name);
		}

		if (attributes & AV_RVALUE)
		{
			auto pValueInstruction = _getIdentifierValue(name);

			// this means that an access to the type is more sophisticated than just load it into a register
			if (attributes & AV_AGGREGATE_TYPE)
			{
				return pValueInstruction; 
			}

			return irBuilder.CreateLoad(pValueInstruction, name);
		}

		return (mVariablesTable.find(identifierHandle) != mVariablesTable.cend()) ? _getIdentifierValue(name) : _allocateVariableOnStack(name);
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
			case TT_MINUS:
				return currIRBuidler.CreateNeg(pBaseExpr, "neg_value");
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
		if (opType == TT_PLUS || opType == TT_MINUS || opType == TT_STAR || opType == TT_SLASH || opType == TT_PERCENT_SIGN)
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

		auto pBlock = llvm::BasicBlock::Create(mContext, "entry", mpCurrActiveFunction, mpLoopEndBlock);
		
		mIRBuildersStack.push(llvm::IRBuilder<>(pBlock));
		mDefferedExpressionsStack.push({});

		E_NODE_TYPE nodeType;

		llvm::BasicBlock* pDeferEndBlock = nullptr;

		bool isDeferUsed = false;

		for (auto pCurrStatement : pNode->GetStatements())
		{
			if (mShouldSkipLoopTail)
			{
				break;
			}

			nodeType = pCurrStatement->GetType();

			// \note skip rest operators til the end of a loop
			if ((mShouldSkipLoopTail = ((mpLoopConditionBlock || mpLoopEndBlock) && (nodeType == NT_BREAK_OPERATOR || nodeType == NT_CONTINUE_OPERATOR))) ||
				(nodeType == NT_RETURN_STATEMENT))
			{
				// \note execute defer block before the interrupting operator
				pDeferEndBlock = _constructDeferBlock(mDefferedExpressionsStack.top());

				if (pDeferEndBlock)
				{
					mIRBuildersStack.push(llvm::IRBuilder<>(pDeferEndBlock));
					isDeferUsed = true;
				}
			}

			pCurrStatement->Accept(this);

			if (isDeferUsed)
			{
				// \note extract pushed "defer_end_block" from the stack
				mIRBuildersStack.pop();
			}
		}

		// \note create deffer block if there is at least one deffered statement in the stack
		// we should execute this one if block has no any interrupting operator at its end like 'break', 'return' and 'continue'
		if (!isDeferUsed)
		{
			pDeferEndBlock = _constructDeferBlock(mDefferedExpressionsStack.top());
		}

		mDefferedExpressionsStack.pop();
		mIRBuildersStack.pop();

		mpSymTable->LeaveScope();
		
		// \note return defer block if it exists
		return pDeferEndBlock ? pDeferEndBlock : pBlock;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitIfStatement(CASTIfStatementNode* pNode)
	{
		llvm::IRBuilder<>& currIRBuilder = mIRBuildersStack.top();

		llvm::BasicBlock* pConditionBlock = llvm::BasicBlock::Create(mContext, "cond", mpCurrActiveFunction);

		mIRBuildersStack.push(llvm::IRBuilder<>(pConditionBlock));
		llvm::Value* pConditifon = std::get<llvm::Value*>(pNode->GetCondition()->Accept(this));
		mIRBuildersStack.pop();

		currIRBuilder.CreateBr(pConditionBlock);

		llvm::BasicBlock* pEndBlock = llvm::BasicBlock::Create(mContext, "end", mpCurrActiveFunction);

		llvm::BasicBlock* pThenBlock = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetThenBlock()->Accept(this)));
		
		// add end point for then branch
		if (pThenBlock->back().getOpcode() != llvm::Instruction::Ret)
		{
			llvm::IRBuilder<> thenBlockIRBuilder{ pThenBlock };
			thenBlockIRBuilder.CreateBr(pEndBlock);
		}

		llvm::BasicBlock* pElseBlock = pNode->GetElseBlock() ? llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetElseBlock()->Accept(this))) : nullptr;

		if (pElseBlock)
		{
			// add end point for else branch
			llvm::IRBuilder<> elseBlockIRBuilder{ pElseBlock };
			elseBlockIRBuilder.CreateBr(pEndBlock);
		}
		else
		{
			pElseBlock = pEndBlock;
		}

		currIRBuilder.SetInsertPoint(pConditionBlock);
		llvm::Value* pBRInstruction = currIRBuilder.CreateCondBr(pConditifon, pThenBlock, pElseBlock);

		currIRBuilder.SetInsertPoint(pEndBlock);

		mpLastVisitedEndBlock = pEndBlock;

		return pBRInstruction;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitLoopStatement(CASTLoopStatementNode* pNode)
	{
		llvm::IRBuilder<>& currIRBuilder = mIRBuildersStack.top();

		//llvm::BasicBlock* pConditionBlock = llvm::BasicBlock::Create(mContext, "cond", mpCurrActiveFunction);
		llvm::BasicBlock* pEndBlock       = llvm::BasicBlock::Create(mContext, "end", mpCurrActiveFunction);

		mpLoopConditionBlock = nullptr; // the member is used for break and continue operators
		mpLoopEndBlock       = pEndBlock;

		llvm::BasicBlock* pLoopBody = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetBody()->Accept(this)));

		// link loop with its parent block
		
		currIRBuilder.CreateBr(pLoopBody);

		llvm::IRBuilder<> loopIRBuilder { pLoopBody };
		
		if (!mShouldSkipLoopTail)
		{
			loopIRBuilder.CreateBr(pLoopBody);
		}
		
		currIRBuilder.SetInsertPoint(pEndBlock);

		mpLastVisitedEndBlock = pEndBlock;

		mpLoopConditionBlock = nullptr;
		mpLoopEndBlock       = nullptr;

		mShouldSkipLoopTail = false;

		return pLoopBody;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode)
	{
		llvm::IRBuilder<>& currIRBuilder = mIRBuildersStack.top();

		llvm::BasicBlock* pConditionBlock = llvm::BasicBlock::Create(mContext, "cond", mpCurrActiveFunction);

		mIRBuildersStack.push(llvm::IRBuilder<>(pConditionBlock));
		llvm::Value* pLoopCondition = std::get<llvm::Value*>(pNode->GetCondition()->Accept(this));
		mIRBuildersStack.pop();

		llvm::BasicBlock* pEndBlock = llvm::BasicBlock::Create(mContext, "end", mpCurrActiveFunction);

		mpLoopConditionBlock = pConditionBlock; // the member is used for break and continue operators
		mpLoopEndBlock       = pEndBlock;

		currIRBuilder.CreateBr(pConditionBlock);

		llvm::BasicBlock* pLoopBody = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetBody()->Accept(this)));

		// create loop to condition's check up
		llvm::IRBuilder<> loopIRBuilder { pLoopBody };

		if (!mShouldSkipLoopTail)
		{
			loopIRBuilder.CreateBr(pConditionBlock);
		}

		currIRBuilder.SetInsertPoint(pConditionBlock);
		llvm::Value* pBRInstruction = currIRBuilder.CreateCondBr(pLoopCondition, pLoopBody, pEndBlock);

		currIRBuilder.SetInsertPoint(pEndBlock);

		mpLastVisitedEndBlock = pEndBlock;

		mpLoopConditionBlock = nullptr;
		mpLoopEndBlock       = nullptr;

		mShouldSkipLoopTail = false;

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
		
		std::vector<llvm::Value*> args{};

		auto pNodeArgs = pNode->GetArgs();

		if (!pNodeArgs)
		{
			return currIRBuilder.CreateCall(pCallee);
		}

		for (auto pCurrArg : pNodeArgs->GetChildren())
		{
			args.push_back(std::get<llvm::Value*>(pCurrArg->Accept(this)));
		}

		return currIRBuilder.CreateCall(pCallee, args);
	}
	
	TLLVMIRData CLLVMCodeGenerator::VisitIntrinsicCall(CASTIntrinsicCallNode* pNode)
	{
		auto pArgs = pNode->GetArgs();

		auto& irBuilder = mIRBuildersStack.top();

		switch (pNode->GetType())
		{
			case NT_SIZEOF_OPERATOR:
				{
					CType* pType = mpTypeResolver->Resolve(dynamic_cast<CASTTypeNode*>(pArgs->GetChildren()[0]));

					return llvm::ConstantInt::get(llvm::Type::getInt64Ty(mContext), pType->GetSize());
				}
			case NT_TYPEID_OPERATOR:
				{
					CType* pType = mpTypeResolver->Resolve(dynamic_cast<CASTTypeNode*>(pArgs->GetChildren()[0]));

					return llvm::ConstantInt::get(llvm::Type::getInt64Ty(mContext), pType->GetTypeId());
				}
			case NT_MEMCPY32_INTRINSIC:
			case NT_MEMCPY64_INTRINSIC:
				{
					llvm::Type* types[] = {
						llvm::Type::getInt8PtrTy(mContext),
						llvm::Type::getInt8PtrTy(mContext),
						(pNode->GetType() == NT_MEMCPY32_INTRINSIC) ? llvm::Type::getInt32Ty(mContext) : llvm::Type::getInt64Ty(mContext),
						llvm::Type::getInt1Ty(mContext),
					};

					std::vector<llvm::Value*> args;

					for (auto pCurrArg : pArgs->GetChildren())
					{
						args.push_back(std::get<llvm::Value*>(pCurrArg->Accept(this)));
					}
					
					args.push_back(llvm::ConstantInt::getFalse(mContext));

					return irBuilder.CreateIntrinsic(llvm::Intrinsic::memcpy, types, args);
				}
			case NT_MEMSET32_INTRINSIC:
			case NT_MEMSET64_INTRINSIC:
				{
					llvm::Type* types[] = {
							llvm::Type::getInt8PtrTy(mContext),
							llvm::Type::getInt8Ty(mContext),
							(pNode->GetType() == NT_MEMSET32_INTRINSIC) ? llvm::Type::getInt32Ty(mContext) : llvm::Type::getInt64Ty(mContext),
							llvm::Type::getInt1Ty(mContext),
					};

					std::vector<llvm::Value*> args;

					for (auto pCurrArg : pArgs->GetChildren())
					{
						args.push_back(std::get<llvm::Value*>(pCurrArg->Accept(this)));
					}

					args.push_back(llvm::ConstantInt::getFalse(mContext));

					// \todo FIXME: this intrinsic doesn't work
					return irBuilder.CreateIntrinsic(llvm::Intrinsic::memset, types, args);
				}
			case NT_CAST_INTRINSIC:
				{
					return _emitTypeConversion(dynamic_cast<CASTTypeNode*>(pArgs->GetChildren()[0]), 
											   dynamic_cast<CASTUnaryExpressionNode*>(pArgs->GetChildren()[1]));
				}
			case NT_ABORT_INTRINSIC:
				{
					return irBuilder.CreateIntrinsic(llvm::Intrinsic::trap, {}, {});
				}
		}

		UNIMPLEMENTED();

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitReturnStatement(CASTReturnStatementNode* pNode)
	{
		auto& currIRBuilder = mIRBuildersStack.top();

		if (!pNode->GetExpr())
		{
			return currIRBuilder.CreateRetVoid();
		}

		return currIRBuilder.CreateRet(std::get<llvm::Value*>(pNode->GetExpr()->Accept(this)));
	}

	TLLVMIRData CLLVMCodeGenerator::VisitDefinitionNode(CASTDefinitionNode* pNode)
	{
		bool isGlobalScope = _isGlobalScope();

		//get list of identifiers
		auto pIdentifiers = pNode->GetDeclaration()->GetIdentifiers()->GetChildren();
		
		const TSymbolDesc* pCurrSymbolDesc = nullptr;

		CType* pType = nullptr;

		llvm::Type* pIdentifiersType   = nullptr;
		llvm::Value* pIdentifiersValue = nullptr;

		auto& irBuilder = isGlobalScope ? *mpInitModuleGlobalsIRBuilder : mIRBuildersStack.top();
		
		for (auto pCurrIdentifier : pIdentifiers)
		{
			const std::string& currIdentifierName = dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName();

			pCurrSymbolDesc = mpSymTable->LookUp(currIdentifierName);
			
			assert(pCurrSymbolDesc && pCurrSymbolDesc->mpType);

			pType = pCurrSymbolDesc->mpType;

			// compute only once, because all identifiers are the same type
			pIdentifiersType = pIdentifiersType ? pIdentifiersType : std::get<llvm::Type*>(pType->Accept(mpTypeGenerator));

			pIdentifiersValue = pIdentifiersValue ? pIdentifiersValue : std::get<llvm::Value*>(pNode->GetValue()->Accept(this));
			
			llvm::Value* pCurrVariableAllocation = nullptr;

			if (isGlobalScope)
			{
				const std::string& mangledIdentifier = _mangleGlobalModuleIdentifier(pType, currIdentifierName);

				pCurrVariableAllocation = mpModule->getOrInsertGlobal(mangledIdentifier, pIdentifiersType);

				mpModule->getGlobalVariable(mangledIdentifier)->setInitializer(llvm::Constant::getNullValue(pIdentifiersType));
			}
			else
			{
				pCurrVariableAllocation = irBuilder.CreateAlloca(pIdentifiersType, nullptr, dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName());
			}

			mVariablesTable[mpSymTable->GetSymbolHandleByName(currIdentifierName)] = pCurrVariableAllocation;

			if (pType->GetType() == CT_POINTER && pIdentifiersValue->getType()->isIntegerTy())
			{
				// \note this case is for null literal which is represented via 0 value of i32 type
				irBuilder.CreateStore(pIdentifiersValue, irBuilder.CreateBitCast(pCurrVariableAllocation, llvm::Type::getInt32PtrTy(mContext)));
			}
			else
			{
				switch (pType->GetType())
				{
					case CT_CHAR:
						{
							auto runeValue = _getStructElementValue(irBuilder, pCurrVariableAllocation, 0);

							irBuilder.CreateStore(pIdentifiersValue, runeValue);
						}
						break;
					default:
						irBuilder.CreateStore(pIdentifiersValue, pCurrVariableAllocation);
						break;
				}
			}
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

		mpLastVisitedEndBlock = nullptr; // \note if this pointer will be changed later, it means there are loops, conditional branches, etc

		// generate its definition
		llvm::BasicBlock* pBlock = llvm::dyn_cast<llvm::BasicBlock>(std::get<llvm::Value*>(pNode->GetValue()->Accept(this)));

		// \note skip this condition if there is return statement already
		if (pInternalLambdaType->IsProcedure() && pBlock->getInstList().back().getOpcode() != llvm::Instruction::Ret)
		{
			llvm::IRBuilder<> funcBodyIRBuilder(mpLastVisitedEndBlock ? mpLastVisitedEndBlock : pBlock);

			funcBodyIRBuilder.CreateRetVoid();
		}

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

		// define struct's constructor
		_defineStructTypeConstructor(dynamic_cast<CStructType*>(pStructSymbolDesc->mpType));

		return pStructType;
	}

	llvm::IRBuilder<>* CLLVMCodeGenerator::GetCurrIRBuilder()
	{
		return &mIRBuildersStack.top();
	}

	llvm::IRBuilder<>* CLLVMCodeGenerator::GetGlobalIRBuilder()
	{
		return mpGlobalIRBuilder;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitBreakOperator(CASTBreakOperatorNode* pNode)
	{
		auto& currIRBuilder = mIRBuildersStack.top();

		return currIRBuilder.CreateBr(mpLoopEndBlock);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitContinueOperator(CASTContinueOperatorNode* pNode)
	{
		auto& currIRBuilder = mIRBuildersStack.top();

		return currIRBuilder.CreateBr(mpLoopConditionBlock ? mpLoopConditionBlock : currIRBuilder.GetInsertBlock());
	}

	TLLVMIRData CLLVMCodeGenerator::VisitAccessOperator(CASTAccessOperatorNode* pNode)
	{
		CType* pExprType = pNode->GetExpression()->Resolve(mpTypeResolver);

		// evaluate right part of the AST, cause it stores actual data, left is just a module's name
		// \note FIXME: possibly, this case should be work for structures too when you call their methods
		if (pExprType->GetType() == CT_MODULE)
		{
			TLLVMIRData result = {};

			mpSymTable->VisitNamedScopeWithRestore(pExprType->GetName(), [&result, &pNode, this](ISymTable* pSymTable)
			{
				result = pNode->GetMemberName()->Accept(this);
			});

			return result;
		}

		auto pMemberNode = dynamic_cast<CASTUnaryExpressionNode*>(pNode->GetMemberName());
		
		// \note for now we suppose that right part after '.' is an identifier
		std::string identifier;

		switch (pMemberNode->GetType())
		{
			case NT_IDENTIFIER:
				identifier = _extractIdentifier(pMemberNode);
				break;
			case NT_UNARY_EXPR:
				identifier = _extractIdentifier(dynamic_cast<CASTUnaryExpressionNode*>(pMemberNode->GetData()));
				break;
			default:
				UNREACHABLE();
				break;
		}		

		auto& currIRBuilder = mIRBuildersStack.top();

		// get type's description
		auto pTypeDesc = mpSymTable->LookUpNamedScope(pExprType->GetName());

		assert(pTypeDesc && pTypeDesc->mpType && (pTypeDesc->mVariables.find(identifier) != pTypeDesc->mVariables.cend()));

		TSymbolHandle firstFieldId = pTypeDesc->mVariables.begin()->second;
		TSymbolHandle currFieldId  = 0x0;

		TSymbolDesc* pFieldValue = nullptr;

		llvm::Value* pCurrValue = nullptr;

		switch (pTypeDesc->mpType->GetType())
		{
			case CT_ENUM:
				{
					// retrieve value of the field
					currFieldId = pTypeDesc->mVariables[identifier];

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
				currFieldId = pTypeDesc->mVariables[identifier];

				pFieldValue = mpSymTable->LookUp(currFieldId);

				pCurrValue = currIRBuilder.CreateGEP(std::get<llvm::Value*>(pNode->GetExpression()->Accept(this)),
												{
													llvm::ConstantInt::get(llvm::Type::getInt32Ty(mContext), 0),
													llvm::ConstantInt::get(llvm::Type::getInt32Ty(mContext), currFieldId - firstFieldId)
												});

				if (dynamic_cast<CASTUnaryExpressionNode*>(pMemberNode->GetData())->GetData()->GetAttributes() & AV_RVALUE)
				{
					return currIRBuilder.CreateLoad(pCurrValue);
				}

				return pCurrValue;
			case CT_MODULE:
				{
					currFieldId = pTypeDesc->mVariables[identifier];
					
					auto pValue = mVariablesTable[currFieldId];

					pFieldValue = mpSymTable->LookUp(currFieldId);

					// if the variable is a function invoke it
					if (pFieldValue->mpType->GetType() == CT_FUNCTION)
					{
						//return currIRBuilder.CreateCall(pValue,)
					}
				}

				UNIMPLEMENTED();

				return {};
		}

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitArrayTypeNode(CASTArrayTypeNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode)
	{
		auto& currIRBuilder = mIRBuildersStack.top();

		llvm::Value* pPrimaryExprCode = _getStructElementValue(currIRBuilder, std::get<llvm::Value*>(pNode->GetExpression()->Accept(this)), 0);
		llvm::Value* pIndexExprCode   = std::get<llvm::Value*>(pNode->GetIndexExpression()->Accept(this));

		U32 attributes = pNode->GetAttributes();

		auto pAccessInstruction = currIRBuilder.CreateGEP(pPrimaryExprCode, { llvm::ConstantInt::get(llvm::Type::getInt32Ty(mContext), 0), pIndexExprCode }, "arr_access");

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

	TLLVMIRData CLLVMCodeGenerator::VisitImportDirectiveNode(CASTImportDirectiveNode* pNode)
	{
		// \note here we add initialization functions calls of modules that are imported into this one

		// firstly, we declare it

		// then call
		llvm::FunctionType* pFuncType = llvm::FunctionType::get(llvm::Type::getVoidTy(mContext), {}, false);

		mpInitModuleGlobalsIRBuilder->CreateCall(mpModule->getOrInsertFunction(_getInitModuleFuncName(pNode->GetImportedModuleName()), pFuncType));

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitDeferOperatorNode(CASTDeferOperatorNode* pNode)
	{
		auto& currBlockDefferedStmts = mDefferedExpressionsStack.top();

		currBlockDefferedStmts.push(pNode->GetExpr());

		return {};
	}

	ITypeVisitor<TLLVMIRData>* CLLVMCodeGenerator::GetTypeGenerator() const
	{
		return mpTypeGenerator;
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
			case TT_PERCENT_SIGN:
				return isFloatingPointOp ? llvm::Instruction::FRem : llvm::Instruction::SRem;
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
		auto pMainFuncType = llvm::FunctionType::get(llvm::Type::getInt32Ty(mContext), 
													{
														llvm::Type::getInt32Ty(mContext),
														llvm::PointerType::get(llvm::Type::getInt8PtrTy(mContext), 0)
													}, false);

		auto pMainFunctionDef = llvm::Function::Create(pMainFuncType, llvm::Function::ExternalLinkage, "main", mpModule);

		llvm::BasicBlock* pMainFuncBody = llvm::BasicBlock::Create(mContext, "entry", pMainFunctionDef);

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
		auto pInitModuleGlobalsFuncType = llvm::FunctionType::get(llvm::Type::getVoidTy(mContext), false);
		
		std::string moduleName = mpModule->getName();
		moduleName = moduleName.substr(0, moduleName.find_first_of('.'));

		// \note create flag that tells was the module initialized or not
		auto boolType = llvm::Type::getInt1Ty(mContext);

		auto isModuleInitialized = mpModule->getOrInsertGlobal(std::string("is_").append(moduleName).append("_initialized"), boolType);

		llvm::dyn_cast<llvm::GlobalVariable>(isModuleInitialized)->setInitializer(llvm::ConstantInt::get(boolType, 0));

		mpInitModuleGlobalsFunction = llvm::Function::Create(pInitModuleGlobalsFuncType, llvm::Function::ExternalLinkage, _getInitModuleFuncName(moduleName), mpModule);
		
		llvm::BasicBlock* pInitModuleGlobalsFuncBody = llvm::BasicBlock::Create(mContext, "entry", mpInitModuleGlobalsFunction);

		mpInitModuleGlobalsIRBuilder = new llvm::IRBuilder<>(pInitModuleGlobalsFuncBody);

		// \note assign true to is_<module-name>_initialized variable
		mpInitModuleGlobalsIRBuilder->CreateStore(llvm::ConstantInt::getTrue(mContext), isModuleInitialized);

		// \note add check up whether the module's globals were initialized or not to prevent doubled initialization
		llvm::IRBuilder<> preConditionIRBuilder(llvm::BasicBlock::Create(mContext, "pre_cond_entry", mpInitModuleGlobalsFunction, pInitModuleGlobalsFuncBody));
		
		llvm::BasicBlock* pTruePreConditionBlock = llvm::BasicBlock::Create(mContext, "already_initialized", mpInitModuleGlobalsFunction);

		preConditionIRBuilder.CreateCondBr(preConditionIRBuilder.CreateLoad(isModuleInitialized), pTruePreConditionBlock, pInitModuleGlobalsFuncBody);

		llvm::IRBuilder<> trueConditionIRBuilder(pTruePreConditionBlock);
		trueConditionIRBuilder.CreateRetVoid();
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

	void CLLVMCodeGenerator::_defineStructTypeConstructor(CStructType* pType)
	{
		std::string structName      = pType->GetName();
		std::string constructorName = pType->GetMangledName() + "$ctor";

		auto pInputArgType = llvm::PointerType::get(std::get<llvm::Type*>(pType->Accept(mpTypeGenerator)), 0);

		llvm::FunctionType* pConstructorType = llvm::FunctionType::get(pInputArgType, { pInputArgType }, false);

		auto pTypeDesc = mpSymTable->LookUpNamedScope(structName);

		mpSymTable->VisitNamedScope(structName);

		TSymbolHandle constructorHandle = mpSymTable->AddVariable({ constructorName, nullptr, nullptr });

		auto pConstructorFunction = llvm::Function::Create(pConstructorType, llvm::Function::ExternalLinkage, constructorName, *mpModule);
	
		auto pArg = pConstructorFunction->args().begin();

		mVariablesTable[constructorHandle] = pConstructorFunction;
		
		llvm::IRBuilder<> currIRBuilder{ llvm::BasicBlock::Create(mContext, "entry", pConstructorFunction) };

		U32 firstFieldHandle = pTypeDesc->mVariables.begin()->second;
		U32 currFieldHandle  = InvalidSymbolHandle;

		llvm::Value* pCurrValue = nullptr;

		auto zeroIndex = llvm::ConstantInt::get(llvm::Type::getInt32Ty(mContext), 0);

		for (auto currFieldTypeInfo : pType->GetFieldsTypes())
		{
			currFieldHandle = pTypeDesc->mVariables[currFieldTypeInfo.first];

			pCurrValue = currIRBuilder.CreateGEP(pArg,
				{
					zeroIndex,
					llvm::ConstantInt::get(llvm::Type::getInt32Ty(mContext), currFieldHandle - firstFieldHandle)
				});
			
			auto pAssignedValue = std::get<llvm::Value*>(mpSymTable->LookUp(currFieldHandle)->mpValue->Accept(this));

			if (currFieldTypeInfo.second->GetType() == CT_POINTER && pAssignedValue->getType()->isIntegerTy())
			{
				currIRBuilder.CreateStore(pAssignedValue, currIRBuilder.CreateBitOrPointerCast(pCurrValue, llvm::Type::getInt32PtrTy(mContext), "ptr_reinterp_cast"));
			}
			else
			{
				currIRBuilder.CreateStore(pAssignedValue, pCurrValue);
			}
		}

		mpSymTable->LeaveScope();

		currIRBuilder.CreateRet(pArg);
	}

	inline bool CLLVMCodeGenerator::_isGlobalScope() const
	{
		return mpCurrActiveFunction == nullptr;
	}

	std::string CLLVMCodeGenerator::_mangleGlobalModuleIdentifier(CType* pType, const std::string& identifier) const
	{
		// \note native functions are not mangled nomatter on module they are placed in
		if (pType->GetAttributes() & AV_NATIVE_FUNC)
		{
			return identifier;
		}

		CType* pParentType = pType->GetParent();

		return (pParentType ? pParentType->GetMangledName() : "") + identifier;
	}

	llvm::BasicBlock* CLLVMCodeGenerator::_constructDeferBlock(TExpressionsStack& expressionsStack)
	{
		if (expressionsStack.empty())
		{
			return nullptr;
		}
		
		auto pDeferBlock    = llvm::BasicBlock::Create(mContext, "defer_block", mpCurrActiveFunction);
		auto pDeferEndBlock = llvm::BasicBlock::Create(mContext, "end_defer_block", mpCurrActiveFunction, pDeferBlock);

		auto& predcessingIRBuilder = mIRBuildersStack.top();
		predcessingIRBuilder.CreateBr(pDeferBlock);
		
		llvm::IRBuilder<> currIRBuilder { pDeferBlock };

		mIRBuildersStack.push(currIRBuilder);

		CASTExpressionNode* pCurrExpression = nullptr;

		while (!expressionsStack.empty())
		{
			pCurrExpression = expressionsStack.top();

			pCurrExpression->Accept(this);

			expressionsStack.pop();
		}
		
		// \note jump to "end_defer_block"
		currIRBuilder.CreateBr(pDeferEndBlock);
		
		mIRBuildersStack.pop();

		return pDeferEndBlock;
	}

	std::string CLLVMCodeGenerator::_extractIdentifier(CASTUnaryExpressionNode* pNode) const
	{
		return dynamic_cast<CASTIdentifierNode*>(pNode->GetData())->GetName();
	}

	std::string CLLVMCodeGenerator::_getInitModuleFuncName(const std::string& moduleName) const
	{
		return std::string(moduleName).append("$initModuleGlobals");
	}

	llvm::Value* CLLVMCodeGenerator::_declareImportedFunction(const TSymbolDesc* pFuncDesc)
	{
		TSymbolHandle funcHandle = mpSymTable->GetSymbolHandleByName(pFuncDesc->mName);

		if (mVariablesTable.find(funcHandle) != mVariablesTable.cend())
		{
			return mVariablesTable[funcHandle];
		}

		auto pFunctionType = llvm::dyn_cast<llvm::PointerType>(std::get<llvm::Type*>(pFuncDesc->mpType->Accept(mpTypeGenerator)));

		mVariablesTable[funcHandle] = mpModule->getOrInsertGlobal(pFuncDesc->mName, pFunctionType);

		return mVariablesTable[funcHandle];
	}

	llvm::Value* CLLVMCodeGenerator::_emitTypeConversion(CASTTypeNode* pType, CASTUnaryExpressionNode* pExpr)
	{
		CType* pInternalType     = pType->Resolve(mpTypeResolver);
		CType* pInternalExprType = pExpr->Resolve(mpTypeResolver);

		E_COMPILER_TYPES destType = pInternalType->GetType();
		E_COMPILER_TYPES srcType  = pInternalExprType->GetType();

		U32 destTypeSize = pInternalType->GetSize();
		U32 srcTypeSize  = pInternalExprType->GetSize();

		auto& irBuilder = mIRBuildersStack.top();

		auto pValue = std::get<llvm::Value*>(pExpr->Accept(this));

		// \note integer <-> integer or char <-> integer
		if ((pInternalType->IsInteger() || destType == CT_CHAR) && (pInternalExprType->IsInteger() || srcType == CT_CHAR))
		{		
			// \note firstly, extract value from the structure { i32 } if it's not just a character literal
			if (srcType == CT_CHAR && pValue->getType()->isStructTy())
			{
				pValue = _getStructElementValue(irBuilder, pValue, 0);
			}

			// \note the situation is possible when you want downcast to lesser integral type or upcast to wider one
			if (srcTypeSize > destTypeSize)
			{
				return irBuilder.CreateTrunc(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "trunc_cast");
			}
			else if (srcTypeSize < destTypeSize) // make upcast to wider type
			{
				// \note signed -> unsigned
				if (pInternalType->IsUnsignedInteger() && !pInternalExprType->IsUnsignedInteger())
				{
					return irBuilder.CreateSExt(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "sign_cast");
				}
				else
				{
					return irBuilder.CreateZExt(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "up_cast");
				}
			}
			else
			{
				if (pInternalType->IsUnsignedInteger() && !pInternalExprType->IsUnsignedInteger())
				{
					return irBuilder.CreateSExtOrBitCast(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "sign_cast");
				}

				return pValue;
			}
		}

		// \note float <-> double
		if (pInternalType->IsFloatingPoint() && pInternalExprType->IsFloatingPoint())
		{
			// double -> float
			if (srcTypeSize < destTypeSize)
			{
				return irBuilder.CreateFPExt(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "fp_up_cast");
			}
			else // float -> double
			{
				return irBuilder.CreateFPTrunc(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "fp_trunc_cast");
			}
		}

		// \note float -> integer
		if (pInternalType->IsInteger() && pInternalExprType->IsFloatingPoint())
		{
			if (pInternalType->IsUnsignedInteger())
			{
				return irBuilder.CreateFPToUI(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "fp2uint_cast");
			}

			return irBuilder.CreateFPToSI(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "fp2sint_cast");
		}

		// \note integer -> float
		if (pInternalType->IsFloatingPoint() && pInternalExprType->IsInteger())
		{
			if (pInternalExprType->IsUnsignedInteger())
			{
				return irBuilder.CreateUIToFP(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "uint2fp_cast");
			}

			return irBuilder.CreateSIToFP(pValue, std::get<llvm::Type*>(pInternalType->Accept(mpTypeGenerator)), "sint2fp_cast");
		}

		UNIMPLEMENTED();

		return nullptr;
	}

	llvm::Value* CLLVMCodeGenerator::_getStructElementValue(llvm::IRBuilder<>& irBuilder, llvm::Value* pStructValue, I32 index)
	{
		static auto zeroIndex { llvm::ConstantInt::get(llvm::Type::getInt32Ty(mContext), 0) };

		return irBuilder.CreateGEP(pStructValue,
			{
				zeroIndex, llvm::ConstantInt::get(llvm::Type::getInt32Ty(mContext), index)
			});
	}
}
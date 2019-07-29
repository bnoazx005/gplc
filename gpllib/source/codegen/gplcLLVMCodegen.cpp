#include "codegen/gplcLLVMCodegen.h"
#include "common/gplcSymTable.h"
#include "codegen/gplcLLVMLiteralVisitor.h"
#include "codegen/gplcLLVMTypeVisitor.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcValues.h"
#include "common/gplcTypeSystem.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>


namespace gplc
{
	TLLVMIRData CLLVMCodeGenerator::Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable)
	{
		if (!pSymTable)
		{
			return {};
		}

		mpCurrActiveFunction = nullptr;

		llvm::LLVMContext llvmContext;

		mpLiteralIRGenerator = new CLLVMLiteralVisitor(llvmContext);

		mpTypeGenerator = new CLLVMTypeVisitor(llvmContext);

		mpTypeResolver = new CTypeResolver();

		mpContext  = &llvmContext;
		mpSymTable = pSymTable;

		mIRBuildersStack.push(llvm::IRBuilder<>(llvmContext)); // module's builder

		mpGlobalIRBuilder = &mIRBuildersStack.top();

		mpModule = new llvm::Module("top", llvmContext);

		_defineInitModuleGlobalsFunction();

		pNode->Accept(this);

		mpInitModuleGlobalsIRBuilder->CreateRetVoid();

		// \note FOR DEBUG PURPOSE ONLY
		mpModule->dump();
		//llvm::WriteBitcodeToFile(*mpModule, llvm::outs());
		
		delete mpTypeResolver;

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
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitIdentifier(CASTIdentifierNode* pNode)
	{
		const std::string& name = pNode->GetName();

		if (pNode->GetAttributes() & AV_RVALUE)
		{
			auto pValueInstruction = _getIdentifierValue(name);

			return mIRBuildersStack.top().CreateLoad(pValueInstruction, name);
		}

		return _allocateVariableOnStack(name);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitLiteral(CASTLiteralNode* pNode)
	{
		return pNode->GetValue()->Accept(mpLiteralIRGenerator);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitUnaryExpression(CASTUnaryExpressionNode* pNode)
	{
		return pNode->GetData()->Accept(this);
	}

	TLLVMIRData CLLVMCodeGenerator::VisitBinaryExpression(CASTBinaryExpressionNode* pNode)
	{
		llvm::Value* pLeftExprValue  = std::get<llvm::Value*>(pNode->GetLeft()->Accept(this));
		llvm::Value* pRightExprValue = std::get<llvm::Value*>(pNode->GetRight()->Accept(this));

		return mIRBuildersStack.top().CreateBinOp(_convertOpTypeToLLVM(pNode->GetOpType(), false), pLeftExprValue, pRightExprValue, "tmpexpr");
	}

	TLLVMIRData CLLVMCodeGenerator::VisitAssignment(CASTAssignmentNode* pNode)
	{
		// generate l-value's part
		auto leftIRCode = pNode->GetLeft()->Accept(this);

		// generate r-value's part
		auto rightIRCode = pNode->GetRight()->Accept(this);

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitStatementsBlock(CASTBlockNode* pNode)
	{
		mpSymTable->VisitScope();
	
		for (auto& currArg : mpCurrActiveFunction->args())
		{
			_allocateVariableOnStack(currArg.getName());
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
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitLoopStatement(CASTLoopStatementNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode)
	{

		return {};
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
		return {};
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
		CFunctionType* pInternalLambdaType = dynamic_cast<CFunctionType*>(mpTypeResolver->Resolve(pNode->GetLambdaTypeInfo(), mpSymTable));
		
		// generate function pointer type for the left part
		auto pLValueFnType = llvm::dyn_cast<llvm::PointerType>(std::get<llvm::Type*>(pInternalLambdaType->Accept(mpTypeGenerator)));

		std::string lambdaName = CreateAnonymousLambdaName(pInternalLambdaType);

		pInternalLambdaType->SetAttributes(AV_STATIC);
		pInternalLambdaType->SetName(lambdaName);

		auto pLambdaFunctionType = llvm::dyn_cast<llvm::FunctionType>(std::get<llvm::Type*>(pInternalLambdaType->Accept(mpTypeGenerator)));

		mpCurrActiveFunction = llvm::Function::Create(pLambdaFunctionType, llvm::Function::ExternalLinkage, lambdaName, mpModule);

		// set names for function's arguments
		auto funcArgsTypes = pInternalLambdaType->GetArgsTypes();

		U32 currArgId = 0;

		for (auto& currArg : mpCurrActiveFunction->args())
		{
			currArg.setName(funcArgsTypes[currArgId++].first);
		}

		mpCurrActiveFunction->dump();

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

		const TSymbolDesc* pFuncDesc = mpSymTable->LookUp(pFuncIdentifierNode->GetName());

		if (pFuncDesc->mpType->GetAttributes() & AV_ENTRY_POINT)
		{
			_defineEntryPoint();
		}

		return pLValueAssignInstruction;
	}

	TLLVMIRData CLLVMCodeGenerator::VisitEnumDeclaration(CASTEnumDeclNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitStructDeclaration(CASTStructDeclNode* pNode)
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
			// logic ops
			// \todo implement logic operators
		}

		return {};
	}

	llvm::Value* CLLVMCodeGenerator::_getIdentifierValue(const std::string& identifier) const
	{
		TSymbolHandle symbolHandle = mpSymTable->GetSymbolHandleByName(identifier);

		assert(symbolHandle != InvalidSymbolHandle);

		return mVariablesTable.at(symbolHandle);
	}

	llvm::Value* CLLVMCodeGenerator::_allocateVariableOnStack(const std::string& identifier)
	{
		TSymbolHandle symbolHandle = mpSymTable->GetSymbolHandleByName(identifier);

		assert(symbolHandle != InvalidSymbolHandle);

		// extract identifier's info
		auto symbolDesc = mpSymTable->LookUp(symbolHandle);

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

		mpInitModuleGlobalsFunction = llvm::Function::Create(pInitModuleGlobalsFuncType, llvm::Function::InternalLinkage, "_initModuleGlobals", mpModule);
		
		llvm::BasicBlock* pInitModuleGlobalsFuncBody = llvm::BasicBlock::Create(*mpContext, "entry", mpInitModuleGlobalsFunction);

		mpInitModuleGlobalsIRBuilder = new llvm::IRBuilder<>(pInitModuleGlobalsFuncBody);
	}
}
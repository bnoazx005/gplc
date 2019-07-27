#include "codegen/gplcLLVMCodegen.h"
#include "common/gplcSymTable.h"
#include "codegen/gplcLLVMLiteralVisitor.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcValues.h"
#include <cassert>


namespace gplc
{
	TLLVMIRData CLLVMCodeGenerator::Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable)
	{
		if (!pSymTable)
		{
			return {};
		}

		llvm::LLVMContext llvmContext;

		mpLiteralIRGenerator = new CLLVMLiteralVisitor(llvmContext);

		mpContext  = &llvmContext;
		mpSymTable = pSymTable;

		mpBuilder = new llvm::IRBuilder<>(llvmContext);

		llvm::Module* pSourceUnitModule = new llvm::Module("top", llvmContext);

		///>============================================
		/// demo code
		auto fn = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(llvmContext), false), llvm::Function::ExternalLinkage, "main", *pSourceUnitModule);
		auto entry = llvm::BasicBlock::Create(llvmContext, "entry", fn);

		llvm::IRBuilder<> builder(entry);
		int v = 42;
		llvm::AllocaInst* pInst = builder.CreateAlloca(llvm::Type::getInt32Ty(llvmContext), nullptr, "x");
		builder.CreateStore(builder.CreateBinOp(llvm::Instruction::Add, builder.CreateLoad(pInst, "x"), llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvmContext), 42), "tmp"), pInst);
		builder.CreateRet(pInst);
				
		pSourceUnitModule->dump();
		// end of demo code
		//<==============================================
		\
		pNode->Accept(this);
		
		delete mpBuilder;

		delete mpLiteralIRGenerator;

		delete pSourceUnitModule;

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
			std::get<llvm::Value*>(pCurrStatement->Accept(this))->dump();
		}

		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitDeclaration(CASTDeclarationNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitIdentifier(CASTIdentifierNode* pNode)
	{
		if (pNode->GetAttributes() & AV_RVALUE)
		{
			auto pValueInstruction = _getIdentifierValue(pNode->GetName());

			return mpBuilder->CreateLoad(pValueInstruction, pNode->GetName());
		}



		return {};
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

		return mpBuilder->CreateBinOp(_convertOpTypeToLLVM(pNode->GetOpType(), false), pLeftExprValue, pRightExprValue, "tmpexpr");
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
		return {};
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
		return mpBuilder->CreateRet(std::get<llvm::Value*>(pNode->GetExpr()->Accept(this)));
	}

	TLLVMIRData CLLVMCodeGenerator::VisitDefinitionNode(CASTDefinitionNode* pNode)
	{
		return {};
	}

	TLLVMIRData CLLVMCodeGenerator::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode)
	{
		return {};
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

	void CLLVMCodeGenerator::_pushIdentifierValue(const std::string& identifier, llvm::Value* pValue)
	{

	}
}
/*!
	\author Ildar Kasimov
	\date   15.07.2019
	\copyright

	\brief The file contains definition of the code generator based on LLVM

	\todo
*/

#ifndef GPLC_LLVM_CODE_GENERATOR_H
#define GPLC_LLVM_CODE_GENERATOR_H


#include "codegen/gplcCodegen.h"
#include "common/gplcVisitor.h"
#include "lexer/gplcTokens.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include <variant>


namespace gplc
{
	class CLLVMLiteralVisitor;


	class CLLVMCodeGenerator : public ICodeGenerator
	{
		protected:
			typedef ILiteralVisitor<TLLVMIRData> TLLVMLiteralVisitor;
		public:
			CLLVMCodeGenerator() = default;
			virtual ~CLLVMCodeGenerator() = default;

			TLLVMIRData Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable) override;

			TLLVMIRData VisitProgramUnit(CASTSourceUnitNode* pProgramNode) override;

			TLLVMIRData VisitDeclaration(CASTDeclarationNode* pNode) override;

			TLLVMIRData VisitIdentifier(CASTIdentifierNode* pNode) override;

			TLLVMIRData VisitLiteral(CASTLiteralNode* pNode) override;

			TLLVMIRData VisitUnaryExpression(CASTUnaryExpressionNode* pNode) override;

			TLLVMIRData VisitBinaryExpression(CASTBinaryExpressionNode* pNode) override;

			TLLVMIRData VisitAssignment(CASTAssignmentNode* pNode) override;

			TLLVMIRData VisitStatementsBlock(CASTBlockNode* pNode) override;

			TLLVMIRData VisitIfStatement(CASTIfStatementNode* pNode) override;

			TLLVMIRData VisitLoopStatement(CASTLoopStatementNode* pNode) override;

			TLLVMIRData VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) override;

			TLLVMIRData VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) override;

			TLLVMIRData VisitFunctionClosure(CASTFunctionClosureNode* pNode) override;

			TLLVMIRData VisitFunctionArgs(CASTFunctionArgsNode* pNode) override;

			TLLVMIRData VisitFunctionCall(CASTFunctionCallNode* pNode) override;

			TLLVMIRData VisitReturnStatement(CASTReturnStatementNode* pNode) override;

			TLLVMIRData VisitDefinitionNode(CASTDefinitionNode* pNode) override;

			TLLVMIRData VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) override;
		protected:
			CLLVMCodeGenerator(const CLLVMCodeGenerator& codeGenerator) = default;

			llvm::Instruction::BinaryOps _convertOpTypeToLLVM(E_TOKEN_TYPE opType, bool isFloatingPointOp = false) const;
		protected:
			TLLVMLiteralVisitor* mpLiteralIRGenerator;

			llvm::LLVMContext*   mpContext;

			ISymTable*           mpSymTable;

			llvm::IRBuilder<>*   mpBuilder;
	};
}

#endif
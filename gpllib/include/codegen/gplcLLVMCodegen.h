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
#include <unordered_map>
#include <stack>


namespace gplc
{
	struct TSymbolDesc;

	class ITypeResolver;
	class CASTExpressionNode;


	class CLLVMCodeGenerator : public ICodeGenerator
	{
		protected:
			typedef ILiteralVisitor<TLLVMIRData>                    TLLVMLiteralVisitor;

			typedef ITypeVisitor<TLLVMIRData>                       TLLVMTypeVisitor;

			typedef std::unordered_map<TSymbolHandle, llvm::Value*> TValuesTable;

			typedef std::stack<llvm::IRBuilder<>>                   TIRBuidlersStack;

			typedef std::stack<CASTExpressionNode*>                 TExpressionsStack;

			typedef std::stack<TExpressionsStack>                   TDeferredExpressionStack;
		public:
			CLLVMCodeGenerator() = default;
			virtual ~CLLVMCodeGenerator() = default;

			TLLVMIRData Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable, ITypeResolver* pTypeResolver, IConstExprInterpreter* pInterpreter,
								 const TOnPreGenerateCallback& onPreGenerateCallback) override;

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

			TLLVMIRData VisitEnumDeclaration(CASTEnumDeclNode* pNode) override;

			TLLVMIRData VisitStructDeclaration(CASTStructDeclNode* pNode) override;

			TLLVMIRData VisitBreakOperator(CASTBreakOperatorNode* pNode) override;

			TLLVMIRData VisitContinueOperator(CASTContinueOperatorNode* pNode) override;

			TLLVMIRData VisitAccessOperator(CASTAccessOperatorNode* pNode) override;
			
			TLLVMIRData VisitArrayTypeNode(CASTArrayTypeNode* pNode) override;

			TLLVMIRData VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode) override;

			TLLVMIRData VisitPointerTypeNode(CASTPointerTypeNode* pNode) override;

			TLLVMIRData VisitImportDirectiveNode(CASTImportDirectiveNode* pNode) override;

			TLLVMIRData VisitDeferOperatorNode(CASTDeferOperatorNode* pNode) override;

			TLLVMIRData VisitIntrinsicCall(CASTIntrinsicCallNode* pNode) override;

			llvm::IRBuilder<>* GetCurrIRBuilder();

			llvm::IRBuilder<>* GetGlobalIRBuilder();

			ITypeVisitor<TLLVMIRData>* GetTypeGenerator() const override;
		protected:
			CLLVMCodeGenerator(const CLLVMCodeGenerator& codeGenerator) = default;

			llvm::Instruction::BinaryOps _convertOpTypeToLLVM(E_TOKEN_TYPE opType, bool isFloatingPointOp = false) const;

			llvm::CmpInst::Predicate _convertLogicOpTypeToLLVM(E_TOKEN_TYPE opType, bool isFloatingPointOp = false) const;

			llvm::Value* _getIdentifierValue(const std::string& identifier) const;

			llvm::Value* _allocateVariableOnStack(const std::string& identifier, bool isFuncArg = false);

			void _defineInitModuleGlobalsFunction();

			void _defineEntryPoint();

			llvm::Value* _declareNativeFunction(const TSymbolDesc* pFuncDesc);

			void _defineStructTypeConstructor(CStructType* pType);

			std::string _mangleGlobalModuleIdentifier(CType* pType, const std::string& identifier) const;

			inline bool _isGlobalScope() const;

			llvm::BasicBlock* _constructDeferBlock(TExpressionsStack& expressionsStack);

			std::string _extractIdentifier(CASTUnaryExpressionNode* pNode) const;
		protected:
			TLLVMLiteralVisitor*     mpLiteralIRGenerator;

			TLLVMTypeVisitor*        mpTypeGenerator;

			ITypeResolver*           mpTypeResolver;

			llvm::LLVMContext        mContext;

			llvm::Module*            mpModule;

			llvm::Function*          mpCurrActiveFunction;

			ISymTable*               mpSymTable;

			TIRBuidlersStack         mIRBuildersStack;

			llvm::IRBuilder<>*       mpGlobalIRBuilder;

			TValuesTable             mVariablesTable;

			llvm::Function*          mpInitModuleGlobalsFunction;

			llvm::IRBuilder<>*       mpInitModuleGlobalsIRBuilder;

			llvm::BasicBlock*        mpLoopConditionBlock;

			llvm::BasicBlock*        mpLoopEndBlock;

			IConstExprInterpreter*   mpConstExprInterpreter;

			bool                     mShouldSkipLoopTail;

			TDeferredExpressionStack mDefferedExpressionsStack;
	};
}

#endif
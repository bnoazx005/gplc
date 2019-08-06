/*!
	\author Ildar Kasimov
	\date   02.08.2019
	\copyright

	\brief The file contains implementation of interpreter of constant expression that
	is used to compute array size in compile time

	\todo
*/
#ifndef GPLC_CONST_EXPR_INTERPRETER_H
#define GPLC_CONST_EXPR_INTERPRETER_H


#include "gplcTypes.h"
#include "gplcVisitor.h"
#include "utils/CResult.h"


namespace gplc
{
	class ISymTable;
	class CASTExpressionNode;


	class IConstExprInterpreter : public IASTNodeVisitor<TLLVMIRData>
	{
		public:
			virtual TResult<U32> Eval(CASTExpressionNode* pExpr, ISymTable* pSymTable) = 0;
	};


	class CConstExprInterpreter : public IConstExprInterpreter
	{
		public:
			CConstExprInterpreter() = default;
			virtual ~CConstExprInterpreter() = default;

			TResult<U32> Eval(CASTExpressionNode* pExpr, ISymTable* pSymTable) override;

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
		protected:
			ISymTable* mpSymTable;
	};
}

#endif
/*!
	\author Ildar Kasimov
	\date   17.07.2019
	\copyright

	\brief The file contains definition of the transpiler into C language

	\todo
*/

#ifndef GPLC_Ñ_CODE_GENERATOR_H
#define GPLC_Ñ_CODE_GENERATOR_H


#include "codegen/gplcCodegen.h"
#include "common/gplcVisitor.h"
#include "lexer/gplcTokens.h"
#include <variant>
#include <string>


namespace gplc
{
	class CCTypeVisitor;
	class CCLiteralVisitor;
	class ITypeResolver;


	class CCCodeGenerator : public ICodeGenerator
	{
		public:
			CCCodeGenerator() = default;
			virtual ~CCCodeGenerator() = default;

			TLLVMIRData Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable, ITypeResolver* pTypeResolver, IConstExprInterpreter* pInterpreter) override;

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
		protected:
			CCCodeGenerator(const CCCodeGenerator& codeGenerator) = default;
		protected:
			ITypeResolver*    mpTypeResolver;

			ISymTable*        mpSymTable;

			CCTypeVisitor*    mpTypeVisitor;

			CCLiteralVisitor* mpLiteralVisitor;

			std::string       mGlobalDeclarationsContext;

			std::string       mGlobalDefinitionsContext;
	};
}

#endif
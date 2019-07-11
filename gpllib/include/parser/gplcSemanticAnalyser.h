/*!
	\author Ildar Kasimov
	\date   11.07.2016
	\copyright

	\brief The file contains semantic analyser's class declaration

	\todo
*/

#ifndef GPLC_SEMANTIC_ANALYSER_H
#define GPLC_SEMANTIC_ANALYSER_H


#include "common/gplcVisitor.h"


namespace gplc
{
	class CASTNode;
	class ISymTable;


	/*!
		\brief The interface describes a functionality of a semantic analyser stage
	*/

	class ISemanticAnalyser: public IVisitor<bool>
	{
		public:
			ISemanticAnalyser() = default;
			virtual ~ISemanticAnalyser() = default;
					   
			/*!
				\brief The method analyzes a given input AST node and 
				returns true if all statements are correct

				\return The method returns true if each statement in the AST is correct,
				false in other cases
			*/

			virtual bool Analyze(CASTNode* pInput, ISymTable* pSymTable) = 0;
	};


	class CSemanticAnalyser : public ISemanticAnalyser
	{
		public:
			CSemanticAnalyser() = default;
			virtual ~CSemanticAnalyser() = default;

			/*!
				\brief The method analyzes a given input AST node and
				returns true if all statements are correct

				\return The method returns true if each statement in the AST is correct,
				false in other cases
			*/

			bool Analyze(CASTNode* pInput, ISymTable* pSymTable) override;

			bool VisitProgramUnit(CASTNode* pProgramNode) override;

			bool VisitDeclaration(CASTDeclarationNode* pNode) override;

			bool VisitIdentifier(CASTIdentifierNode* pNode) override;

			bool VisitLiteral(CASTLiteralNode* pNode) override;

			bool VisitUnaryExpression(CASTUnaryExpressionNode* pNode) override;

			bool VisitBinaryExpression(CASTBinaryExpressionNode* pNode) override;

			bool VisitAssignment(CASTAssignmentNode* pNode) override;

			bool VisitStatementsBlock(CASTBlockNode* pNode) override;

			bool VisitIfStatement(CASTIfStatementNode* pNode) override;

			bool VisitLoopStatement(CASTLoopStatementNode* pNode) override;

			bool VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) override;

			bool VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) override;

			bool VisitFunctionClosure(CASTFunctionClosureNode* pNode) override;

			bool VisitFunctionArgs(CASTFunctionArgsNode* pNode) override;

			bool VisitFunctionCall(CASTFunctionCallNode* pNode) override;

			bool VisitReturnStatement(CASTReturnStatementNode* pNode) override;

			bool VisitDefinitionNode(CASTDefinitionNode* pNode) override;

			bool VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) override;
		protected:
			ISymTable* mpSymTable;
	};
}

#endif
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
#include "utils/Delegate.h"
#include "common/gplcTypes.h"
#include <functional>


namespace gplc
{
	class CASTNode;
	class ITypeResolver;
	class ISymTable;


	/*!
		\brief The interface describes a functionality of a semantic analyser stage
	*/

	class ISemanticAnalyser: public IASTNodeVisitor<bool>
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

			virtual bool Analyze(CASTNode* pInput, ITypeResolver* pTypeResolver, ISymTable* pSymTable) = 0;
		public:
			CDelegate<void, E_SEMANTIC_ANALYSER_ERRORS> OnErrorOutput;
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

			bool Analyze(CASTNode* pInput, ITypeResolver* pTypeResolver, ISymTable* pSymTable) override;

			bool VisitProgramUnit(CASTSourceUnitNode* pProgramNode) override;

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

			bool VisitEnumDeclaration(CASTEnumDeclNode* pNode) override;

			bool VisitStructDeclaration(CASTStructDeclNode* pNode) override;

			bool VisitBreakOperator(CASTBreakOperatorNode* pNode) override;

			bool VisitContinueOperator(CASTContinueOperatorNode* pNode) override;

			bool VisitAccessOperator(CASTAccessOperatorNode* pNode) override;

			bool VisitArrayTypeNode(CASTArrayTypeNode* pNode) override;

			bool VisitIndexedAccessOperator(CASTIndexedAccessOperatorNode* pNode) override;
		protected:
			bool _enterScope(CASTBlockNode* pNode, ISymTable* pSymTable);

			void _lockSymbolTable(const std::function<void()>& action, bool lockSymTable);

			bool _enterLoopScope(CASTBlockNode* pNode, ISymTable* pSymTable);

			bool _isLoopInterruptionAllowed() const;

			bool _containsBreak(CASTBlockNode* pLoopBody) const;
		protected:
			ITypeResolver* mpTypeResolver;

			ISymTable*     mpSymTable;
			
			bool           mLockSymbolTable;

			bool           mStayWithinLoop;
	};
}

#endif
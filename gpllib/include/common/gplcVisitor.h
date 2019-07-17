/*!
	\author Ildar Kasimov
	\date   08.07.2019
	\copyright

	\brief The file contains definitions of IASTNodeVisitor interface
*/

#ifndef GPLC_VISITOR_H
#define GPLC_VISITOR_H


#include <string>


namespace gplc
{
	class CASTNode;
	class CASTSourceUnitNode;
	class CASTDeclarationNode;
	class CASTIdentifierNode;
	class CASTLiteralNode;
	class CASTUnaryExpressionNode;
	class CASTBinaryExpressionNode;
	class CASTAssignmentNode;
	class CASTIfStatementNode;
	class CASTLoopStatementNode;
	class CASTWhileLoopStatementNode;
	class CASTFunctionDeclNode;
	class CASTFunctionClosureNode;
	class CASTFunctionArgsNode;
	class CASTFunctionCallNode;
	class CASTReturnStatementNode;
	class CASTDefinitionNode;
	class CASTFuncDefinitionNode;
	class CASTBlockNode;


	template <typename T>
	class IASTNodeVisitor
	{
		public:
			virtual ~IASTNodeVisitor() = default;

			virtual T VisitProgramUnit(CASTSourceUnitNode* pProgramNode) = 0;
			virtual T VisitDeclaration(CASTDeclarationNode* pNode) = 0;
			virtual T VisitIdentifier(CASTIdentifierNode* pNode) = 0;
			virtual T VisitLiteral(CASTLiteralNode* pNode) = 0;
			virtual T VisitUnaryExpression(CASTUnaryExpressionNode* pNode) = 0;
			virtual T VisitBinaryExpression(CASTBinaryExpressionNode* pNode) = 0;
			virtual T VisitAssignment(CASTAssignmentNode* pNode) = 0;
			virtual T VisitStatementsBlock(CASTBlockNode* pNode) = 0;
			virtual T VisitIfStatement(CASTIfStatementNode* pNode) = 0;
			virtual T VisitLoopStatement(CASTLoopStatementNode* pNode) = 0;
			virtual T VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode) = 0;
			virtual T VisitFunctionDeclaration(CASTFunctionDeclNode* pNode) = 0;
			virtual T VisitFunctionClosure(CASTFunctionClosureNode* pNode) = 0;
			virtual T VisitFunctionArgs(CASTFunctionArgsNode* pNode) = 0;
			virtual T VisitFunctionCall(CASTFunctionCallNode* pNode) = 0;
			virtual T VisitReturnStatement(CASTReturnStatementNode* pNode) = 0;
			virtual T VisitDefinitionNode(CASTDefinitionNode* pNode) = 0;
			virtual T VisitFunctionDefNode(CASTFuncDefinitionNode* pNode) = 0;
		protected:
			IASTNodeVisitor() = default;
			IASTNodeVisitor(const IASTNodeVisitor& visitor) = default;
	};


	template <typename T, typename TVisitor>
	class IVisitable
	{
		public:
			virtual ~IVisitable() = default;

			virtual T Accept(TVisitor* pVisitor) = 0;
		protected:
			IVisitable() = default;
			IVisitable(const IVisitable& visitable) = default;
	};


	class CType;
	class CIntLiteral;
	class CUIntLiteral;
	class CFloatLiteral;
	class CDoubleLiteral;
	class CCharLiteral;
	class CStringLiteral;
	class CBoolLiteral;


	template<typename T>
	class ILiteralVisitor
	{
		public:
			ILiteralVisitor() = default;
			virtual ~ILiteralVisitor() = default;

			virtual T VisitIntLiteral(const CIntLiteral* pLiteral) = 0;
			virtual T VisitUIntLiteral(const CUIntLiteral* pLiteral) = 0;
			virtual T VisitFloatLiteral(const CFloatLiteral* pLiteral) = 0;
			virtual T VisitDoubleLiteral(const CDoubleLiteral* pLiteral) = 0;
			virtual T VititCharLiteral(const CCharLiteral* pLiteral) = 0;
			virtual T VisitStringLiteral(const CStringLiteral* pLiteral) = 0;
			virtual T VisitBoolLiteral(const CBoolLiteral* pLiteral) = 0;
		protected:
			ILiteralVisitor(const ILiteralVisitor& visitor) = default;
	};


	class CType;
	class CFunctionType;


	template <typename T>
	class ITypeVisitor
	{
		public:
			ITypeVisitor() = default;
			virtual ~ITypeVisitor() = default;

			virtual T VisitBasicType(const CType* pType) = 0;
			virtual T VisitFunctionType(const CFunctionType* pFuncType) = 0;
		protected:
			ITypeVisitor(const ITypeVisitor& visitor) = default;
	};


	typedef IASTNodeVisitor<std::string> IStringASTNodeVisitor;
	typedef IASTNodeVisitor<bool>        IBoolASTNodeVisitor;
}


#endif
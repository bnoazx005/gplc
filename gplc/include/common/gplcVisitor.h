/*!
	\author Ildar Kasimov
	\date   08.07.2019
	\copyright

	\brief The file contains definitions of IVisitor interface
*/

#ifndef GPLC_VISITOR_H
#define GPLC_VISITOR_H


namespace gplc
{
	class CASTNode;
	class CASTIdentifierNode;
	class CASTLiteralNode;
	class CASTUnaryExpressionNode;
	class CASTBinaryExpressionNode;
	class CASTAssignmentNode;


	template <typename T>
	class IVisitor
	{
		public:
			virtual ~IVisitor() = default;

			virtual T VisitProgramUnit(CASTNode* pProgramNode) = 0;
			virtual T VisitIdentifier(CASTIdentifierNode* pNode) = 0;
			virtual T VisitLiteral(CASTLiteralNode* pNode) = 0;
			virtual T VisitUnaryExpression(CASTUnaryExpressionNode* pNode) = 0;
			virtual T VisitBinaryExpression(CASTBinaryExpressionNode* pNode) = 0;
			virtual T VisitAssignment(CASTAssignmentNode* pNode) = 0;
		protected:
			IVisitor() = default;
			IVisitor(const IVisitor& visitor) = default;
	};


	template <typename T>
	class IVisitable
	{
		public:
			virtual ~IVisitable() = default;

			virtual T Accept(IVisitor<T>* pVisitor) = 0;
		protected:
			IVisitable() = default;
			IVisitable(const IVisitable& visitable) = default;
	};
}


#endif
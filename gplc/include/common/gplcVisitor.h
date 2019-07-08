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
	template <typename T>
	class IVisitor
	{
		public:
			virtual ~IVisitor() = default;

			virtual T VisitProgramUnit(const CASTNode* pProgramNode) = 0;


		protected:
			IVisitor() = default;
			IVisitor(const IVisitor& visitor) = default;
	};
}


#endif
/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains CSymTable declaration

	\todo
*/
#ifndef GPLC_TYPE_SYSTEM_H
#define GPLC_TYPE_SYSTEM_H


#include "gplcTypes.h"
#include <vector>


namespace gplc
{
	/*!
		\brief CType class
	*/

	class CType
	{
		public:
			CType(E_COMPILER_TYPES type);
			virtual ~CType();

			E_COMPILER_TYPES GetType() const;

			bool IsBuiltIn() const;
		protected:
			CType();
			CType(const CType& type);
		protected:
			E_COMPILER_TYPES mType;

			std::vector<CType*> mChildren;
	};
}

#endif
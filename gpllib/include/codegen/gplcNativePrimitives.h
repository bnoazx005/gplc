/*!
	\author Ildar Kasimov
	\date   15.08.2019
	\copyright

	\brief The file contains definitions of built-in primitives

	\todo
*/

#ifndef GPLC_NATIVE_PRIMITIVES_H
#define GPLC_NATIVE_PRIMITIVES_H


#include "common/gplcTypes.h"
#include <functional>


namespace gplc
{
	class ICodeGenerator;
	class ITypesFactory;
	class ISymTable;
	class IASTNodesFactory;

	template <typename T>
	class ITypeVisitor;


	class INativeModules
	{
		public:
		public:
			INativeModules() = default;
			virtual ~INativeModules() = default;

			virtual Result InitModules(ISymTable* pSymTable, IASTNodesFactory* pNodesFactory, ITypesFactory* pTypesFactory) = 0;
		protected:
			INativeModules(const INativeModules&) = delete;
	};
}

#endif
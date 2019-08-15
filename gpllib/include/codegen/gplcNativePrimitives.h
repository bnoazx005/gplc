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


namespace gplc
{
	class ICodeGenerator;
	class ITypesFactory;

	template <typename T>
	class ITypeVisitor;


	class INativeModules
	{
		public:
			INativeModules() = default;
			virtual ~INativeModules() = default;

			virtual Result InitModule(ICodeGenerator* pCodeGenerator, ITypesFactory* pTypesFactory, ITypeVisitor<TLLVMIRData>* pTypeVisitor) = 0;
		protected:
			INativeModules(const INativeModules&) = delete;
	};
}

#endif
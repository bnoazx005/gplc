/*!
	\author Ildar Kasimov
	\date   15.08.2019
	\copyright

	\brief The file contains definitions of built-in primitives for LLVM code generator

	\todo
*/

#ifndef GPLC_LLVM_NATIVE_PRIMITIVES_H
#define GPLC_LLVM_NATIVE_PRIMITIVES_H


#include "gplcNativePrimitives.h"


namespace gplc
{
	class CLLVMNativeModules: public INativeModules
	{
		public:
			CLLVMNativeModules() = default;
			virtual ~CLLVMNativeModules() = default;

			Result InitModule(ICodeGenerator* pCodeGenerator, ITypesFactory* pTypesFactory, ITypeVisitor<TLLVMIRData>* pTypeVisitor) override;
		protected:
			CLLVMNativeModules(const CLLVMNativeModules&) = delete;
		protected:
			bool mIsInitialized;
	};
}

#endif
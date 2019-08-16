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

			Result InitModules(ISymTable* pSymTable, IASTNodesFactory* pNodesFactory, ITypesFactory* pTypesFactory) override;
		protected:
			CLLVMNativeModules(const CLLVMNativeModules&) = delete;

			Result _initStringType(ISymTable* pSymTable, IASTNodesFactory* pNodesFactory, ITypesFactory* pTypesFactory);
		protected:
			bool mIsInitialized;
	};
}

#endif
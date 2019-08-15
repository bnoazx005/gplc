#include "codegen/gplcLLVMNativePrimitives.h"
#include "codegen/gplcLLVMCodegen.h"
#include "codegen/gplcLLVMTypeVisitor.h"
#include "common/gplcTypesFactory.h"


namespace gplc
{
	Result CLLVMNativeModules::InitModule(ICodeGenerator* pCodeGenerator, ITypesFactory* pTypesFactory, ITypeVisitor<TLLVMIRData>* pTypeVisitor)
	{
		if (mIsInitialized)
		{
			return RV_SUCCESS;
		}

		mIsInitialized = true;

		return RV_SUCCESS;
	}
}
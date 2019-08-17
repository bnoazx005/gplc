#include "codegen/gplcLLVMLinker.h"
#include "llvm/Linker/Linker.h"


namespace gplc
{
	Result CLLVMLinker::Link()
	{
		// we need:
		// 1) all modules
		// 2) graph which tells relationships between main module and its dependencies

		// start from leaves and go up to the main module

		return RV_SUCCESS;
	}
}
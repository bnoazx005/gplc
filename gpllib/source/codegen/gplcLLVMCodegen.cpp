#include "codegen/gplcLLVMCodegen.h"


namespace gplc
{
	std::string CLLVMCodeGenerator::Generate(CASTSourceUnitNode* pNode)
	{
		llvm::LLVMContext llvmContext;

		llvm::Module* pSourceUnitModule = new llvm::Module("top", llvmContext);
		
		delete pSourceUnitModule;

		return {};
	}
}
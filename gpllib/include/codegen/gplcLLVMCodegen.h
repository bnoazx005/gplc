/*!
	\author Ildar Kasimov
	\date   15.07.2019
	\copyright

	\brief The file contains definition of the code generator based on LLVM

	\todo
*/

#ifndef GPLC_LLVM_CODE_GENERATOR_H
#define GPLC_LLVM_CODE_GENERATOR_H


#include "codegen/gplcCodegen.h"
#include "common/gplcVisitor.h"
#include "llvm\IR\Module.h"
#include <variant>


namespace gplc
{
	typedef std::variant<llvm::Module*, llvm::Value*, llvm::Function*, llvm::Instruction*> TLLVMIRData;


	class CLLVMCodeGenerator : public ICodeGenerator
	{
		public:
			CLLVMCodeGenerator() = default;
			virtual ~CLLVMCodeGenerator() = default;

			std::string Generate(CASTSourceUnitNode* pNode) override;
		protected:
			CLLVMCodeGenerator(const CLLVMCodeGenerator& codeGenerator) = default;
	};
}

#endif
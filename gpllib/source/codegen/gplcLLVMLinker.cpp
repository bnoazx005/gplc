#include "codegen/gplcLLVMLinker.h"
#include "llvm/Linker/Linker.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/ADT/Optional.h"
#include "llvm/IR/LegacyPassManager.h"
#include <filesystem>
#include <algorithm>
#include <memory>


namespace gplc
{
	Result CLLVMLinker::Link(const std::string& outputFilename, TModulesArray& modulesRegistry, TModuleEntry* pMainModuleDependencies, bool isMainModule)
	{
		Result result = RV_SUCCESS;

		// we need:
		// 1) all modules
		// 2) graph which tells relationships between main module and its dependencies

		// start from leaves and go up to the main module
		llvm::Module* pMainModule = _getModule(modulesRegistry, pMainModuleDependencies->mModuleName);

		for (auto currDependency : pMainModuleDependencies->mDependencies)
		{
			if (currDependency.mDependencies.empty())
			{
				// \note we make a copy of original leaf module, but override its parent one
				if (llvm::Linker::linkModules(*pMainModule, std::move(llvm::CloneModule(*_getModule(modulesRegistry, currDependency.mModuleName))), llvm::Linker::OverrideFromSrc))
				{
					return RV_FAIL;
				}

				continue;
			}

			if (!SUCCESS(result = Link(outputFilename, modulesRegistry, &currDependency)))
			{
				return result;
			}
		}

		// call system linker
		if (isMainModule)
		{
			if (!SUCCESS(result = _generateObjectFile(pMainModule, pMainModuleDependencies->mModuleName)))
			{
				return result;
			}

			// \todo For now we use clang to call system linker
			system(std::string("clang ")
						.append(pMainModuleDependencies->mModuleName)
						.append(".obj")
						.append(" -o ")
						.append(outputFilename).c_str());
		}
		
		return RV_SUCCESS;
	}

	llvm::Module* CLLVMLinker::_getModule(TModulesArray& modulesRegistry, const std::string& name) const
	{
		auto iter = std::find_if(modulesRegistry.cbegin(), modulesRegistry.cend(), [&name](const IModuleResolver::TCompiledModuleData& moduleData)
		{
			return moduleData.mModuleName == name;
		});

		return (iter == modulesRegistry.cend()) ? nullptr : std::get<llvm::Module*>(iter->mCompiledIRCode);
	}

	Result CLLVMLinker::_generateObjectFile(llvm::Module* pModule, const std::string& filename)
	{
		std::error_code EC;

		llvm::raw_fd_ostream out(std::filesystem::path(filename).replace_extension("obj").string(), EC, llvm::sys::fs::F_None);

		// \todo Refactor this part later, define some class that will return assembly code for specific target machine
		auto targetTriple = llvm::sys::getDefaultTargetTriple();

		std::string errorMsg;

		auto pTarget = llvm::TargetRegistry::lookupTarget(targetTriple, errorMsg);

		if (!pTarget)
		{
			return RV_FAIL;
		}

		const C8* CPU      = "generic";
		const C8* features = "";

		llvm::TargetOptions targetOptions;

		auto RM = llvm::Optional<llvm::Reloc::Model>();

		auto targetMachine = pTarget->createTargetMachine(targetTriple, CPU, features, targetOptions, RM);

		// \todo move it from here to some proper place
		pModule->setDataLayout(targetMachine->createDataLayout());
		pModule->setTargetTriple(targetTriple);

		llvm::legacy::PassManager passManager;

		if (targetMachine->addPassesToEmitFile(passManager, out, nullptr, llvm::TargetMachine::CGFT_ObjectFile))
		{
			return RV_FAIL;
		}

		passManager.run(*pModule);

		out.flush();

		return RV_SUCCESS;
	}
}
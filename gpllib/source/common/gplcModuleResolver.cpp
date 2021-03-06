#include "common/gplcModuleResolver.h"
#include "common/gplcSymTable.h"
#include "common/gplcTypesFactory.h"
#include "common/gplcTypeSystem.h"
#include "parser/gplcASTNodes.h"
#include "codegen/gplcLinker.h"
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>


namespace gplc
{
	std::string CModuleResolver::mFileExtension { ".gpls" };

	CModuleResolver::CModuleResolver():
		mpCurrVisitingModule(&mRootModuleDeps)
	{
	}

	Result CModuleResolver::Resolve(CASTSourceUnitNode* pModuleAST, ISymTable* pSymTable, ITypesFactory* pTypesFactory, const std::string& currentWorkinDir,
									const TOnCompileCallback& onCompileModule)
	{
		if (!pModuleAST || !pSymTable || !pTypesFactory)
		{
			return RV_INVALID_ARGUMENTS;
		}

		mpSymTable = pSymTable;

		mpTypesFactory = pTypesFactory;

		mCurrentWorkingDir = currentWorkinDir;

		mOnCompileModuleCallback = onCompileModule;

		const std::string& currModuleName = pModuleAST->GetModuleName();

		// if modules registry already contains information about the module exit
		auto iter = std::find_if(mModulesRegistry.begin(), mModulesRegistry.end(), [&currModuleName](const TCompiledModuleData& m)
		{
			return m.mModuleName == currModuleName;
		});

		if (iter != mModulesRegistry.cend())
		{
			return RV_SUCCESS;
		}

		ResolveModuleType(mpSymTable, mpTypesFactory, currModuleName);

		mModulesRegistry.push_back({ currModuleName, pModuleAST });

		// push back a top level module
		*mpCurrVisitingModule = { currModuleName, {} };

		return _visitNode(pModuleAST);
	}

	void CModuleResolver::ResolveModuleType(ISymTable* pSymTable, ITypesFactory* pTypesFactory, const std::string& moduleName)
	{
		auto pCurrModuleEntry = pSymTable->LookUpNamedScope(moduleName);

		if (!pCurrModuleEntry->mpType)
		{
			pCurrModuleEntry->mpType = pTypesFactory->CreateModuleType(moduleName, 0x0, pSymTable->GetParentScopeType());
		}
	}

	Result CModuleResolver::Link(const std::string& outputFilename, ILinker* pLinker, bool skipFinalLinking)
	{
		return pLinker->Link(outputFilename, mModulesRegistry, &mRootModuleDeps, true, skipFinalLinking);
	}

	CModuleResolver::TCompiledModuleData& CModuleResolver::GetModuleEntry(const std::string& moduleName)
	{
		auto iter = std::find_if(mModulesRegistry.begin(), mModulesRegistry.end(), [&moduleName](IModuleResolver::TCompiledModuleData& moduleData)
		{
			return moduleData.mModuleName == moduleName;
		});

		return (*iter);
	}

	void CModuleResolver::DumpDependencyGraph() const
	{
		_printDependencyGraphLevel(&mRootModuleDeps);
	}

	void CModuleResolver::_printDependencyGraphLevel(const TModuleEntry* pCurrModuleEntry, U32 currLevel) const
	{
		for (U32 i = 0; i < currLevel; ++i)
		{
			std::cout << "    ";
		}

		std::cout << pCurrModuleEntry->mModuleName << std::endl;

		for (const auto pCurrDependency : pCurrModuleEntry->mDependencies)
		{
			_printDependencyGraphLevel(&pCurrDependency, currLevel + 1);
		}
	}

	Result CModuleResolver::_visitNode(CASTNode* pNode)
	{
		Result result = RV_SUCCESS;

		for (auto pCurrChild : pNode->GetChildren())
		{
			if (!pCurrChild)
			{
				continue;
			}

			result = (pCurrChild->GetType() == NT_IMPORT) ? _resolveImport(dynamic_cast<CASTImportDirectiveNode*>(pCurrChild)) : _visitNode(pCurrChild);

			if (!SUCCESS(result))
			{
				return result;
			}
		}

		return RV_SUCCESS;
	}

	Result CModuleResolver::_resolveImport(CASTImportDirectiveNode* pNode)
	{		
		std::string moduleFullPath = std::filesystem::path(mCurrentWorkingDir).append(std::string(pNode->GetModulePath()).append(mFileExtension)).string();

		if (!std::filesystem::exists(moduleFullPath))
		{
			return RV_FILE_NOT_FOUND;
		}

		const std::string& moduleName = pNode->GetImportedModuleName();
		
		mpSymTable->CreateNamedScope(moduleName);

		ResolveModuleType(mpSymTable, mpTypesFactory, moduleName);

		// build dependencies graph
		TModuleEntry currModuleEntry { moduleName, {} };

		mpCurrVisitingModule->mDependencies.push_back(currModuleEntry);

		auto pCurrModuleDeps = mpCurrVisitingModule;
		mpCurrVisitingModule = &mpCurrVisitingModule->mDependencies.back();

		Result result = RV_SUCCESS;

		TLLVMIRData compiledModuleData;

		if (!SUCCESS(result = mOnCompileModuleCallback(moduleFullPath, moduleName, compiledModuleData)))
		{
			return result;
		}

		mpCurrVisitingModule = pCurrModuleDeps;

		mpSymTable->LeaveScope();

		// extract module's data from the registry
		auto currModuleIter = std::find_if(mModulesRegistry.begin(), mModulesRegistry.end(), [&moduleName](const TCompiledModuleData& m)
		{
			return m.mModuleName == moduleName;
		});

		assert(currModuleIter != mModulesRegistry.cend());

		currModuleIter->mCompiledIRCode = std::move(compiledModuleData);

		return RV_SUCCESS;
	}
}
#include "common/gplcModuleResolver.h"
#include "common/gplcSymTable.h"
#include "parser/gplcASTNodes.h"
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cassert>


namespace gplc
{
	std::string CModuleResolver::mFileExtension { ".gpls" };

	CModuleResolver::CModuleResolver():
		mpCurrVisitingModule(&mRootModuleDeps)
	{
	}

	Result CModuleResolver::Resolve(CASTSourceUnitNode* pModuleAST, ISymTable* pSymTable, const std::string& currentWorkinDir,
									const TOnCompileCallback& onCompileModule)
	{
		if (!pModuleAST || !pSymTable)
		{
			return RV_INVALID_ARGUMENTS;
		}

		mpSymTable = pSymTable;

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

		mModulesRegistry.push_back({ currModuleName, pModuleAST });

		// push back a top level module
		*mpCurrVisitingModule = { currModuleName, {} };

		return _visitNode(pModuleAST);
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

		Result result = RV_SUCCESS;

		TLLVMIRData compiledModuleData;

		if (!SUCCESS(result = mOnCompileModuleCallback(moduleFullPath, moduleName, compiledModuleData)))
		{
			return result;
		}

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
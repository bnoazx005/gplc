/*!
	\author Ildar Kasimov
	\date   11.08.2019
	\copyright

	\brief The file contains definition of a module resolving system

	\todo 
*/
#ifndef GPLC_MODULE_RESOLVER_H
#define GPLC_MODULE_RESOLVER_H


#include "gplcTypes.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>


namespace gplc
{
	class ISymTable;
	class CASTSourceUnitNode;
	class CASTNode;
	class CASTImportDirectiveNode;


	class IModuleResolver
	{
		public:
			typedef std::function<Result (const std::string&, const std::string&, TLLVMIRData&)> TOnCompileCallback;
	
			typedef struct TModuleEntry
			{
				typedef std::vector<TModuleEntry> TDependenciesArray;

				std::string        mModuleName;

				TDependenciesArray mDependencies;

			} TModuleEntry, *TModuleEntryPtr;

			typedef struct TCompiledModuleData
			{
				std::string         mModuleName;

				CASTSourceUnitNode* mpModuleAST;

				TLLVMIRData         mCompiledIRCode;
			} TCompiledModuleData, *TCompiledModuleDataPtr;

			typedef std::vector<TCompiledModuleData> TModulesArray;
		public:
			IModuleResolver() = default;
			virtual ~IModuleResolver() = default;

			virtual Result Resolve(CASTSourceUnitNode* pModuleAST, ISymTable* pSymTable, const std::string& currentWorkinDir,
								   const TOnCompileCallback& onCompileModule) = 0;
		protected:
			IModuleResolver(const IModuleResolver&) = delete;
	};


	class CModuleResolver: public IModuleResolver
	{
		public:
			CModuleResolver();
			virtual ~CModuleResolver() = default;

			Result Resolve(CASTSourceUnitNode* pModuleAST, ISymTable* pSymTable, const std::string& currentWorkinDir,
						   const TOnCompileCallback& onCompileModule) override;
		protected:
			CModuleResolver(const CModuleResolver&) = delete;

			Result _visitNode(CASTNode* pNode);

			Result _resolveImport(CASTImportDirectiveNode* pNode);
		protected:
			static std::string mFileExtension;

			ISymTable*         mpSymTable;

			std::string        mCurrentWorkingDir;

			TOnCompileCallback mOnCompileModuleCallback;

			TModuleEntry       mRootModuleDeps;

			TModuleEntry*      mpCurrVisitingModule;

			TModulesArray      mModulesRegistry;
	};
}

#endif
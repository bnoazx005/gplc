/*!
	\author Ildar Kasimov
	\date   14.08.2019
	\copyright

	\brief The file contains declaration of an interface of a linker

	\todo
*/

#ifndef GPLC_LINKER_H
#define GPLC_LINKER_H


#include "common/gplcTypes.h"
#include "common/gplcModuleResolver.h"
#include <vector>
#include <string>


namespace gplc
{
	class ILinker
	{
		public:
			typedef IModuleResolver::TModulesArray TModulesArray;
			typedef IModuleResolver::TModuleEntry  TModuleEntry;
		public:
			ILinker() = default;
			virtual ~ILinker() = default;

			virtual Result Link(const std::string& outputFilename, TModulesArray& modulesRegistry, TModuleEntry* pMainModuleDependencies, bool isMainModule = false, 
								bool skipFinalLinking = false) = 0;
		protected:
			ILinker(const ILinker&) = delete;
	};
}

#endif
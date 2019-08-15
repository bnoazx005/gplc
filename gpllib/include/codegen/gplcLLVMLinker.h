/*!
	\author Ildar Kasimov
	\date   14.08.2019
	\copyright

	\brief The file contains declaration of an interface of a LLVM linker

	\todo
*/

#ifndef GPLC_LLVM_LINKER_H
#define GPLC_LLVM_LINKER_H


#include "gplcLinker.h"
#include "common/gplcTypes.h"
#include <vector>


namespace gplc
{
	class CLLVMLinker: public ILinker
	{
		public:
			CLLVMLinker() = default;
			virtual ~CLLVMLinker() = default;

			Result Link() override;
		protected:
			CLLVMLinker(const CLLVMLinker&) = delete;
	};
}

#endif
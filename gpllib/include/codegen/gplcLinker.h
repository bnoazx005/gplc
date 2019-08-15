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
#include <vector>


namespace gplc
{
	class ILinker
	{
		public:
			ILinker() = default;
			virtual ~ILinker() = default;

			virtual Result Link() = 0;
		protected:
			ILinker(const ILinker&) = delete;
	};
}

#endif
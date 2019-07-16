/*!
	\author Ildar Kasimov
	\date   15.07.2019
	\copyright

	\brief The file contains declaration of an interface of a code generator

	\todo
*/

#ifndef GPLC_CODE_GENERATOR_H
#define GPLC_CODE_GENERATOR_H


#include "common/gplcVisitor.h"
#include <string>


namespace gplc
{
	class ICodeGenerator
	{
		public:
			ICodeGenerator() = default;
			virtual ~ICodeGenerator() = default;

			virtual std::string Generate(CASTSourceUnitNode* pNode) = 0;
		protected:
			ICodeGenerator(const ICodeGenerator& codeGenerator) = default;
	};
}

#endif
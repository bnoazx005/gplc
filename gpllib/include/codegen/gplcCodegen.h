/*!
	\author Ildar Kasimov
	\date   15.07.2019
	\copyright

	\brief The file contains declaration of an interface of a code generator

	\todo
*/

#ifndef GPLC_CODE_GENERATOR_H
#define GPLC_CODE_GENERATOR_H


#include "common/gplcTypes.h"
#include "common/gplcVisitor.h"
#include <string>


namespace gplc
{
	class ISymTable;


	class ICodeGenerator: public IASTNodeVisitor<TLLVMIRData>
	{
		public:
			ICodeGenerator() = default;
			virtual ~ICodeGenerator() = default;

			virtual TLLVMIRData Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable) = 0;
		protected:
			ICodeGenerator(const ICodeGenerator& codeGenerator) = default;
	};
}

#endif
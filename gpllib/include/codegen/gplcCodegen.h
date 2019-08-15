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
#include <functional>
#include <string>


namespace gplc
{
	class ISymTable;
	class ITypeResolver;
	class IConstExprInterpreter;

	template <typename T> class ITypeVisitor;


	class ICodeGenerator: public IASTNodeVisitor<TLLVMIRData>
	{
		public:
			typedef std::function<Result(ICodeGenerator*)> TOnPreGenerateCallback;
		public:
			ICodeGenerator() = default;
			virtual ~ICodeGenerator() = default;

			virtual TLLVMIRData Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable, ITypeResolver* pTypeResolver, IConstExprInterpreter* pInterpreter,
										 const TOnPreGenerateCallback& onPreGenerateCallback) = 0;

			virtual ITypeVisitor<TLLVMIRData>* GetTypeGenerator() const = 0;
		protected:
			ICodeGenerator(const ICodeGenerator& codeGenerator) = default;
	};
}

#endif
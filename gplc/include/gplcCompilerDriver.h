/*!
	\author Ildar Kasimov
	\date   07.08.2019
	\copyright

	\brief The file contains definition of main compiler's class that runs all processes
	within it

	\todo
*/
#pragma once


#include "gplcCommon.h"
#include <gplc.h>
#include <vector>
#include <string>


namespace gplc
{
	class ICompilerDriver
	{
		public:
			typedef std::vector<std::string> TStringsArray;
		public:
			virtual Result Init(const TCompilerOptions& options) = 0;
			virtual Result Free() = 0;

			virtual Result Run(const TStringsArray& inputFiles) = 0;
		protected:
			ICompilerDriver() = default;
			ICompilerDriver(const ICompilerDriver&) = delete;
			virtual ~ICompilerDriver() = default;
	};


	class CCompilerDriver: public ICompilerDriver
	{
		public:
			friend TResult<ICompilerDriver*> CreateCompilerDriver(const TCompilerOptions&);
		public:
			Result Init(const TCompilerOptions& options) override;
			Result Free() override;

			Result Run(const TStringsArray& inputFiles) override;
		protected:
			CCompilerDriver() = default;
			CCompilerDriver(const CCompilerDriver&) = delete;
			virtual ~CCompilerDriver() = default;
		
			void _onLexerStageError(const TLexerErrorInfo& errorInfo);

			void _onParserStageError(const TParserErrorInfo& errorInfo);

			void _onSemanticAnalyserStageError(const TSemanticAnalyserMessageInfo& errorInfo);

			Result _compileSeparateFile(const std::string& filename, const std::string& moduleName, TLLVMIRData& compiledModuleData);

			std::string _getCurrentWorkingDirectory(const TStringsArray& inputFiles) const;

			void _outputCompilationUnit(const std::string& filename, llvm::Module& module) const;

			void _initLLVMInfrastructure() const;
		protected:
			bool                   mIsInitialized;

			ILexer*                mpLexer;

			IParser*               mpParser;

			ISymTable*             mpSymTable;

			ISemanticAnalyser*     mpSemanticAnalyser;

			IASTNodesFactory*      mpASTNodesFactory;

			ITypeResolver*         mpTypeResolver;

			IConstExprInterpreter* mpConstExprInterpreter;

			ICodeGenerator*        mpCodeGenerator;

			IModuleResolver*       mpModuleResolver;

			ITypesFactory*         mpTypesFactory;

			INativeModules*        mpNativeModules;

			bool                   mIsPanicModeEnabled;

			TCompilerOptions       mCompilerOptions;
	};


	TResult<ICompilerDriver*> CreateCompilerDriver(const TCompilerOptions&);
}
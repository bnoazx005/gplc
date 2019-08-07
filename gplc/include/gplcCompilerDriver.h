/*!
	\author Ildar Kasimov
	\date   07.08.2019
	\copyright

	\brief The file contains definition of main compiler's class that runs all processes
	within it

	\todo
*/
#pragma once


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
			virtual Result Init() = 0;
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
			friend TResult<ICompilerDriver*> CreateCompilerDriver();
		public:
			Result Init() override;
			Result Free() override;

			Result Run(const TStringsArray& inputFiles) override;
		protected:
			CCompilerDriver() = default;
			CCompilerDriver(const CCompilerDriver&) = delete;
			virtual ~CCompilerDriver() = default;
		
			void _onLexerStageError(const TLexerErrorInfo& errorInfo);

			void _onParserStageError(const TParserErrorInfo& errorInfo);

			void _onSemanticAnalyserStageError(const TSemanticAnalyserMessageInfo& errorInfo);

			Result _compileSeparateFile(const std::string& filename);
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

			bool                   mIsPanicModeEnabled;
	};


	TResult<ICompilerDriver*> CreateCompilerDriver();
}
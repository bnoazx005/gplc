#include "gplcCompilerDriver.h"
#include <iostream>
#include <filesystem>


namespace gplc
{
	Result CCompilerDriver::Init()
	{
		Result result = RV_SUCCESS;

		if (mIsInitialized)
		{
			return RV_SUCCESS;
		}

		mpLexer                = new CLexer();
		mpParser               = new CParser();
		mpSymTable             = new CSymTable();
		mpSemanticAnalyser     = new CSemanticAnalyser();
		mpASTNodesFactory      = new CASTNodesFactory();
		mpTypeResolver         = new CTypeResolver();
		mpConstExprInterpreter = new CConstExprInterpreter();
		mpCodeGenerator        = new CLLVMCodeGenerator();
		mpModuleResolver       = new CModuleResolver();
		mpTypesFactory         = new CTypesFactory();

		if (!SUCCESS(result = mpTypeResolver->Init(mpSymTable, mpConstExprInterpreter, mpTypesFactory)))
		{
			return result;
		}

		mpLexer->OnErrorOutput            += MakeMethodDelegate(this, &CCompilerDriver::_onLexerStageError);
		mpParser->OnErrorOutput           += MakeMethodDelegate(this, &CCompilerDriver::_onParserStageError);
		mpSemanticAnalyser->OnErrorOutput += MakeMethodDelegate(this, &CCompilerDriver::_onSemanticAnalyserStageError);

		// \todo reorganize this stuff later
		mpSymTable->AddVariable({ "puts", nullptr, mpTypesFactory->CreateFunctionType({ { "str", mpTypesFactory->CreateType(CT_STRING, BTS_POINTER, 0x0) } }, 
																					  mpTypesFactory->CreateType(CT_INT32, BTS_INT32, 0x0), AV_NATIVE_FUNC) });

		mIsInitialized = true;

		return RV_SUCCESS;
	}

	Result CCompilerDriver::Free()
	{
		if (!mIsInitialized)
		{
			return RV_FAIL;
		}

		delete mpTypesFactory;
		delete mpModuleResolver;
		delete mpLexer;
		delete mpParser;
		delete mpSymTable;
		delete mpSemanticAnalyser;
		delete mpASTNodesFactory;
		delete mpTypeResolver;
		delete mpConstExprInterpreter;
		delete mpCodeGenerator;

		mIsInitialized = false;

		delete this;

		return RV_SUCCESS;
	}

	Result CCompilerDriver::Run(const TStringsArray& inputFiles)
	{
		if (!mIsInitialized)
		{
			return RV_FAIL;
		}

		if (inputFiles.empty())
		{
			return RV_INVALID_ARGUMENTS;
		}

		Result result = RV_SUCCESS;

		TLLVMIRData compiledProgram;

		for (auto currFilename : inputFiles)
		{
			mIsPanicModeEnabled = false;

			auto moduleName = currFilename.substr(0, currFilename.find_first_of('.'));

			mpSymTable->CreateNamedScope(moduleName);

			if (!SUCCESS(result = _compileSeparateFile(currFilename, moduleName, compiledProgram)))
			{
				mpSymTable->LeaveScope();

				return result;
			}

			mpSymTable->LeaveScope();
		}

		return RV_SUCCESS;
	}

	Result CCompilerDriver::_compileSeparateFile(const std::string& filename, const std::string& moduleName, TLLVMIRData& compiledModuleData)
	{
		Result result = mpLexer->Reset();

		mpModuleResolver->ResolveModuleType(mpSymTable, mpTypesFactory, moduleName);

		std::cout << "gplc: Compiling " << moduleName << " ..." << std::endl;
		
		if (!SUCCESS(result))
		{
			return result;
		}

		IInputStream* pInputStream = new CFileInputStream(filename);

		auto disposeInputStream = [&pInputStream]()
		{
			pInputStream->Close();

			delete pInputStream;
		};

		// open a file's data
		if (!SUCCESS(result = pInputStream->Open()))
		{
			disposeInputStream();

			return result;
		}

		// initialize the lexer
		if (!SUCCESS(result = mpLexer->Init(pInputStream)))
		{
			disposeInputStream();

			return result;
		}

		// parse the source file
		CASTSourceUnitNode* pSourceAST = dynamic_cast<CASTSourceUnitNode*>(mpParser->Parse(mpLexer, mpSymTable, mpASTNodesFactory, mpTypesFactory, moduleName));

		if (mIsPanicModeEnabled)
		{
			disposeInputStream();

			return RV_FAIL;
		}

		// \todo resolve all modules here
		if (!SUCCESS(mpModuleResolver->Resolve(pSourceAST, mpSymTable, mpTypesFactory, std::filesystem::current_path().string(), 
											   std::bind(&CCompilerDriver::_compileSeparateFile, this, 
														 std::placeholders::_1, 
														 std::placeholders::_2,
														 std::placeholders::_3))))
		{
			return RV_FAIL;
		}

		// analyse its semantic
		if (!mpSemanticAnalyser->Analyze(pSourceAST, mpTypeResolver, mpSymTable, mpASTNodesFactory))
		{
			disposeInputStream();

			return RV_FAIL;
		}

		if (mIsPanicModeEnabled)
		{
			disposeInputStream();

			return RV_FAIL;
		}

		// emit IR code
		compiledModuleData = mpCodeGenerator->Generate(pSourceAST, mpSymTable, mpTypeResolver, mpConstExprInterpreter);

		disposeInputStream();

		return RV_SUCCESS;
	}

	void CCompilerDriver::_onLexerStageError(const TLexerErrorInfo& errorInfo)
	{
		mIsPanicModeEnabled = true;

		std::cout << "Error: (" << errorInfo.mPos << ";" << errorInfo.mLine << ") " << CMessageOutputUtils::LexerMessageToString(errorInfo.mErrorType) << std::endl;
	}

	void CCompilerDriver::_onParserStageError(const TParserErrorInfo& errorInfo)
	{
		mIsPanicModeEnabled = true;

		std::cout << "Error: (" << errorInfo.mPos << ";" << errorInfo.mLine << ") " << CMessageOutputUtils::ParserMessageToString(errorInfo) << std::endl;
	}

	void CCompilerDriver::_onSemanticAnalyserStageError(const TSemanticAnalyserMessageInfo& errorInfo)
	{
		mIsPanicModeEnabled = (errorInfo.mType == E_MESSAGE_TYPE::MT_ERROR);

		std::cout << CMessageOutputUtils::MessageTypeToString(errorInfo.mType) << ": " << CMessageOutputUtils::SemanticAnalyserMessageToString(errorInfo.mMessage) << std::endl;
	}


	TResult<ICompilerDriver*> CreateCompilerDriver()
	{
		ICompilerDriver* pCompilerDriver = new CCompilerDriver();

		Result result = pCompilerDriver->Init();

		if (!SUCCESS(result))
		{
			pCompilerDriver->Free();

			return TErrorValue<E_RESULT_VALUE>(static_cast<E_RESULT_VALUE>(result));
		}

		return TOkValue(pCompilerDriver);
	}
}
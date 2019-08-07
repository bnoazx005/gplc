#include "gplcCompilerDriver.h"
#include <iostream>


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

		if (!SUCCESS(result = mpTypeResolver->Init(mpSymTable, mpConstExprInterpreter)))
		{
			return result;
		}

		mpLexer->OnErrorOutput            += MakeMethodDelegate(this, &CCompilerDriver::_onLexerStageError);
		mpParser->OnErrorOutput           += MakeMethodDelegate(this, &CCompilerDriver::_onParserStageError);
		mpSemanticAnalyser->OnErrorOutput += MakeMethodDelegate(this, &CCompilerDriver::_onSemanticAnalyserStageError);

		// \todo reorganize this stuff later
		mpSymTable->AddVariable({ "puts", nullptr, new CFunctionType({ { "str", new CType(CT_STRING, BTS_POINTER, 0x0) } }, new CType(CT_INT32, BTS_INT32, 0x0), AV_NATIVE_FUNC) });

		mIsInitialized = true;

		return RV_SUCCESS;
	}

	Result CCompilerDriver::Free()
	{
		if (!mIsInitialized)
		{
			return RV_FAIL;
		}

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

		for (auto currFilename : inputFiles)
		{
			mIsPanicModeEnabled = false;

			if (!SUCCESS(result = _compileSeparateFile(currFilename)))
			{
				return result;
			}
		}

		return RV_SUCCESS;
	}

	Result CCompilerDriver::_compileSeparateFile(const std::string& filename)
	{
		Result result = mpLexer->Reset();

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
		CASTSourceUnitNode* pSourceAST = dynamic_cast<CASTSourceUnitNode*>(mpParser->Parse(mpLexer, mpSymTable, mpASTNodesFactory, filename));

		if (mIsPanicModeEnabled)
		{
			disposeInputStream();

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
		mpCodeGenerator->Generate(pSourceAST, mpSymTable, mpTypeResolver, mpConstExprInterpreter);

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
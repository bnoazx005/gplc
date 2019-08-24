#include "gplcCompilerDriver.h"
#include "gplcCommon.h"
#include "llvm/IR/Module.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/ADT/Optional.h"
#include "llvm/IR/LegacyPassManager.h"
#include <iostream>
#include <filesystem>


namespace gplc
{
	Result CCompilerDriver::Init(const TCompilerOptions& options)
	{
		Result result = RV_SUCCESS;

		if (mIsInitialized)
		{
			return RV_SUCCESS;
		}

		mCompilerOptions = options;

		_initLLVMInfrastructure();
		
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
		mpNativeModules        = new CLLVMNativeModules();

		if (!SUCCESS(result = mpTypeResolver->Init(mpSymTable, mpConstExprInterpreter, mpTypesFactory)))
		{
			return result;
		}

		mpLexer->OnErrorOutput            += MakeMethodDelegate(this, &CCompilerDriver::_onLexerStageError);
		mpParser->OnErrorOutput           += MakeMethodDelegate(this, &CCompilerDriver::_onParserStageError);
		mpSemanticAnalyser->OnErrorOutput += MakeMethodDelegate(this, &CCompilerDriver::_onSemanticAnalyserStageError);

		//if (!SUCCESS(result = mpNativeModules->InitModules(mpSymTable, mpASTNodesFactory, mpTypesFactory)))
		//{
		//	return result;
		//}

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

		delete mpNativeModules;
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

		// extract directory with which we will associate current working directory
		std::string currentWorkingDirectory = _getCurrentWorkingDirectory(inputFiles);

		for (auto currFilename : inputFiles)
		{
			mIsPanicModeEnabled = false;

			// check whether the specified file exist
			if (!std::filesystem::exists(currFilename))
			{
				std::cout << GetRedConsoleText("Error: ").append("Source file not found (").append(currFilename).append(")") << std::endl;

				mIsPanicModeEnabled = true;

				continue;
			}
			
			currFilename    = std::filesystem::path(currFilename).filename().string(); // extract filename without precedence path
			auto moduleName = currFilename.substr(0, currFilename.find_first_of('.'));

			mpSymTable->CreateNamedScope(moduleName);

			if (!SUCCESS(result = _compileSeparateFile(currFilename, moduleName, compiledProgram)))
			{
				mpSymTable->LeaveScope();

				return result;
			}

			mpSymTable->LeaveScope();

			if (mIsPanicModeEnabled)
			{
				return RV_FAIL;
			}

			mpModuleResolver->GetModuleEntry(moduleName).mCompiledIRCode = std::move(compiledProgram);

			auto pLinker = new CLLVMLinker();

			// \todo Implement proper way to append extension for an output file
			mCompilerOptions.mOutputFilename.append(".exe");

			if (!SUCCESS(result = mpModuleResolver->Link(mCompilerOptions.mOutputFilename, pLinker, mCompilerOptions.mEmitFlag != E_EMIT_FLAGS::EF_NONE)))
			{
				delete pLinker;

				return result;
			}

			if (mCompilerOptions.mEmitFlag != E_EMIT_FLAGS::EF_NONE)
			{
				_outputCompilationUnit(!mCompilerOptions.mOutputFilename.empty() ? mCompilerOptions.mOutputFilename : moduleName,
									   *std::get<llvm::Module*>(compiledProgram));
			}

			delete pLinker;
		}

		// \note dump scope's structure on user's demand
		if (!mIsPanicModeEnabled && (mCompilerOptions.mPrintFlags & PF_SYMTABLE_DUMP))
		{
			mpSymTable->DumpScopesStructure();
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
		compiledModuleData = mpCodeGenerator->Generate(pSourceAST, mpSymTable, mpTypeResolver, mpConstExprInterpreter, [](ICodeGenerator* pCodeGenerator)
		{
			return RV_SUCCESS;
		});

		std::cout << "gplc: Compiling " << moduleName << " (Finished)" << std::endl;

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

	std::string CCompilerDriver::_getCurrentWorkingDirectory(const TStringsArray& inputFiles) const
	{
		if (inputFiles.empty())
		{
			return "";
		}

		return std::filesystem::path(inputFiles.front()).parent_path().string();
	}

	void CCompilerDriver::_outputCompilationUnit(const std::string& filename, llvm::Module& module) const
	{
		std::error_code EC;

		llvm::raw_fd_ostream out(std::filesystem::path(filename)
										.replace_extension(EmitFlagsToExtensionString(mCompilerOptions.mEmitFlag))
										.string(), 
								 EC, llvm::sys::fs::F_None);

		switch (mCompilerOptions.mEmitFlag)
		{
			case E_EMIT_FLAGS::EF_LLVM_BC:
				{
					WriteBitcodeToFile(module, out);
					out.flush();
				}
				break;
			case E_EMIT_FLAGS::EF_LLVM_IR:
				{
					module.print(out, nullptr);
					out.flush();
				}	
				break;
			case E_EMIT_FLAGS::EF_ASM:
				{
					// \todo Refactor this part later, define some class that will return assembly code for specific target machine
					auto targetTriple = llvm::sys::getDefaultTargetTriple();

					std::string errorMsg;

					auto pTarget = llvm::TargetRegistry::lookupTarget(targetTriple, errorMsg);

					if (!pTarget) 
					{
						Panic(GetRedConsoleText("Error: ").append("Couldn't find specified target"));
					}

					const C8* CPU      = "generic";
					const C8* features = "";

					llvm::TargetOptions targetOptions;

					auto RM = llvm::Optional<llvm::Reloc::Model>();
					
					auto targetMachine = pTarget->createTargetMachine(targetTriple, CPU, features, targetOptions, RM);

					// \todo move it from here to some proper place
					module.setDataLayout(targetMachine->createDataLayout());
					module.setTargetTriple(targetTriple);

					llvm::legacy::PassManager passManager;

					if (targetMachine->addPassesToEmitFile(passManager, out, nullptr, llvm::TargetMachine::CGFT_AssemblyFile)) 
					{
						Panic(GetRedConsoleText("Error: ").append("Couldn't emit code for specified target"));
					}

					passManager.run(module);

					out.flush();
				}
				break;
		}
	}

	void CCompilerDriver::_initLLVMInfrastructure() const
	{
		llvm::InitializeAllTargetInfos();
		llvm::InitializeAllTargets();
		llvm::InitializeAllTargetMCs();
		llvm::InitializeAllAsmParsers();
		llvm::InitializeAllAsmPrinters();

		if (mCompilerOptions.mPrintFlags & PF_COMPILER_TARGETS)
		{
			auto indent = [](U8 count) -> std::string
			{
				return std::string().append(count, ' ');
			};

			auto allTargets = llvm::TargetRegistry::targets();

			std::string currTargetName;

			std::cout << "Registered targets:\n";

			for (auto currTarget : allTargets)
			{
				currTargetName = currTarget.getName();

				std::cout << "\t-- " << currTargetName << indent(12 - currTargetName.length()) << " - " << currTarget.getShortDescription() << std::endl;
			}
		}
	}


	TResult<ICompilerDriver*> CreateCompilerDriver(const TCompilerOptions& compilerOptions)
	{
		ICompilerDriver* pCompilerDriver = new CCompilerDriver();

		Result result = pCompilerDriver->Init(compilerOptions);

		if (!SUCCESS(result))
		{
			pCompilerDriver->Free();

			return TErrorValue<E_RESULT_VALUE>(static_cast<E_RESULT_VALUE>(result));
		}

		return TOkValue(pCompilerDriver);
	}
}
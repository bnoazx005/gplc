#include <iostream>
#include <fstream>
#include <gplc.h>


using namespace std;
using namespace gplc;


void OnLexerError(const TLexerErrorInfo& errorInfo)
{
	std::cout << "(" << errorInfo.mPos << ";" << errorInfo.mLine << ") "<< errorInfo.mPos << std::endl;
}


void OnParserError(const TParserErrorInfo& errorInfo)
{
	std::cout << "(" << errorInfo.mPos << ";" << errorInfo.mLine << ") " << errorInfo.mMessage << std::endl;
}


void OnSemanticAnalyserError(E_SEMANTIC_ANALYSER_ERRORS errorInfo)
{
	std::cout << errorInfo << std::endl;
}


int main(int argc, const char** argv)
{
	// \todo parse options
	ILexer* pLexer = new CLexer();
	IParser* pParser = new CParser();
	ISymTable* pSymTable = new CSymTable();

	pSymTable->AddVariable({ "puts", nullptr, new CFunctionType({ { "str", new CType(CT_STRING, BTS_POINTER, 0x0) } }, new CType(CT_INT32, BTS_INT32, 0x0), AV_NATIVE_FUNC ) });

	ISemanticAnalyser* pSemanticAnalyser = new CSemanticAnalyser();

	pLexer->OnErrorOutput += OnLexerError;
	pSemanticAnalyser->OnErrorOutput += OnSemanticAnalyserError;

	IInputStream* pInputStream = new CFileInputStream("test.gpls");

	if (!SUCCESS(pLexer->Init(pInputStream)))
	{
		delete pParser;
		delete pLexer;
		delete pSymTable;

		return -1;
	}

	pParser->OnErrorOutput += OnParserError;

	CASTSourceUnitNode* pSourceAST = dynamic_cast<CASTSourceUnitNode*>(pParser->Parse(pLexer, pSymTable));

	ITypeResolver* pTypeResolver = new CTypeResolver();

	IConstExprInterpreter* pInterpreter = new CConstExprInterpreter();

	if (!SUCCESS(pTypeResolver->Init(pSymTable, pInterpreter)))
	{
		system("pause");

		return -1;
	}

	bool result = pSemanticAnalyser->Analyze(pSourceAST, pTypeResolver, pSymTable);

	if (!result)
	{
		system("pause");

		return -1;
	}

	ICodeGenerator* pCodeGenerator = new CLLVMCodeGenerator();//new CCCodeGenerator();

	pCodeGenerator->Generate(pSourceAST, pSymTable, pTypeResolver);
	//std::string transformedSource = std::get<std::string>(pCodeGenerator->Generate(pSourceAST, pSymTable));

	//std::ofstream out("main.c");

	//out << transformedSource;

	//out.close();

	system("clang main.c -o main.exe");

	delete pInterpreter;
	delete pInputStream;
	delete pCodeGenerator;
	delete pTypeResolver;
	delete pSemanticAnalyser;
	delete pSymTable;
	delete pParser;
	delete pLexer;

	return 0;
}
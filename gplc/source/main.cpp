#include <iostream>
#include <fstream>
#include <gplc.h>


using namespace std;
using namespace gplc;


void OnLexerError(const TLexerErrorInfo& errorInfo)
{
	std::cout << errorInfo.mPos << std::endl;
}


void OnParserError(const TParserErrorInfo& errorInfo)
{
	std::cout << errorInfo.mMessage << std::endl;
}


int main(int argc, const char** argv)
{
	// \todo parse options
	//ILexer* pLexer = new CLexer();
	//IParser* pParser = new CParser();
	//ISymTable* pSymTable = new CSymTable();
	//ISemanticAnalyser* pSemanticAnalyser = new CSemanticAnalyser();

	//std::ifstream inputFile("test.gpls");
	//
	//std::istreambuf_iterator<char> begin(inputFile), end;
	//std::string sourceCode(begin, end);

	//pLexer->OnErrorOutput += OnLexerError;

	//if (!SUCCESS(pLexer->Init(sourceCode/*argv[1]*/, "./../../configs/.tokens")))
	//{
	//	delete pParser;
	//	delete pLexer;
	//	delete pSymTable;

	//	return -1;
	//}

	//pParser->OnErrorOutput += OnParserError;

	//CASTSourceUnitNode* pSourceAST = dynamic_cast<CASTSourceUnitNode*>(pParser->Parse(pLexer, pSymTable));

	//ITypeResolver* pTypeResolver = new CTypeResolver();

	//bool result = pSemanticAnalyser->Analyze(pSourceAST, pTypeResolver, pSymTable);

	//ICodeGenerator* pCodeGenerator = new CCCodeGenerator();

	//std::string transformedSource = std::get<std::string>(pCodeGenerator->Generate(pSourceAST, pSymTable));

	//std::ofstream out("main.c");

	//out << transformedSource;

	//out.close();

	//delete pCodeGenerator;
	//delete pTypeResolver;
	//delete pSemanticAnalyser;
	//delete pSymTable;
	//delete pParser;
	//delete pLexer;

	return 0;
}
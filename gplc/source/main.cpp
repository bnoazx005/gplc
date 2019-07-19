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
	ILexer* pLexer = new CLexer();
	IParser* pParser = new CParser();
	ISymTable* pSymTable = new CSymTable();

	std::ifstream inputFile("test.gpls");
	
	std::istreambuf_iterator<char> begin(inputFile), end;
	std::string sourceCode(begin, end);

	pLexer->OnErrorOutput += OnLexerError;

	if (!SUCCESS(pLexer->Init(sourceCode/*argv[1]*/, "./../../configs/.tokens")))
	{
		delete pParser;
		delete pLexer;
		delete pSymTable;

		return -1;
	}

	pParser->OnErrorOutput += OnParserError;

	auto pSourceAST = pParser->Parse(pLexer, pSymTable);

	delete pParser;
	delete pLexer;

	return 0;
}
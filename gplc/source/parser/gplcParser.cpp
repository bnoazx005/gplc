/*!
	\author Ildar Kasimov
	\date   17.07.2016
	\copyright

	\brief The file contains parser's class defenition

	\todo
*/

#include "parser\gplcParser.h"
#include "parser\gplcASTNodes.h"
#include "lexer\gplcLexer.h"


namespace gplc
{
	/*!
		CParser defenition
	*/

	CParser::CParser() :
		IParser()
	{
	}

	CParser::CParser(const CParser& parser) :
		IParser(parser)
	{
	}

	CParser::~CParser()
	{
	}

	CASTNode* CParser::Parse(const ILexer* lexer, TParserErrorInfo* errorInfo)
	{
		if (lexer == nullptr)
		{
			errorInfo = new TParserErrorInfo();
			
			errorInfo->mErrorCode = RV_INVALID_ARGUMENTS;
			errorInfo->mMessage   = "A pointer to lexer equals to null";

			return nullptr;
		}

		errorInfo = nullptr;

		if (lexer->GetCurrToken() == nullptr) //returns just an empty program unit
		{
			return new CASTNode(NT_PROGRAM_UNIT);
		}

		return _parseProgramUnit(lexer, errorInfo);
	}

	/*!
		\brief Try to parse the following grammar rule.

		<program-unit> ::= <statements>;

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return A pointer to node of a program unit
	*/

	CASTNode* CParser::_parseProgramUnit(const ILexer* lexer, TParserErrorInfo* errorInfo)
	{
		CASTNode* pProgramUnit = new CASTNode(NT_PROGRAM_UNIT);

		CASTNode* pStatements = _parseStatementsList(lexer, errorInfo);

		if (errorInfo != nullptr || pStatements == nullptr)
		{
			return pProgramUnit;
		}

		pProgramUnit->AttachChildren(pStatements->GetChildren());

		return pProgramUnit;
	}

	CASTNode* CParser::_parseStatementsList(const ILexer* lexer, TParserErrorInfo* errorInfo)
	{
		return _parseStatement(lexer, errorInfo);
	}

	CASTNode* CParser::_parseStatement(const ILexer* lexer, TParserErrorInfo* errorInfo)
	{
		return _parseOperator(lexer, errorInfo);
	}

	CASTNode* CParser::_parseOperator(const ILexer* lexer, TParserErrorInfo* errorInfo)
	{
		return _parseDeclaration(lexer, errorInfo);
	}

	CASTNode* CParser::_parseDeclaration(const ILexer* lexer, TParserErrorInfo* errorInfo)
	{
		return new CASTNode(NT_DECL);
	}
}
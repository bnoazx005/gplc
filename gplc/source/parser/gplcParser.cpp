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

	CASTNode* CParser::Parse(ILexer* lexer, TParserErrorInfo* &errorInfo)
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

	Result CParser::_expect(E_TOKEN_TYPE expectedValue, const CToken* currValue, TParserErrorInfo* &errorInfo)
	{
		if (currValue == nullptr)
		{
			errorInfo = new TParserErrorInfo();

			errorInfo->mErrorCode = RV_INVALID_ARGUMENTS;

			return RV_FAIL;
		}

		E_TOKEN_TYPE currValueType = currValue->GetType();

		if (expectedValue == currValueType)
		{
			errorInfo = nullptr;

			return RV_SUCCESS;
		}

		errorInfo = new TParserErrorInfo();
		
		C8 tmpStrBuf[255];

		sprintf_s(tmpStrBuf, sizeof(C8) * 255, "An unexpected token was found at %d. %d instead of %d\0", currValue->GetPos(), currValueType, expectedValue);

		errorInfo->mMessage   = tmpStrBuf;
		errorInfo->mErrorCode = RV_UNEXPECTED_TOKEN;
		errorInfo->mPos       = currValue->GetPos();

		return RV_UNEXPECTED_TOKEN;
	}

	/*!
		\brief Try to parse the following grammar rule.

		<program-unit> ::= <statements>;

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return A pointer to node of a program unit
	*/

	CASTNode* CParser::_parseProgramUnit(ILexer* lexer, TParserErrorInfo* &errorInfo)
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

	/*!
		\brief Try to parse the list of statements.

		<statements> ::=   <statement>
		                 | <statement> <statements>;

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return A pointer to node with a statements list
	*/

	CASTNode* CParser::_parseStatementsList(ILexer* lexer, TParserErrorInfo* &errorInfo)
	{
		CASTNode* pStatementsList = new CASTNode(NT_STATEMENTS);
		CASTNode* pCurrStatement  = nullptr;

		while (pCurrStatement = _parseStatement(lexer, errorInfo))
		{
			pStatementsList->AttachChild(pCurrStatement);
		}

		return pStatementsList;
	}

	/*!
		\brief Try to parse a single statement

		<statement> ::= <operator> ; ;

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return A pointer to node with a particular statement
	*/

	CASTNode* CParser::_parseStatement(ILexer* lexer, TParserErrorInfo* &errorInfo)
	{
		CASTNode* pOperator = _parseOperator(lexer, errorInfo);

		_expect(TT_SEMICOLON, lexer->GetCurrToken(), errorInfo);

		lexer->GetNextToken(); //get ;
		
		return pOperator;
	}

	/*!
		\brief Try to parse a single operator

		<operator> ::= <declaration>;

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return A pointer to node with an operator
	*/

	CASTNode* CParser::_parseOperator(ILexer* lexer, TParserErrorInfo* &errorInfo)
	{
		CASTNode* pOperator = nullptr;

		if ((pOperator = _parseDeclaration(lexer, errorInfo)))
		{
			return pOperator;
		}
		else
		{
			errorInfo = new TParserErrorInfo();

			errorInfo->mMessage = "Unrecognized tokens sequence";
			errorInfo->mMessage = RV_UNRECOGNIZED_TOKENS_SEQ;
			errorInfo->mPos     = lexer->GetCurrToken()->GetPos();

			return nullptr;
		}

		return nullptr;
	}

	/*!
		\brief Try to parse a declaration

		<declaration> ::=   <identifiers> : <attributes> <type>
		                  | <identifiers> : <type>;

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return A pointer to node with a declaration
	*/

	CASTNode* CParser::_parseDeclaration(ILexer* lexer, TParserErrorInfo* &errorInfo)
	{
		CASTNode* pDeclaration = new CASTNode(NT_DECL);

		//check the rule accordance
		const CToken* pTmpToken = nullptr;

		const CIdentifierToken* pIdToken = nullptr;

		U32 i = 0;

		do 
		{
			if (pTmpToken != nullptr)
			{
				pIdToken = dynamic_cast<const CIdentifierToken*>(pTmpToken);

				pDeclaration->AttachChild(new CASTIdentifierNode(pIdToken->GetName()));
			}

			pTmpToken = lexer->PeekNextToken(i++);
		}
		while (pTmpToken->GetType() == TT_IDENTIFIER);

		if ((pDeclaration->GetChildrenCount() < 1) || (lexer->PeekNextToken(i)->GetType() == TT_COLON && lexer->PeekNextToken(i + 1)->GetType() != TT_ASSIGN_OP)) // it's not a declaration
		{
			return nullptr;
		}

		for (U32 k = 0; k < i + 2; k++)
		{
			lexer->GetNextToken(); //get all passed tokens
		}

		//parse type

		//store into sym table

		//const CIdentifierToken* pIdentifierToken = dynamic_cast<const CIdentifierToken*>(lexer->GetCurrToken());

		//pDeclaration->AttachChild(new CASTIdentifierNode(pIdentifierToken->GetName())); //attach the identifier
		//
		//if (_expect(TT_COLON, lexer->GetNextToken(), errorInfo) != RV_SUCCESS)
		//{
		//	return pDeclaration;
		//}

		//pDeclaration->AttachChild(_parseType(lexer, errorInfo));

		////Add information we've got into a symbols' table
		////pSymbolTable->Add(pIdTok->GetName, pType)
		
		return pDeclaration;
	}

	/*!
		\brief Try to parse a type

		<type> ::=   <builtin_type>
		           | <identifier>
		           | <struct_declaration>
		           | <enum_declaration>
		           | <func_declaration>;

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return A pointer to node describes type
	*/

	CASTNode* _parseType(ILexer* lexer, TParserErrorInfo* &errorInfo)
	{
		return nullptr;
	}
}
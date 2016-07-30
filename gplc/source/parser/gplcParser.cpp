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
#include "common\gplcTypeSystem.h"


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
		
		if (errorInfo != nullptr)
		{
			if (pStatements != nullptr)
			{
				delete pStatements;
			}
			
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

		if (!SUCCESS(_expect(TT_SEMICOLON, lexer->GetCurrToken(), errorInfo)))
		{
			C8 tmpStrBuf[255];

			sprintf_s(tmpStrBuf, sizeof(C8) * 255, "Missing ; at %d\0", errorInfo->mPos);
			errorInfo->mMessage = tmpStrBuf;

			return pOperator;
		}

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

		lexer->SavePosition(); // save the current position at the input stream

		//check the rule accordance
		const CToken* pTmpToken = nullptr;

		const CIdentifierToken* pIdToken = nullptr;
		
		do 
		{
			if (pTmpToken != nullptr)
			{
				pIdToken = dynamic_cast<const CIdentifierToken*>(pTmpToken);

				pDeclaration->AttachChild(new CASTIdentifierNode(pIdToken->GetName()));
			}

			pTmpToken = lexer->GetNextToken();
		}
		while (pTmpToken->GetType() == TT_IDENTIFIER);

		U32 numOfReadIdentifiers = pDeclaration->GetChildrenCount();

		if ((numOfReadIdentifiers < 1)) // it's not a declaration
		{
			return nullptr;
		}

		if (!SUCCESS(_expect(TT_COLON, lexer->GetCurrToken(), errorInfo)))
		{
			return nullptr;
		}
		
		CASTNode* pTypeNode = _parseType(lexer, errorInfo);

		
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

		\return A pointer to node with a type
	*/

	CASTNode* CParser::_parseType(ILexer* lexer, TParserErrorInfo* &errorInfo)
	{
		CASTNode* pOperator = nullptr;

		if ((pOperator = _parseBuiltInType(lexer, errorInfo)))
		{
			return pOperator;
		}
		else
		{
			errorInfo = new TParserErrorInfo();

			errorInfo->mMessage = "Unrecognized tokens sequence";
			errorInfo->mMessage = RV_UNRECOGNIZED_TOKENS_SEQ;
			errorInfo->mPos = lexer->GetCurrToken()->GetPos();

			return nullptr;
		}

		return nullptr;
	}

	/*!
		\brief Try to parse a type

		<builtin_type> ::=   <intX>
						   | <uintX>
						   | <float>
						   | <double>
						   | <char>
						   | <string>
						   | <bool>
						   | <void>
						   | <pointer>
						   | <array>

		\param[in] lexer A pointer to lexer's object
		\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

		\return  A pointer to node with a builtin type
	*/

	CASTNode* CParser::_parseBuiltInType(ILexer* lexer, TParserErrorInfo* &errorInfo)
	{
		const CToken* pTypeName  = lexer->GetCurrToken();
		const CToken* pNextToken = lexer->PeekNextToken(1);

		CASTNode* pBuiltinType = nullptr;

		switch (pNextToken->GetType())
		{
			case TT_STAR: // a pointer

				pBuiltinType = new CASTNode(NT_POINTER);

				switch (pTypeName->GetType())
				{
					case TT_INT8_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_INT8));
						break;

					case TT_INT16_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_INT16));
						break;

					case TT_INT32_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_INT32));
						break;

					case TT_INT64_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_INT64));
						break;

					case TT_UINT8_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_UINT8));
						break;

					case TT_UINT16_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_UINT16));
						break;

					case TT_UINT32_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_UINT32));
						break;

					case TT_UINT64_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_UINT64));
						break;

					case TT_CHAR_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_CHAR));
						break;

					case TT_STRING_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_STRING));
						break;

					case TT_BOOL_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_BOOL));
						break;

					case TT_VOID_TYPE:
						pBuiltinType->AttachChild(new CASTNode(NT_VOID));
						break;
				}

				break;

			case TT_OPEN_SQR_BRACE: // an array



				break;

			default: //just a builtin type

				switch (pTypeName->GetType())
				{
					case TT_INT8_TYPE:
						pBuiltinType = new CASTNode(NT_INT8);
						break;

					case TT_INT16_TYPE:
						pBuiltinType = new CASTNode(NT_INT16);
						break;

					case TT_INT32_TYPE:
						pBuiltinType = new CASTNode(NT_INT32);
						break;

					case TT_INT64_TYPE:
						pBuiltinType = new CASTNode(NT_INT64);
						break;

					case TT_UINT8_TYPE:
						pBuiltinType = new CASTNode(NT_UINT8);
						break;

					case TT_UINT16_TYPE:
						pBuiltinType = new CASTNode(NT_UINT16);
						break;

					case TT_UINT32_TYPE:
						pBuiltinType = new CASTNode(NT_UINT32);
						break;

					case TT_UINT64_TYPE:
						pBuiltinType = new CASTNode(NT_UINT64);
						break;

					case TT_CHAR_TYPE:
						pBuiltinType = new CASTNode(NT_CHAR);
						break;

					case TT_STRING_TYPE:
						pBuiltinType = new CASTNode(NT_STRING);
						break;

					case TT_BOOL_TYPE:
						pBuiltinType = new CASTNode(NT_BOOL);
						break;

					case TT_VOID_TYPE:
						pBuiltinType = new CASTNode(NT_VOID);
						break;
				}

				break;
		}

		return pBuiltinType;
	}
}
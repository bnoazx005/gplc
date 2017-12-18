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

	CASTNode* CParser::Parse(ILexer* lexer)
	{
		if (lexer == nullptr)
		{
			TParserErrorInfo errorInfo;

			memset(&errorInfo, 0, sizeof(errorInfo));
						
			errorInfo.mErrorCode = RV_INVALID_ARGUMENTS;
			errorInfo.mMessage   = "A pointer to lexer equals to null";
			
			OnErrorOutput.Invoke(errorInfo);
			
			return nullptr;
		}
		
		if (lexer->GetCurrToken() == nullptr) //returns just an empty program unit
		{
			return new CASTNode(NT_PROGRAM_UNIT);
		}

		return _parseProgramUnit(lexer);
	}

	Result CParser::_expect(E_TOKEN_TYPE expectedValue, const CToken* currValue)
	{
		if (currValue == nullptr)
		{
			TParserErrorInfo errorInfo;

			memset(&errorInfo, 0, sizeof(errorInfo));

			errorInfo.mErrorCode = RV_INVALID_ARGUMENTS;

			OnErrorOutput.Invoke(errorInfo);

			return RV_FAIL;
		}

		E_TOKEN_TYPE currValueType = currValue->GetType();

		if (expectedValue == currValueType)
		{
			return RV_SUCCESS;
		}

		TParserErrorInfo errorInfo;

		memset(&errorInfo, 0, sizeof(errorInfo));
				
		C8 tmpStrBuf[255];

		sprintf_s(tmpStrBuf, sizeof(C8) * 255, "An unexpected token was found at %d. %d instead of %d\0", currValue->GetPos(), currValueType, expectedValue);

		errorInfo.mMessage   = tmpStrBuf;
		errorInfo.mErrorCode = RV_UNEXPECTED_TOKEN;
		errorInfo.mPos       = currValue->GetPos();

		OnErrorOutput.Invoke(errorInfo);

		return RV_UNEXPECTED_TOKEN;
	}

	/*!
		\brief Try to parse the following grammar rule.

		<program-unit> ::= <statements>;

		\param[in] lexer A pointer to lexer's object

		\return A pointer to node of a program unit
	*/

	CASTNode* CParser::_parseProgramUnit(ILexer* lexer)
	{
		CASTNode* pProgramUnit = new CASTNode(NT_PROGRAM_UNIT);

		CASTNode* pStatements = _parseStatementsList(lexer);
		
		if (pStatements)
		{
			pProgramUnit->AttachChildren(pStatements->GetChildren());
		}

		return pProgramUnit;
	}

	/*!
		\brief Try to parse the list of statements.

		<statements> ::=   <statement>
		                 | <statement> <statements>;

		\param[in] lexer A pointer to lexer's object

		\return A pointer to node with a statements list
	*/

	CASTNode* CParser::_parseStatementsList(ILexer* lexer)
	{
		CASTNode* pStatementsList = new CASTNode(NT_STATEMENTS);
		CASTNode* pCurrStatement  = nullptr;

		while (pCurrStatement = _parseStatement(lexer))
		{
			pStatementsList->AttachChild(pCurrStatement);
		}
		
		return pStatementsList;
	}

	/*!
		\brief Try to parse a single statement

		<statement> ::= <operator> ; ;

		\param[in] lexer A pointer to lexer's object

		\return A pointer to node with a particular statement
	*/

	CASTNode* CParser::_parseStatement(ILexer* lexer)
	{
		CASTNode* pOperator = _parseOperator(lexer);

		/*if (!SUCCESS(_expect(TT_SEMICOLON, lexer->GetCurrToken())))
		{
			C8 tmpStrBuf[255];

			sprintf_s(tmpStrBuf, sizeof(C8) * 255, "Missing ; at %d\0", errorInfo->mPos);
			errorInfo->mMessage = tmpStrBuf;

			return pOperator;
		}*/

		lexer->GetNextToken(); //get ;
		
		return pOperator;
	}

	/*!
		\brief Try to parse a single operator

		<operator> ::= <declaration>;

		\param[in] lexer A pointer to lexer's object

		\return A pointer to node with an operator
	*/

	CASTNode* CParser::_parseOperator(ILexer* lexer)
	{
		CASTNode* pOperator = nullptr;

		if ((pOperator = _parseDeclaration(lexer)))
		{
			return pOperator;
		}

		/*if ((pOperator = _parseDeclaration(lexer)))
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
*/
		return nullptr;
	}

	/*!
		\brief Try to parse a declaration

		<declaration> ::=   <identifiers> : <attributes> <type>
		                  | <identifiers> : <type>;

		\param[in] lexer A pointer to lexer's object

		\return A pointer to node with a declaration
	*/

	CASTNode* CParser::_parseDeclaration(ILexer* lexer)
	{
		CASTNode* pDeclaration = nullptr;

		if ((pDeclaration = _parseIdentifiersDecl(lexer)))
		{
			return pDeclaration;
		}
		else
		{
			return nullptr;
		}

		return pDeclaration;
	}

	CASTNode* CParser::_parseIdentifiersDecl(ILexer* lexer)
	{
		CASTNode* pDeclaration = nullptr;

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

		//if (numOfReadIdentifiers < 1 || 
		//	!SUCCESS(_expect(TT_COLON, lexer->GetCurrToken(), errorInfo)) ||
		//	lexer->GetNextToken()->GetType() == TT_ASSIGN_OP) // it's not a declaration
		//{
		//	lexer->RestorePosition();

		//	return nullptr;
		//}
		//
		//CASTNode* pTypeNode = nullptr;
		//
		//if (!(pTypeNode = _parseType(lexer, errorInfo)))
		//{
		//	lexer->RestorePosition();

		//	return nullptr;
		//}

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

		\return A pointer to node with a type
	*/

	CASTNode* CParser::_parseType(ILexer* lexer)
	{
		CASTNode* pOperator = nullptr;
/*
		if ((pOperator = _parseBuiltInType(lexer)))
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
		}*/

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

		\return  A pointer to node with a builtin type
	*/

	CASTNode* CParser::_parseBuiltInType(ILexer* lexer)
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
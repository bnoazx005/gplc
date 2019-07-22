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
#include "common/gplcSymTable.h"


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

	CASTNode* CParser::Parse(ILexer* pLexer, ISymTable* pSymTable)
	{
		if (pLexer == nullptr || pSymTable == nullptr)
		{
			TParserErrorInfo errorInfo;

			memset(&errorInfo, 0, sizeof(errorInfo));
						
			errorInfo.mErrorCode = RV_INVALID_ARGUMENTS;
			errorInfo.mMessage   = "A pointer to either pLexer or pSymTable equals to null";
			
			OnErrorOutput.Invoke(errorInfo);
			
			return nullptr;
		}
		
		mpSymTable = pSymTable;

		if (pLexer->GetCurrToken() == nullptr) //returns just an empty program unit
		{
			return new CASTNode(NT_PROGRAM_UNIT);
		}

		return _parseProgramUnit(pLexer);
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

		errorInfo.mMessage = std::string("An unexpected token was found at ")
															.append(std::to_string(currValue->GetPos()))
															.append(". ")
															.append(TokenTypeToString(currValueType))
															.append(" instead of ")
															.append(TokenTypeToString(expectedValue));
		
		errorInfo.mErrorCode = RV_UNEXPECTED_TOKEN;
		errorInfo.mPos       = currValue->GetPos();

		OnErrorOutput.Invoke(errorInfo);

		return RV_UNEXPECTED_TOKEN;
	}

	/*!
		\brief Try to parse the following grammar rule.

		<program-unit> ::= <statements>;

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node of a program unit
	*/

	CASTNode* CParser::_parseProgramUnit(ILexer* pLexer)
	{
		CASTNode* pProgramUnit = new CASTSourceUnitNode();

		CASTNode* pStatements = _parseStatementsList(pLexer);
		
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

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node with a statements list
	*/

	CASTNode* CParser::_parseStatementsList(ILexer* pLexer)
	{
		CASTNode* pStatementsList = new CASTNode(NT_STATEMENTS);
		CASTNode* pCurrStatement  = nullptr;

		while (pCurrStatement = _parseStatement(pLexer))
		{
			pStatementsList->AttachChild(pCurrStatement);
		}
		
		return pStatementsList;
	}

	/*!
		\brief Try to parse a single statement

		<statement> ::= <operator>     |
						<block>        |
						<if-statement> |
						<loop-statement> |
						<while-loop-stmt> |
						<return-stmt>

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node with a particular statement
	*/

	CASTNode* CParser::_parseStatement(ILexer* pLexer)
	{
		CASTNode* pStatementNode = nullptr;

		if (_match(pLexer->GetCurrToken(), TT_OPEN_BRACE))
		{
			pLexer->GetNextToken();

			pStatementNode = _parseBlockStatements(pLexer);

			if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
			{
				return nullptr;
			}

			return pStatementNode;
		}
		
		if (_match(pLexer->GetCurrToken(), TT_IF_KEYWORD))
		{
			pLexer->GetNextToken();

			return _parseIfStatement(pLexer);
		}
		
		if (_match(pLexer->GetCurrToken(), TT_LOOP_KEYWORD))
		{
			pLexer->GetNextToken();

			return _parseLoopStatement(pLexer);
		}
		
		if (_match(pLexer->GetCurrToken(), TT_WHILE_KEYWORD))
		{
			pLexer->GetNextToken();

			return _parseWhileLoopStatement(pLexer);
		}
		
		if (_match(pLexer->GetCurrToken(), TT_RETURN_KEYWORD))
		{
			pLexer->GetNextToken();

			pStatementNode = _parseReturnStatement(pLexer);
		}
		else
		{
			pStatementNode = _parseOperator(pLexer);
		}

		// all statements should ends up with ';' delimiter
		if (pStatementNode)
		{
			_expect(TT_SEMICOLON, pLexer->GetCurrToken());

			pLexer->GetNextToken();
		}

		return pStatementNode;
	}

	CASTBlockNode* CParser::_parseBlockStatements(ILexer* pLexer)
	{
		CASTBlockNode* pBlockNode = new CASTBlockNode();

		CASTNode* pStatements = _parseStatementsList(pLexer);

		if (pStatements)
		{
			pBlockNode->AttachChildren(pStatements->GetChildren());
		}

		return pBlockNode;
	}

	/*!
		\brief Try to parse a single operator

		<operator> ::= <declaration> |
					   <assignment> |
					   <function-call>;

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node with an operator
	*/

	CASTNode* CParser::_parseOperator(ILexer* pLexer)
	{
		CASTNode* pOperator = nullptr;

		if (_match(pLexer->GetCurrToken(), TT_IDENTIFIER))
		{
			if (_match(pLexer->PeekNextToken(1), TT_COMMA) || _match(pLexer->PeekNextToken(1), TT_COLON))
			{
				pOperator = _parseDeclaration(pLexer);
			}
			else if (_match(pLexer->PeekNextToken(1), TT_ASSIGN_OP))
			{
				pOperator = _parseAssignment(pLexer);
			}
			else
			{
				CASTUnaryExpressionNode* pUnaryExpr = _parseUnaryExpression(pLexer); 

				CASTNode* pFuncCallNode = pUnaryExpr->GetData();

				if (pFuncCallNode != nullptr && pFuncCallNode->GetType() == NT_FUNC_CALL) // return if the node is a function's call
				{
					return pFuncCallNode;
				}
			}
		}

		return pOperator;
	}

	/*!
		\brief Try to parse a declaration

		<declaration> ::=   <identifiers> : <attributes> <type>
		                  | <identifiers> : <type>;

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node with a declaration
	*/

	CASTNode* CParser::_parseDeclaration(ILexer* pLexer, U32 attributes)
	{
		CASTNode* pIdentifiers = _parseIdentifiers(pLexer, attributes);

		if (!SUCCESS(_expect(TT_COLON, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken();

		CASTNode* pTypeInfo = nullptr;

		// parse a definition with incompatible type's information, the case :=
		if (_match(pLexer->GetCurrToken(), TT_ASSIGN_OP))
		{
			pLexer->GetNextToken();

			return _parseDefinition(new CASTDeclarationNode(pIdentifiers, nullptr, attributes), pLexer);
		}

		pTypeInfo = _parseType(pLexer);

		CASTDeclarationNode* pDeclaration = new CASTDeclarationNode(pIdentifiers, pTypeInfo, attributes);

		// parse definition not declaration, full def : type = <value>
		if (_match(pLexer->GetCurrToken(), TT_ASSIGN_OP))
		{
			pLexer->GetNextToken();

			return _parseDefinition(pDeclaration, pLexer);
		}

		return pDeclaration;
	}

	/*!
		\brief Try to parse an identifiers list

		<identifiers> ::= <identifier>
		<identifier> , <identifiers>;

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node, which contains identifiers' names
	*/

	CASTNode* CParser::_parseIdentifiers(ILexer* pLexer, U32 attributes)
	{
		CASTNode* pIdentifiersRoot = new CASTNode(NT_IDENTIFIERS_LIST);

		const CToken* pCurrToken = nullptr;

		do
		{
			pCurrToken = pLexer->GetCurrToken();

			if (_match(pCurrToken, TT_COMMA))
			{
				pCurrToken = pLexer->GetNextToken(); // should be identifier
			}

			if (!SUCCESS(_expect(TT_IDENTIFIER, pCurrToken)))
			{
				return pIdentifiersRoot;
			}

			pIdentifiersRoot->AttachChild(new CASTIdentifierNode(dynamic_cast<const CIdentifierToken*>(pCurrToken)->GetName()));

			// function argument doesn't allow multiple variable per declaration
			/*if ((attributes & AV_FUNC_ARG_DECL) == AV_FUNC_ARG_DECL)
			{
				break;
			}*/
		} 
		while (_match(pLexer->GetNextToken(), TT_COMMA) || _match(pLexer->GetCurrToken(), TT_IDENTIFIER));

		return pIdentifiersRoot;
	}
	
	/*!
		\brief Try to parse a type

		<type> ::=   <builtin_type>
		           | <identifier>
		           | <struct_declaration>
		           | <enum_declaration>
		           | <func_declaration>;

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node with a type
	*/

	CASTNode* CParser::_parseType(ILexer* pLexer)
	{
		// function's declaration
		if (_match(pLexer->GetCurrToken(), TT_OPEN_SQR_BRACE) || 
			_match(pLexer->GetCurrToken(), TT_OPEN_BRACKET))
		{
			return _parseFunctionDeclaration(pLexer);
		}

		return _parseBuiltInType(pLexer);
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

		\param[in] pLexer A pointer to pLexer's object

		\return  A pointer to node with a builtin type
	*/

	CASTNode* CParser::_parseBuiltInType(ILexer* pLexer)
	{
		const CToken* pTypeName  = pLexer->GetCurrToken();
		const CToken* pNextToken = pLexer->PeekNextToken(1);

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
				pLexer->GetNextToken(); // move to ;

				switch (pTypeName->GetType())
				{
					case TT_INT8_TYPE:
						return new CASTTypeNode(NT_INT8);

					case TT_INT16_TYPE:
						return new CASTTypeNode(NT_INT16);

					case TT_INT32_TYPE:
						return new CASTTypeNode(NT_INT32);						

					case TT_INT64_TYPE:
						return new CASTTypeNode(NT_INT64);						

					case TT_UINT8_TYPE:
						return new CASTTypeNode(NT_UINT8);						

					case TT_UINT16_TYPE:
						return new CASTTypeNode(NT_UINT16);						

					case TT_UINT32_TYPE:
						return new CASTTypeNode(NT_UINT32);						

					case TT_UINT64_TYPE:
						return new CASTTypeNode(NT_UINT64);						

					case TT_CHAR_TYPE:
						return new CASTTypeNode(NT_CHAR);						

					case TT_STRING_TYPE:
						return new CASTTypeNode(NT_STRING);						

					case TT_BOOL_TYPE:
						return new CASTTypeNode(NT_BOOL);

					case TT_VOID_TYPE:
						return new CASTTypeNode(NT_VOID);

					case TT_FLOAT_TYPE:
						return new CASTTypeNode(NT_FLOAT);

					case TT_DOUBLE_TYPE:
						return new CASTTypeNode(NT_DOUBLE);
				}

				break;
		}

		return pBuiltinType;
	}

	CASTExpressionNode* CParser::_parseExpression(ILexer* pLexer)
	{
		return _parseHighPrecedenceExpr(pLexer);
	}

	CASTExpressionNode* CParser::_parseHighPrecedenceExpr(ILexer* pLexer)
	{
		CASTExpressionNode* pLeft = _parseLowPrecedenceExpr(pLexer);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_PLUS) || 
			   _match(pLexer->GetCurrToken(), TT_MINUS))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseLowPrecedenceExpr(pLexer);

			pLeft = new CASTBinaryExpressionNode(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTExpressionNode* CParser::_parseLowPrecedenceExpr(ILexer* pLexer)
	{
		CASTExpressionNode* pLeft = _parseUnaryExpression(pLexer);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_STAR) ||
			_match(pLexer->GetCurrToken(), TT_SLASH))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseUnaryExpression(pLexer);

			pLeft = new CASTBinaryExpressionNode(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTExpressionNode* CParser::_parseEqualityExpr(ILexer* pLexer)
	{
		CASTExpressionNode* pLeft = _parseComparisonExpr(pLexer);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_EQ) ||
			_match(pLexer->GetCurrToken(), TT_NE))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseComparisonExpr(pLexer);

			pLeft = new CASTBinaryExpressionNode(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTExpressionNode* CParser::_parseComparisonExpr(ILexer* pLexer)
	{
		CASTExpressionNode* pLeft = _parseLowPrecedenceExpr(pLexer);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_GE) || 
			   _match(pLexer->GetCurrToken(), TT_GT) ||
			   _match(pLexer->GetCurrToken(), TT_LE) || 
			   _match(pLexer->GetCurrToken(), TT_LT))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseLowPrecedenceExpr(pLexer);

			pLeft = new CASTBinaryExpressionNode(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTUnaryExpressionNode* CParser::_parseUnaryExpression(ILexer* pLexer)
	{
		const CToken* pCurrToken = pLexer->GetCurrToken();

		if (_match(pCurrToken, TT_MINUS))
		{
			pLexer->GetNextToken();

			return new CASTUnaryExpressionNode(TT_MINUS, _parsePrimaryExpression(pLexer));
		}

		if (_match(pCurrToken, TT_NOT))
		{
			pLexer->GetNextToken();

			return new CASTUnaryExpressionNode(TT_NOT, _parsePrimaryExpression(pLexer));
		}

		CASTUnaryExpressionNode* pPrimaryNode = new CASTUnaryExpressionNode(TT_DEFAULT, _parsePrimaryExpression(pLexer));

		// function's call
		if (_match(pLexer->GetCurrToken(), TT_OPEN_BRACKET))
		{
			return new CASTUnaryExpressionNode(TT_DEFAULT, _parseFunctionCall(pPrimaryNode, pLexer));
		}

		return pPrimaryNode;
	}

	CASTNode* CParser::_parsePrimaryExpression(ILexer* pLexer)
	{
		const CToken* pCurrToken = pLexer->GetCurrToken();

		CASTNode* pNode = nullptr;

		switch (pCurrToken->GetType())
		{
			case TT_IDENTIFIER:
				pNode = new CASTIdentifierNode(dynamic_cast<const CIdentifierToken*>(pCurrToken)->GetName());
				break;
			case TT_LITERAL:
				pNode = new CASTLiteralNode(dynamic_cast<const CLiteralToken*>(pCurrToken)->GetValue());
				break;
		}

		pLexer->GetNextToken();

		return pNode;
	}

	CASTNode* CParser::_parseAssignment(ILexer* pLexer)
	{
		CASTUnaryExpressionNode* pLeftNode = _parseUnaryExpression(pLexer);

		_expect(TT_ASSIGN_OP, pLexer->GetCurrToken());

		pLexer->GetNextToken();

		CASTExpressionNode* pRightNode = _parseExpression(pLexer);

		return new CASTAssignmentNode(pLeftNode, pRightNode);
	}

	CASTIfStatementNode* CParser::_parseIfStatement(ILexer* pLexer)
	{
		CASTExpressionNode* pCondition = _parseExpression(pLexer);

		if (!SUCCESS(_expect(TT_OPEN_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take {

		CASTBlockNode* pThenBlock = _parseBlockStatements(pLexer);

		if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take }

		CASTBlockNode* pElseBlock = _match(pLexer->GetCurrToken(), TT_ELSE_KEYWORD) ? _parseBlockStatements(pLexer) : nullptr;

		return new CASTIfStatementNode(pCondition, pThenBlock, pElseBlock);
	}
	
	CASTLoopStatementNode* CParser::_parseLoopStatement(ILexer* pLexer)
	{
		if (!SUCCESS(_expect(TT_OPEN_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take {

		CASTBlockNode* pBodyBlock = _parseBlockStatements(pLexer);

		if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take }

		return new CASTLoopStatementNode(pBodyBlock);
	}
	   
	CASTWhileLoopStatementNode* CParser::_parseWhileLoopStatement(ILexer* pLexer)
	{
		CASTExpressionNode* pCondition = _parseExpression(pLexer);

		if (!SUCCESS(_expect(TT_OPEN_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take {

		CASTBlockNode* pBodyBlock = _parseBlockStatements(pLexer);

		if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take }

		return new CASTWhileLoopStatementNode(pCondition, pBodyBlock);
	}

	CASTFunctionDeclNode* CParser::_parseFunctionDeclaration(ILexer* pLexer, bool allowCapture)
	{
		// parse a closure's declaration if it exists
		CASTFunctionClosureNode* pClosureDecl = allowCapture && _match(pLexer->GetCurrToken(), TT_OPEN_SQR_BRACE) ? _parseFunctionClosure(pLexer) : nullptr;

		// parse arguments
		CASTFunctionArgsNode* pArgsList = _match(pLexer->GetCurrToken(), TT_OPEN_BRACKET) ? _parseFunctionArgs(pLexer) : nullptr;

		if (!SUCCESS(_expect(TT_ARROW, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take ->

		// parse return value's type
		CASTNode* pReturnType = _parseType(pLexer);

		return new CASTFunctionDeclNode(pClosureDecl, pArgsList, pReturnType);
	}
	
	CASTFunctionClosureNode* CParser::_parseFunctionClosure(ILexer* pLexer)
	{
		if (!SUCCESS(_expect(TT_OPEN_SQR_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}
		
		pLexer->GetNextToken(); // take [

		CASTFunctionClosureNode* pClosureDecl = new CASTFunctionClosureNode();

		do
		{
			if (_match(pLexer->GetCurrToken(), TT_COMMA))
			{
				pLexer->GetNextToken();
			}

			pClosureDecl->AttachChild(_parseUnaryExpression(pLexer));
		} 
		while (_match(pLexer->GetCurrToken(), TT_COMMA));

		if (!SUCCESS(_expect(TT_CLOSE_SQR_BRACE, pLexer->GetCurrToken())))
		{
			return pClosureDecl;
		}

		pLexer->GetNextToken(); // take ]

		return pClosureDecl;
	}

	CASTFunctionArgsNode* CParser::_parseFunctionArgs(ILexer* pLexer)
	{
		if (!SUCCESS(_expect(TT_OPEN_BRACKET, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take (

		CASTFunctionArgsNode* pArgsNode = new CASTFunctionArgsNode();

		// return empty arguments list
		if (_match(pLexer->GetCurrToken(), TT_CLOSE_BRACKET))
		{
			pLexer->GetNextToken(); // take )

			return pArgsNode;
		}

		do
		{
			if (_match(pLexer->GetCurrToken(), TT_COMMA))
			{
				pLexer->GetNextToken();
			}

			pArgsNode->AttachChild(_parseDeclaration(pLexer)); ///< \todo function's arguments can't have attributes before type's description
		} while (_match(pLexer->GetCurrToken(), TT_COMMA));

		if (!SUCCESS(_expect(TT_CLOSE_BRACKET, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take )

		return pArgsNode;
	}

	CASTFunctionCallNode* CParser::_parseFunctionCall(CASTUnaryExpressionNode* pPrimaryExpr, ILexer* pLexer)
	{
		if (!SUCCESS(_expect(TT_OPEN_BRACKET, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take (

		CASTNode* pArgsNode = new CASTNode(NT_FUNC_ARGS);

		do
		{
			if (_match(pLexer->GetCurrToken(), TT_COMMA))
			{
				pLexer->GetNextToken();
			}

			pArgsNode->AttachChild(_parseExpression(pLexer)); ///< \todo function's arguments can't have attributes before type's description
		} while (_match(pLexer->GetCurrToken(), TT_COMMA));

		if (!SUCCESS(_expect(TT_CLOSE_BRACKET, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take )

		return new CASTFunctionCallNode(pPrimaryExpr, pArgsNode);
	}

	CASTReturnStatementNode* CParser::_parseReturnStatement(ILexer* pLexer)
	{
		return new CASTReturnStatementNode(_parseExpression(pLexer));
	}

	CASTDefinitionNode* CParser::_parseDefinition(CASTDeclarationNode* pDecl, ILexer* pLexer)
	{
		auto pTypeInfo = pDecl->GetTypeInfo();

		// infer the type 
		if (!pTypeInfo)
		{
			// function definition
			if (_match(pLexer->GetCurrToken(), TT_OPEN_SQR_BRACE) || _match(pLexer->GetCurrToken(), TT_OPEN_BRACKET))
			{
				CASTFuncDefinitionNode* pFuncDefNode = _parseFunctionDefinition(pDecl, pLexer);

				pDecl->SetTypeInfo(pFuncDefNode->GetLambdaTypeInfo());

				return pFuncDefNode;
			}
		}
		else if (pTypeInfo->GetType() == NT_FUNC_DECL)
		{
			//parse function's body
			return _parseFunctionDefinition(pDecl, pLexer);
		}

		return new CASTDefinitionNode(pDecl, _parseExpression(pLexer));
	}

	CASTFuncDefinitionNode* CParser::_parseFunctionDefinition(CASTDeclarationNode* pDecl, ILexer* pLexer)
	{
		CASTFunctionDeclNode* pLambdaDefType = _parseFunctionDeclaration(pLexer, true);

		if (!SUCCESS(_expect(TT_OPEN_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take {
		
		CASTBlockNode* pLambdaBody = _parseBlockStatements(pLexer);

		if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take }

		return new CASTFuncDefinitionNode(pDecl, pLambdaDefType, pLambdaBody);
	}

	bool CParser::_match(const CToken* pToken, E_TOKEN_TYPE type)
	{
		return pToken && pToken->GetType() == type;
	}
}
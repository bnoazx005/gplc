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
#include "common/gplcValues.h"
#include "parser/gplcASTNodesFactory.h"


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

	CASTNode* CParser::Parse(ILexer* pLexer, ISymTable* pSymTable, IASTNodesFactory* pNodesFactory, const std::string& moduleName)
	{
		if (!pLexer || !pSymTable || !pNodesFactory)
		{
			OnErrorOutput.Invoke({ PE_INVALID_ENVIRONMENT, 1, 1, { TParserErrorInfo::TUnexpectedTokenInfo { TT_DEFAULT, TT_DEFAULT } } });
			
			return nullptr;
		}
		
		mpSymTable = pSymTable;

		mpNodesFactory = pNodesFactory;

		if (!pLexer->GetCurrToken()) //returns just an empty program unit
		{
			return mpNodesFactory->CreateSourceUnitNode(moduleName);
		}

		return _parseProgramUnit(pLexer, moduleName);
	}

	Result CParser::_expect(E_TOKEN_TYPE expectedValue, const CToken* currValue)
	{
		E_TOKEN_TYPE currValueType = currValue->GetType();

		if (expectedValue == currValueType)
		{
			return RV_SUCCESS;
		}

		OnErrorOutput.Invoke({ PE_UNEXPECTED_TOKEN, currValue->GetPos(), currValue->GetLine(), { TParserErrorInfo::TUnexpectedTokenInfo { expectedValue, currValueType } } });

		return RV_UNEXPECTED_TOKEN;
	}

	/*!
		\brief Try to parse the following grammar rule.

		<program-unit> ::= <statements>;

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node of a program unit
	*/

	CASTNode* CParser::_parseProgramUnit(ILexer* pLexer, const std::string& moduleName)
	{
		CASTNode* pProgramUnit = mpNodesFactory->CreateSourceUnitNode(moduleName);

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
		CASTNode* pStatementsList = mpNodesFactory->CreateNode(NT_STATEMENTS);
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
		
		if (_match(pLexer->GetCurrToken(), TT_ENUM_TYPE))
		{
			pLexer->GetNextToken();

			return _parseEnumDeclaration(pLexer);
		}

		if (_match(pLexer->GetCurrToken(), TT_STRUCT_TYPE))
		{
			pLexer->GetNextToken();

			return _parseStructDeclaration(pLexer);
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
		CASTBlockNode* pBlockNode = mpNodesFactory->CreateBlockNode();

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
					   <function-call> |
					   <access-operator>;

		\param[in] pLexer A pointer to pLexer's object

		\return A pointer to node with an operator
	*/

	CASTNode* CParser::_parseOperator(ILexer* pLexer)
	{
		CASTNode* pOperator = nullptr;

		const CToken* pCurrToken = pLexer->GetCurrToken();

		if (_match(pCurrToken, TT_IDENTIFIER))
		{
			if (_match(pLexer->PeekNextToken(1), TT_COMMA) || _match(pLexer->PeekNextToken(1), TT_COLON))
			{
				pOperator = _parseDeclaration(pLexer);
			}
			else if (_match(pLexer->PeekNextToken(1), TT_ASSIGN_OP))
			{
				pOperator = _parseAssignment(pLexer);
			}
			else if (_match(pLexer->PeekNextToken(1), TT_OPEN_BRACKET))
			{
				CASTUnaryExpressionNode* pUnaryExpr = dynamic_cast<CASTUnaryExpressionNode*>(_parseUnaryExpression(pLexer)); 

				CASTNode* pNestedExprNode = pUnaryExpr ? pUnaryExpr->GetData() : nullptr;

				if (pNestedExprNode != nullptr && 
					(pNestedExprNode->GetType() == NT_FUNC_CALL || 
					 pNestedExprNode->GetType() == NT_ACCESS_OPERATOR)) // return if the node is a function's call or an access operator
				{
					return pNestedExprNode;
				}
			}
		}

		if (_match(pCurrToken, TT_BREAK_KEYWORD) || _match(pCurrToken, TT_CONTINUE_KEYWORD))
		{
			pLexer->GetNextToken(); // take 'break' or 'continue'

			if (pCurrToken->GetType() == TT_BREAK_KEYWORD)
			{
				return mpNodesFactory->CreateBreakNode();
			}
			else
			{
				return mpNodesFactory->CreateContinueNode();
			}
		}

		if (!pOperator)
		{
			return _parseAssignment(pLexer);
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

			return _parseDefinition(mpNodesFactory->CreateDeclNode(pIdentifiers, nullptr, attributes), pLexer);
		}

		pTypeInfo = _parseType(pLexer);

		CASTDeclarationNode* pDeclaration = mpNodesFactory->CreateDeclNode(pIdentifiers, pTypeInfo, attributes);

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
		CASTNode* pIdentifiersRoot = mpNodesFactory->CreateNode(NT_IDENTIFIERS_LIST);

		const CToken* pCurrToken = nullptr;

		std::string currIdentifier;

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

			currIdentifier = dynamic_cast<const CIdentifierToken*>(pCurrToken)->GetName();

			if ((attributes & AV_STRUCT_FIELD_DECL))
			{
				mpSymTable->AddVariable({ currIdentifier, nullptr, nullptr });
			}

			pIdentifiersRoot->AttachChild(mpNodesFactory->CreateIdNode(currIdentifier, attributes));

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
		const CToken* pCurrToken = pLexer->GetCurrToken();
		
		CASTNode* pBaseType = _parseBaseType(pLexer);
		CASTNode* pType     = pBaseType;

		while (pType = _parseComplexType(pLexer, pBaseType))
		{
			pBaseType = pType;
		}

		return pType ? pType : pBaseType;
	}

	CASTNode* CParser::_parseBaseType(ILexer* pLexer)
	{
		const CToken* pCurrToken = pLexer->GetCurrToken(); 
		const CToken* pNextToken = pLexer->PeekNextToken(1);

		// function's declaration
		if (_match(pCurrToken, TT_OPEN_SQR_BRACE) ||
			_match(pCurrToken, TT_OPEN_BRACKET))
		{
			return _parseFunctionDeclaration(pLexer);
		}

		// named type
		if (_match(pCurrToken, TT_IDENTIFIER))
		{
			pLexer->GetNextToken(); // take the identifier

			return mpNodesFactory->CreateNamedTypeNode(mpNodesFactory->CreateIdNode(dynamic_cast<const CIdentifierToken*>(pCurrToken)->GetName()));
		}

		CASTNode* pBuiltinType = _getBasicType(pCurrToken->GetType());

		if (pBuiltinType)
		{
			pLexer->GetNextToken();
		}

		return pBuiltinType;
	}

	CASTNode* CParser::_parseComplexType(ILexer* pLexer, CASTNode* pBaseType)
	{
		const CToken* pCurrToken = pLexer->GetCurrToken();

		// pointer type
		if (_match(pCurrToken, TT_STAR))
		{
			pLexer->GetNextToken(); // take *

			return mpNodesFactory->CreatePointerTypeNode(pBaseType);
		}

		// an array
		if (_match(pCurrToken, TT_OPEN_SQR_BRACE))
		{
			pLexer->GetNextToken(); // take [

			CASTExpressionNode* pSizeExpr = nullptr;

			if (!_match(pLexer->GetCurrToken(), TT_CLOSE_SQR_BRACE))
			{
				pSizeExpr = _parseExpression(pLexer, 0x0);
			}

			if (!SUCCESS(_expect(TT_CLOSE_SQR_BRACE, pLexer->GetCurrToken())))
			{
				return nullptr;
			}

			pLexer->GetNextToken(); // take ]

			return mpNodesFactory->CreateArrayTypeNode(pBaseType, pSizeExpr);
		}

		return nullptr;
	}

	CASTExpressionNode* CParser::_parseExpression(ILexer* pLexer, U32 attributes)
	{
		return _parseEqualityExpr(pLexer, attributes);
	}

	CASTExpressionNode* CParser::_parseHighPrecedenceExpr(ILexer* pLexer, U32 attributes)
	{
		CASTExpressionNode* pLeft = _parseLowPrecedenceExpr(pLexer, attributes);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_PLUS) || 
			   _match(pLexer->GetCurrToken(), TT_MINUS))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseLowPrecedenceExpr(pLexer, attributes);

			pLeft = mpNodesFactory->CreateBinaryExpr(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTExpressionNode* CParser::_parseLowPrecedenceExpr(ILexer* pLexer, U32 attributes)
	{
		CASTExpressionNode* pLeft = _parseUnaryExpression(pLexer, attributes);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_STAR) ||
			_match(pLexer->GetCurrToken(), TT_SLASH))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseUnaryExpression(pLexer, attributes);

			pLeft = mpNodesFactory->CreateBinaryExpr(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTExpressionNode* CParser::_parseEqualityExpr(ILexer* pLexer, U32 attributes)
	{
		CASTExpressionNode* pLeft = _parseComparisonExpr(pLexer, attributes);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_EQ) ||
			_match(pLexer->GetCurrToken(), TT_NE))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseComparisonExpr(pLexer, attributes);

			pLeft = mpNodesFactory->CreateBinaryExpr(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTExpressionNode* CParser::_parseComparisonExpr(ILexer* pLexer, U32 attributes)
	{
		CASTExpressionNode* pLeft = _parseHighPrecedenceExpr(pLexer, attributes);

		CASTExpressionNode* pRight = nullptr;

		E_TOKEN_TYPE opType = TT_VOID_TYPE;

		while (_match(pLexer->GetCurrToken(), TT_GE) || 
			   _match(pLexer->GetCurrToken(), TT_GT) ||
			   _match(pLexer->GetCurrToken(), TT_LE) || 
			   _match(pLexer->GetCurrToken(), TT_LT))
		{
			opType = pLexer->GetCurrToken()->GetType();

			pLexer->GetNextToken();

			pRight = _parseHighPrecedenceExpr(pLexer, attributes);

			pLeft = mpNodesFactory->CreateBinaryExpr(pLeft, opType, pRight);
		}

		return pLeft;
	}

	CASTExpressionNode* CParser::_parseUnaryExpression(ILexer* pLexer, U32 attributes)
	{
		const CToken* pCurrToken = pLexer->GetCurrToken();

		if (_match(pCurrToken, TT_MINUS) || 
			_match(pCurrToken, TT_NOT)   ||
			_match(pCurrToken, TT_AMPERSAND) ||
			_match(pCurrToken, TT_STAR))
		{
			pLexer->GetNextToken();

			if (pCurrToken->GetType() == TT_AMPERSAND)
			{
				attributes &= ~AV_RVALUE; // if it's get address operator then it's already rvalue, so remove the flag
			}

			return mpNodesFactory->CreateUnaryExpr(pCurrToken->GetType(), _parsePrimaryExpression(pLexer, attributes));
		}

		auto pPrimaryExpr = _parsePrimaryExpression(pLexer, attributes);

		if (!pPrimaryExpr)
		{
			return nullptr;
		}

		CASTUnaryExpressionNode* pPrimaryNode = mpNodesFactory->CreateUnaryExpr(TT_DEFAULT, pPrimaryExpr);

		// access to aggregate type's field
		if (_match(pLexer->GetCurrToken(), TT_POINT))
		{
			pLexer->GetNextToken(); // take .

			return _parseAccessOperator(pPrimaryNode, pLexer, attributes);
		}

		// function's call
		if (_match(pLexer->GetCurrToken(), TT_OPEN_BRACKET))
		{
			return mpNodesFactory->CreateUnaryExpr(TT_DEFAULT, _parseFunctionCall(pPrimaryNode, pLexer));
		}

		// indexed access to an aggregate type
		if (_match(pLexer->GetCurrToken(), TT_OPEN_SQR_BRACE))
		{
			return _parseIndexedAccessOperator(pPrimaryNode, pLexer, attributes);
		}

		return pPrimaryNode;
	}

	CASTNode* CParser::_parsePrimaryExpression(ILexer* pLexer, U32 attributes)
	{
		const CToken* pCurrToken = pLexer->GetCurrToken();

		if (!pCurrToken)
		{
			return nullptr;
		}

		CASTNode* pNode = nullptr;

		switch (pCurrToken->GetType())
		{
			case TT_IDENTIFIER:
				pNode = mpNodesFactory->CreateIdNode(dynamic_cast<const CIdentifierToken*>(pCurrToken)->GetName(), attributes);
				break;
			case TT_LITERAL:
				pNode = mpNodesFactory->CreateLiteralNode(dynamic_cast<const CLiteralToken*>(pCurrToken)->GetValue());
				break;
		}

		if (pNode)
		{
			pLexer->GetNextToken();
		}

		return pNode;
	}

	CASTNode* CParser::_parseAssignment(ILexer* pLexer)
	{
		CASTExpressionNode* pLeftNode = _parseUnaryExpression(pLexer);

		if (!pLeftNode)
		{
			return nullptr;
		}

		_expect(TT_ASSIGN_OP, pLexer->GetCurrToken());

		pLexer->GetNextToken();

		CASTExpressionNode* pRightNode = _parseExpression(pLexer, AV_RVALUE);

		return new CASTAssignmentNode(pLeftNode, pRightNode);
	}

	CASTIfStatementNode* CParser::_parseIfStatement(ILexer* pLexer)
	{
		CASTExpressionNode* pCondition = _parseExpression(pLexer, AV_RVALUE);

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

		CASTBlockNode* pElseBlock = nullptr;

		if (_match(pLexer->GetCurrToken(), TT_ELSE_KEYWORD))
		{
			pLexer->GetNextToken(); //take 'else'

			if (!SUCCESS(_expect(TT_OPEN_BRACE, pLexer->GetCurrToken())))
			{
				return nullptr;
			}

			pLexer->GetNextToken(); // take {

			pElseBlock = _parseBlockStatements(pLexer);

			if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
			{
				return nullptr;
			}

			pLexer->GetNextToken(); // take }
		}

		return mpNodesFactory->CreateIfStmtNode(pCondition, pThenBlock, pElseBlock);
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

		return mpNodesFactory->CreateLoopStmtNode(pBodyBlock);
	}
	   
	CASTWhileLoopStatementNode* CParser::_parseWhileLoopStatement(ILexer* pLexer)
	{
		CASTExpressionNode* pCondition = _parseExpression(pLexer, AV_RVALUE);

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

		return mpNodesFactory->CreateWhileStmtNode(pCondition, pBodyBlock);
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

		return mpNodesFactory->CreateFuncDeclNode(pClosureDecl, pArgsList, pReturnType);
	}
	
	CASTFunctionClosureNode* CParser::_parseFunctionClosure(ILexer* pLexer)
	{
		if (!SUCCESS(_expect(TT_OPEN_SQR_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}
		
		pLexer->GetNextToken(); // take [

		CASTFunctionClosureNode* pClosureDecl = mpNodesFactory->CreateFuncClosureNode();

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

		CASTFunctionArgsNode* pArgsNode = mpNodesFactory->CreateFuncArgsNode();

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

			pArgsNode->AttachChild(_parseDeclaration(pLexer, AV_FUNC_ARG_DECL)); ///< \todo function's arguments can't have attributes before type's description
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

		CASTNode* pArgsNode = mpNodesFactory->CreateNode(NT_FUNC_ARGS);

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

		return mpNodesFactory->CreateFuncCallNode(pPrimaryExpr, pArgsNode);
	}

	CASTReturnStatementNode* CParser::_parseReturnStatement(ILexer* pLexer)
	{
		return mpNodesFactory->CreateReturnStmtNode(_parseExpression(pLexer));
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

			CASTExpressionNode* pInitializerExpr = _parseExpression(pLexer);

			// if user didn't specify type explicitly we need to store initializer's value to infer it later in semantic analysis stage
			pDecl->SetTypeInfo(pInitializerExpr);

			return mpNodesFactory->CreateDefNode(pDecl, pInitializerExpr);
		}
		else if (pTypeInfo->GetType() == NT_FUNC_DECL)
		{
			//parse function's body
			return _parseFunctionDefinition(pDecl, pLexer);
		}

		return mpNodesFactory->CreateDefNode(pDecl, _parseExpression(pLexer));
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

		return mpNodesFactory->CreateFuncDefNode(pDecl, pLambdaDefType, pLambdaBody);
	}
	
	CASTEnumDeclNode* CParser::_parseEnumDeclaration(ILexer* pLexer)
	{
		if (!SUCCESS(_expect(TT_IDENTIFIER, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		CASTIdentifierNode* pEnumIdentifier = mpNodesFactory->CreateIdNode((dynamic_cast<const CIdentifierToken*>(pLexer->GetCurrToken()))->GetName());

		pLexer->GetNextToken(); // take enum's name

		if (!SUCCESS(_expect(TT_OPEN_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take {

		// parse enum's values
		if (!_parseEnumValues(pEnumIdentifier->GetName(), pLexer))
		{
			return nullptr;
		}

		if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take }

		CASTEnumDeclNode* pEnumDeclNode = mpNodesFactory->CreateEnumDeclNode(pEnumIdentifier);

		return pEnumDeclNode;
	}

	bool CParser::_parseEnumValues(const std::string& enumName, ILexer* pLexer)
	{
		if (!SUCCESS(mpSymTable->CreateNamedScope(enumName)))
		{
			return false;
		}

		// add information about enum's type
		auto pEnumDesc = mpSymTable->LookUpNamedScope(enumName);

		pEnumDesc->mpType = new CEnumType(mpSymTable, enumName);

		const CToken* pCurrEnumField = nullptr;

		TSymbolHandle currFieldHandle = InvalidSymbolHandle;

		std::string currEnumeratorName;

		I32 currEnumeratorDefaultValue = 0x0;

		CASTExpressionNode* pPrevEnumeratorValue = nullptr;

		while (_match(pCurrEnumField = pLexer->GetCurrToken(), TT_IDENTIFIER))
		{
			currEnumeratorName = dynamic_cast<const CIdentifierToken*>(pCurrEnumField)->GetName();

			if ((currFieldHandle = mpSymTable->AddVariable({ currEnumeratorName, nullptr, nullptr })) == InvalidSymbolHandle)
			{
				auto pCurrToken = pLexer->GetCurrToken();

				OnErrorOutput.Invoke({ PE_INVALID_ENUMERATOR_NAME, pCurrToken->GetPos(), pCurrToken->GetLine(), {} });

				return false;
			}

			// assign values into enumerator's description
			TSymbolDesc* pCurrEnumerator = mpSymTable->LookUp(currFieldHandle);

			pCurrEnumerator->mpType = new CType(CT_INT32, BTS_INT32, 0x0);

			pLexer->GetNextToken(); // take identifier

			if (_match(pLexer->GetCurrToken(), TT_ASSIGN_OP)) // field has an initializer
			{
				pLexer->GetNextToken(); // take =

				// \note by default, the enumerator can be initialized with some basic literal's value, but all enumerators should have the same type of literals
				pPrevEnumeratorValue = _parseExpression(pLexer); // \note type is resolved in semantic analyser's stage

				pCurrEnumerator->mpValue = pPrevEnumeratorValue;
			}
			
			// \note assign default value if it wasn't assigned by a user
			if (!pCurrEnumerator->mpValue)
			{
				if (pPrevEnumeratorValue)
				{
					pPrevEnumeratorValue = mpNodesFactory->CreateBinaryExpr(mpNodesFactory->CreateUnaryExpr(TT_DEFAULT, mpNodesFactory->CreateLiteralNode(new CIntValue(1))), 
																			TT_PLUS, pPrevEnumeratorValue);
				}
				else
				{
					pPrevEnumeratorValue = mpNodesFactory->CreateUnaryExpr(TT_DEFAULT, mpNodesFactory->CreateLiteralNode(new CIntValue(0)));
				}

				pCurrEnumerator->mpValue = pPrevEnumeratorValue;
			}

			if (_match(pLexer->GetCurrToken(), TT_CLOSE_BRACE))
			{
				break;
			}

			if (!SUCCESS(_expect(TT_COMMA, pLexer->GetCurrToken())))
			{
				return false;
			}

			pLexer->GetNextToken(); // take ,
		}

		mpSymTable->LeaveScope();
		
		return true;
	}

	CASTStructDeclNode* CParser::_parseStructDeclaration(ILexer* pLexer)
	{
		if (!SUCCESS(_expect(TT_IDENTIFIER, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		CASTIdentifierNode* pStructIdentifier = mpNodesFactory->CreateIdNode((dynamic_cast<const CIdentifierToken*>(pLexer->GetCurrToken()))->GetName());

		pLexer->GetNextToken(); // take struct's name

		if (!SUCCESS(_expect(TT_OPEN_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take {

		// parse struct's fields
		CASTBlockNode* pStructFields = nullptr;

		if (!(pStructFields = _parseStructFields(pStructIdentifier->GetName(), pLexer)))
		{
			return nullptr;
		}

		if (!SUCCESS(_expect(TT_CLOSE_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take }

		return mpNodesFactory->CreateStructDeclNode(pStructIdentifier, pStructFields);
	}

	CASTBlockNode* CParser::_parseStructFields(const std::string& structName, ILexer* pLexer)
	{
		if (!SUCCESS(mpSymTable->CreateNamedScope(structName)))
		{
			return nullptr;
		}

		CASTBlockNode* pStructFields = mpNodesFactory->CreateBlockNode();

		CASTNode* pCurrField = nullptr;

		while (!_match(pLexer->GetCurrToken(), TT_CLOSE_BRACE) && (pCurrField = _parseDeclaration(pLexer, AV_STRUCT_FIELD_DECL)))
		{
			pStructFields->AttachChild(pCurrField);

			if (!SUCCESS(_expect(TT_SEMICOLON, pLexer->GetCurrToken())))
			{
				return pStructFields;
			}

			pLexer->GetNextToken(); // take ;
		}

		mpSymTable->LeaveScope();

		return pStructFields;
	}
	
	bool CParser::_match(const CToken* pToken, E_TOKEN_TYPE type)
	{
		return pToken && pToken->GetType() == type;
	}

	CASTTypeNode* CParser::_getBasicType(E_TOKEN_TYPE typeToken) const
	{
		switch (typeToken)
		{
			case TT_INT8_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_INT8);

			case TT_INT16_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_INT16);

			case TT_INT32_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_INT32);

			case TT_INT64_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_INT64);

			case TT_UINT8_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_UINT8);

			case TT_UINT16_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_UINT16);

			case TT_UINT32_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_UINT32);

			case TT_UINT64_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_UINT64);

			case TT_CHAR_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_CHAR);

			case TT_STRING_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_STRING);

			case TT_BOOL_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_BOOL);

			case TT_VOID_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_VOID);

			case TT_FLOAT_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_FLOAT);

			case TT_DOUBLE_TYPE:
				return mpNodesFactory->CreateTypeNode(NT_DOUBLE);
		}

		assert(false); // unreachable code

		return nullptr;
	}

	CASTAccessOperatorNode* CParser::_parseAccessOperator(CASTExpressionNode* pPrimaryExpr, ILexer* pLexer, U32 attributes)
	{
		const CToken* pCurrToken = pLexer->GetCurrToken();

		if (!SUCCESS(_expect(TT_IDENTIFIER, pCurrToken)))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take an identifier

		return mpNodesFactory->CreateAccessOperatorNode(pPrimaryExpr,	
														mpNodesFactory->CreateUnaryExpr(TT_DEFAULT, 
																						mpNodesFactory->CreateIdNode(dynamic_cast<const CIdentifierToken*>(pCurrToken)->GetName(), 
																						attributes)));
	}

	CASTIndexedAccessOperatorNode* CParser::_parseIndexedAccessOperator(CASTExpressionNode* pPrimaryExpr, ILexer*pLexer, U32 attributes)
	{
		pLexer->GetNextToken(); // take '['

		auto pIndexedExpression = _parseExpression(pLexer, AV_RVALUE);

		if (!SUCCESS(_expect(TT_CLOSE_SQR_BRACE, pLexer->GetCurrToken())))
		{
			return nullptr;
		}

		pLexer->GetNextToken(); // take ']'

		return mpNodesFactory->CreateIndexedAccessOperatorNode(pPrimaryExpr, pIndexedExpression, attributes);
	}
}
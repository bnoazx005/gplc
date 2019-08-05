/*!
	\author Ildar Kasimov
	\date   17.07.2016
	\copyright

	\brief The file contains parser's class declaration

	\todo
*/

#ifndef GPLC_PARSER_H
#define GPLC_PARSER_H


#include "common\gplcTypes.h"
#include "lexer\gplcTokens.h"
#include "..\utils\Delegate.h"


namespace gplc
{
	class CASTNode;
	class ILexer;
	class CType;
	class ISymTable;
	class CASTExpressionNode;
	class CASTUnaryExpressionNode;
	class CASTIfStatementNode;
	class CASTBlockNode;
	class CASTLoopStatementNode;
	class CASTWhileLoopStatementNode;
	class CASTFunctionDeclNode;
	class CASTFunctionClosureNode;
	class CASTFunctionArgsNode;
	class CASTFunctionCallNode;
	class CASTReturnStatementNode;
	class CASTDefinitionNode;
	class CASTFuncDefinitionNode;
	class CASTDeclarationNode;
	class CASTEnumDeclNode;
	class CASTStructDeclNode;
	class CASTTypeNode;
	class CASTBreakOperatorNode;
	class CASTContinueOperatorNode;
	class CASTAccessOperatorNode;
	class CASTArrayTypeNode;
	class CASTIndexedAccessOperatorNode;


	/*!
		\brief IParser interface

		All parsers should implement its methods, all of 'stub' and 'mock' objects too.
	*/
	
	class IParser
	{
		public:
			IParser() {}
			virtual ~IParser() {}

			virtual CASTNode* Parse(ILexer* pLexer, ISymTable* pSymTable) = 0;
		public:
			CDelegate<void, const TParserErrorInfo&> OnErrorOutput;
		protected:
			IParser(const IParser& parser) {}
	};


	/*!
		\brief CParser class

		Implements methods, which is tools for a syntactic analysis.
	*/
	
	class CParser : public IParser
	{
		public:
			CParser();
			virtual ~CParser();

			virtual CASTNode* Parse(ILexer* pLexer, ISymTable* pSymTable);
		private:
			CParser(const CParser& parser);

			Result _expect(E_TOKEN_TYPE expectedValue, const CToken* currValue);

			/*!
				\brief Try to parse the following grammar rule.

				<program-unit> ::= <statements>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node of a program unit
			*/
			
			CASTNode* _parseProgramUnit(ILexer* pLexer);

			/*!
				\brief Try to parse the list of statements.

				<statements> ::=   <statement> 
                                 | <statement> <statements>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node with a statements list
			*/

			CASTNode* _parseStatementsList(ILexer* pLexer);

			/*!
				\brief Try to parse a single statement

				<statement> ::=   <operator> 
				                | <block>
								| <if-statement>
								| <loop-statement>
								| <while-loop-stmt>;
				
				\todo There is no <directive> non-terminal. It will be added later.

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node with a particular statement
			*/

			CASTNode* _parseStatement(ILexer* pLexer);

			/*!
				\brief Try to parse a single operator

				<operator> ::= <declaration> 
				               | <definition>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node with an operator
			*/

			CASTNode* _parseOperator(ILexer* pLexer);

			/*!
				\brief The method tries to parse the following construct

				<block> ::= statement> <statements>
			*/

			CASTBlockNode* _parseBlockStatements(ILexer* pLexer);

			/*!
				\brief Try to parse a declaration

				<declaration> ::=   <identifiers> : <attributes> <type>
                                  | <identifiers> : <type>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node with a declaration
			*/

			CASTNode* _parseDeclaration(ILexer* pLexer, U32 attributes = 0x0);

			/*!
				\brief Try to parse an identifiers list

				<identifiers> ::= <identifier> 
								  <identifier> , <identifiers>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node, which contains identifiers' names
			*/

			CASTNode* _parseIdentifiers(ILexer* pLexer, U32 attributes);

			/*!
				\brief Try to parse a type

				<type> ::=   <builtin_type>
				           | <identifier>
				           | <struct_declaration>
				           | <enum_declaration>
				           | <func_declaration>;

				\param[in] pLexer A pointer to pLexer's object

				\return  A pointer to node with a type
			*/

			CASTNode* _parseType(ILexer* pLexer);

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

			CASTNode* _parseBuiltInType(ILexer* pLexer);
			
			/*!
				\brief The method tries to parse an expression

				<expression> ::= <equality>
			*/

			CASTExpressionNode* _parseExpression(ILexer* pLexer, U32 attributes = AV_RVALUE);

			/*!
				\brief The method tries to parse high precedence expression
				including mul, div

				<expr1> ::= <expression> '*' <expression> |
							<expression> '/' <expression> 
			*/

			CASTExpressionNode* _parseHighPrecedenceExpr(ILexer* pLexer, U32 attributes = 0x0);

			/*!
				\brief The method tries to parse low precedence expression
				including mul, div

				<expr1> ::= <expression> '+' <expression> |
							<expression> '-' <expression>
			*/

			CASTExpressionNode* _parseLowPrecedenceExpr(ILexer* pLexer, U32 attributes = 0x0);

			/*!
				\brief The method tries to parse low precedence expression
				including equal or not equal

				<equality> ::= <expression> '==' <expression> |
							   <expression> '!=' <expression>
			*/

			CASTExpressionNode* _parseEqualityExpr(ILexer* pLexer, U32 attributes = 0x0);
			
			/*!
				\brief The method tries to parse low precedence expression
				including equal or not equal

				<comparison> ::= <expression> '>' <expression> |
							   <expression> '<' <expression> |
							   <expression> '<=' <expression> |
							   <expression> '>=' <expression> 
			*/

			CASTExpressionNode* _parseComparisonExpr(ILexer* pLexer, U32 attributes = 0x0);

			/*!
				\brief The method tries to parse an unary expression

				<unary_expr> ::= <identifier> | <value> | <function-call> 
			*/

			CASTExpressionNode* _parseUnaryExpression(ILexer* pLexer, U32 attributes = 0x0);

			/*!
				\brief The method tries to parse primary expression

				<value> ::= 
			*/

			CASTNode* _parsePrimaryExpression(ILexer* pLexer, U32 attributes = 0x0);

			/*!
				\brief The method tries to parse an assigment operator

				<assignment> ::= <unary_expr> '=' <expression>
			*/

			CASTNode* _parseAssignment(ILexer* pLexer);

			/*!
				\brief The method tries to parse the following rule

				<if-statement> ::= if <expression> <block> else <block>
								 | if <expression> <block>
			*/

			CASTIfStatementNode* _parseIfStatement(ILexer* pLexer);

			/*!
				\brief The method tries to parse the following rule

				<loop-statement> ::= 'loop' <block>
			*/

			CASTLoopStatementNode* _parseLoopStatement(ILexer* pLexer);

			/*!
				\brief The method parses the following rule

				<while-loop-stmt> ::= 'while' <expression> '{' <block> '}'
			*/

			CASTWhileLoopStatementNode* _parseWhileLoopStatement(ILexer* pLexer);

			/*!
				\brief The method parses the following rule

				<function-decl> ::=   <function-closure> '(' <func-args-declaration> ')' -> <type>
									| (<expr> [,<expr>]*) -> <type>;
			*/

			CASTFunctionDeclNode* _parseFunctionDeclaration(ILexer* pLexer, bool allowCapture = false);
						
			/*!
				\brief The method parses the following rule
			
				<function-closure> ::=   '['<expr> [,<expr>]*']'
			*/

			CASTFunctionClosureNode* _parseFunctionClosure(ILexer* pLexer);

			/*!
				\brief The method parses the following rule
			
				<func-arg> ::= <identifier> : <type>
				
				<function-args-declaration> ::=   <func-arg> [,<func-arg>]*
			*/

			CASTFunctionArgsNode* _parseFunctionArgs(ILexer* pLexer);

			/*!
				\brief The method parses the following rule

				<function-call> ::= <primary> '(' <expression> [,<expression>]* ')'
			*/

			CASTFunctionCallNode* _parseFunctionCall(CASTUnaryExpressionNode* pPrimaryExpr, ILexer* pLexer);

			/*!
				\brief The method parses the following rule

				<return-stmt> ::= 'return' <expression>
			*/

			CASTReturnStatementNode* _parseReturnStatement(ILexer* pLexer);

			/*!
				\brief The method parses the following rule

				<definition> ::= <function-def>
			*/

			CASTDefinitionNode* _parseDefinition(CASTDeclarationNode* pDecl, ILexer* pLexer);

			/*!
				\brief The method parses the following rule

				<function-def> ::= <>
			*/

			CASTFuncDefinitionNode* _parseFunctionDefinition(CASTDeclarationNode* pDecl, ILexer* pLexer);

			CASTEnumDeclNode* _parseEnumDeclaration(ILexer* pLexer);

			bool _parseEnumValues(const std::string& enumName, ILexer* pLexer);

			CASTStructDeclNode* _parseStructDeclaration(ILexer* pLexer);

			CASTBlockNode* _parseStructFields(const std::string& structName, ILexer* pLexer);

			/*!
				\brief 

				<property-access> ::= <primary> '.' <identifier>
			*/

			CASTAccessOperatorNode* _parseAccessOperator(CASTExpressionNode* pPrimaryExpr, ILexer*pLexer, U32 attributes = 0x0);

			/*!
				\brief

				<indexed-access> ::= <expr> '[' <expr> ']'
			*/

			CASTIndexedAccessOperatorNode* _parseIndexedAccessOperator(CASTExpressionNode* pPrimaryExpr, ILexer*pLexer, U32 attributes = 0x0);

			bool _match(const CToken* pToken, E_TOKEN_TYPE type);

			CASTTypeNode* _getBasicType(E_TOKEN_TYPE typeToken) const;
		private:
			ISymTable* mpSymTable;
	};
}

#endif
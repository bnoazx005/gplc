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
	class CASTExpressionNode;
	class CASTUnaryExpressionNode;
	class ISymTable;


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

				<statement> ::= <operator> ;
				
				\todo There is no <directive> non-terminal. It will be added later.

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node with a particular statement
			*/

			CASTNode* _parseStatement(ILexer* pLexer);

			/*!
				\brief Try to parse a single operator

				<operator> ::= <declaration>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node with an operator
			*/

			CASTNode* _parseOperator(ILexer* pLexer);

			/*!
				\brief The method tries to parse the following construct

				<block> ::= statement> <statements>
			*/

			CASTNode* _parseBlockStatements(ILexer* pLexer);

			/*!
				\brief Try to parse a declaration

				<declaration> ::=   <identifiers> : <attributes> <type>
                                  | <identifiers> : <type>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node with a declaration
			*/

			CASTNode* _parseDeclaration(ILexer* pLexer);

			/*!
				\brief Try to parse an identifiers list

				<identifiers> ::= <identifier> 
								  <identifier> , <identifiers>;

				\param[in] pLexer A pointer to pLexer's object

				\return A pointer to node, which contains identifiers' names
			*/

			CASTNode* _parseIdentifiers(ILexer* pLexer);

			CASTNode* _parseStructDecl(ILexer* pLexer);

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

				<expression> ::= <expr_value> | <unary_expr> | <binary_expr> | <group_expr>
			*/

			CASTExpressionNode* _parseExpression(ILexer* pLexer);

			/*!
				\brief The method tries to parse high precedence expression
				including mul, div

				<expr1> ::= <expression> '*' <expression> |
							<expression> '/' <expression> 
			*/

			CASTExpressionNode* _parseHighPrecedenceExpr(ILexer* pLexer);

			/*!
				\brief The method tries to parse low precedence expression
				including mul, div

				<expr1> ::= <expression> '+' <expression> |
							<expression> '-' <expression>
			*/

			CASTExpressionNode* _parseLowPrecedenceExpr(ILexer* pLexer);

			/*!
				\brief The method tries to parse an unary expression

				<unary_expr> ::= <identifier> | <value> 
			*/

			CASTUnaryExpressionNode* _parseUnaryExpression(ILexer* pLexer);

			/*!
				\brief The method tries to parse primary expression

				<value> ::= 
			*/

			CASTNode* _parsePrimaryExpression(ILexer* pLexer);

			/*!
				\brief The method tries to parse an assigment operator

				<assignment> ::= <unary_expr> '=' <expression>
			*/

			CASTNode* _parseAssignment(ILexer* pLexer);

			bool _match(const CToken* pToken, E_TOKEN_TYPE type);
		private:
			ISymTable* mpSymTable;
	};
}

#endif
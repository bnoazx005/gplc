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


namespace gplc
{
	class CASTNode;
	class ILexer;


	/*!
		\brief IParser interface

		All parsers should implement its methods, all of 'stub' and 'mock' objects too.
	*/
	
	class IParser
	{
		public:
			IParser() {}
			virtual ~IParser() {}

			virtual CASTNode* Parse(ILexer* lexer, TParserErrorInfo* &errorInfo) = 0;
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

			virtual CASTNode* Parse(ILexer* lexer, TParserErrorInfo* &errorInfo);
		private:
			CParser(const CParser& parser);

			Result _expect(E_TOKEN_TYPE expectedValue, const CToken* currValue, TParserErrorInfo* &errorInfo);

			/*!
				\brief Try to parse the following grammar rule.

				<program-unit> ::= <statements>;

				\param[in] lexer A pointer to lexer's object
				\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

				\return A pointer to node of a program unit
			*/
			
			CASTNode* _parseProgramUnit(ILexer* lexer, TParserErrorInfo* &errorInfo);

			/*!
				\brief Try to parse the list of statements.

				<statements> ::=   <statement> 
                                 | <statement> <statements>;

				\param[in] lexer A pointer to lexer's object
				\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

				\return A pointer to node with a statements list
			*/

			CASTNode* _parseStatementsList(ILexer* lexer, TParserErrorInfo* &errorInfo);

			/*!
				\brief Try to parse a single statement

				<statement> ::= <operator> ; ; 
				
				\todo There is no <directive> non-terminal. It will be added later.

				\param[in] lexer A pointer to lexer's object
				\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

				\return A pointer to node with a particular statement
			*/

			CASTNode* _parseStatement(ILexer* lexer, TParserErrorInfo* &errorInfo);

			/*!
				\brief Try to parse a single operator

				<operator> ::= <declaration>;

				\param[in] lexer A pointer to lexer's object
				\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

				\return A pointer to node with an operator
			*/

			CASTNode* _parseOperator(ILexer* lexer, TParserErrorInfo* &errorInfo);

			/*!
				\brief Try to parse a declaration

				<declaration> ::=   <identifiers> : <attributes> <type>
                                  | <identifiers> : <type>;

				\param[in] lexer A pointer to lexer's object
				\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

				\return A pointer to node with a declaration
			*/

			CASTNode* _parseDeclaration(ILexer* lexer, TParserErrorInfo* &errorInfo);

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

			CASTNode* _parseType(ILexer* lexer, TParserErrorInfo* &errorInfo);
		private:
	};
}

#endif
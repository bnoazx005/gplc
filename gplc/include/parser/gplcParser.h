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

			virtual CASTNode* Parse(const ILexer* lexer, TParserErrorInfo* errorInfo) = 0;
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

			virtual CASTNode* Parse(const ILexer* lexer, TParserErrorInfo* errorInfo);
		private:
			CParser(const CParser& parser);

			/*!
				\brief Try to parse the following grammar rule.

				<program-unit> ::= <statements>;

				\param[in] lexer A pointer to lexer's object
				\param[out] errorInfo A pointer to structure that contains information about appeared errors. It equals to nullptr if function returns RV_SUCCESS.

				\return A pointer to node of a program unit
			*/
			
			CASTNode* _parseProgramUnit(const ILexer* lexer, TParserErrorInfo* errorInfo);

			/*!
				\brief Try to parse the list of statements.

				<statements> ::=   <statement> 
                                 | <statement> <statements>;

				\param[in]
				\param[out]

				\return
			*/

			CASTNode* _parseStatementsList(const ILexer* lexer, TParserErrorInfo* errorInfo);

			/*!
				\brief Try to parse a single statement

				<statement> ::= <operator> ; ; 
				
				\param[in]
				\param[out]

				\return
			*/

			CASTNode* _parseStatement(const ILexer* lexer, TParserErrorInfo* errorInfo);

			/*!
				\brief Try to parse a single operator

				<operator> ::= <declaration>;

				\param[in]
				\param[out]

				\return
			*/

			CASTNode* _parseOperator(const ILexer* lexer, TParserErrorInfo* errorInfo);

			/*!
				\brief Try to parse a declaration

				<declaration> ::=   <attributes> <identifiers> : <type>
                                  | <identifiers> : <type>;

				\param[in]
				\param[out]

				\return
			*/

			CASTNode* _parseDeclaration(const ILexer* lexer, TParserErrorInfo* errorInfo);
		private:
	};
}

#endif
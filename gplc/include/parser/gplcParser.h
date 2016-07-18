/*!
	\author Ildar Kasimov
	\date   17.07.2016
	\copyright

	\brief The file contains parser's class declaration

	\todo
*/

#ifndef GPLC_PARSER_H
#define GPLC_PARSER_H


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

			virtual CASTNode* Parse(const ILexer* lexer) = 0;
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

			virtual CASTNode* Parse(const ILexer* lexer);
		private:
			CParser(const CParser& parser);
		private:
	};
}

#endif
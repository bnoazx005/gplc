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


	class CParser
	{
		public:
			CParser();
			~CParser();

			CASTNode* Parse(const ILexer* lexer);
		private:
			CParser(const CParser& parser);
		private:
	};
}

#endif
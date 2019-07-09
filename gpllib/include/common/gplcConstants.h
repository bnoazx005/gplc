/*!
	\author Ildar Kasimov
	\date   15.07.2016
	\copyright

	\brief The file contains constants

	\todo
*/

#ifndef GPLC_CONSTANTS_H
#define GPLC_CONSTANTS_H


namespace gplc
{
	/*!
		A lexer's constants
	*/


	/*!
		\brief Enumeration of bit masks, which describes number's parameters during a lexical analysis
	*/

	enum E_NUMBER_DESC_BITS
	{
		NB_INT      = 0x0,  ///< An integer number 
		NB_FLOAT    = 0x1,  ///< A floating point number
		NB_SIGNED   = 0x80, ///< A sign of a number (1 is signed and 0 is unsigned)
		NB_HEX      = 0x10, ///< A number is represented in hexademical numerical system
		NB_OCT      = 0x8,  ///< A number is represented in octal numerical system
		NB_BIN      = 0x4,  ///< A number is represented in binary numerical system
		NB_LONG     = 0x40, ///< A first bit for long type of number; For integer it means long int, for floating point it's equivalent of double
		NB_ADD_LONG = 0x20, ///< An additional bit, which sets up double long type (works only with integers). For "floats" it means nothing.
	};
}

#endif
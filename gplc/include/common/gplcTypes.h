/*!
\author Ildar Kasimov
\date   09.07.2016
\copyright

\brief The file contains types' aliases

\todo
*/

#ifndef GPLC_TYPES_H
#define GPLC_TYPES_H


namespace gplc
{
	///< Integers' aliases
	typedef char      I8;
	typedef short     I16;
	typedef int       I32;
	typedef long long I64;

	typedef unsigned char      U8;
	typedef unsigned short     U16;
	typedef unsigned int       U32;
	typedef unsigned long long U64;

	///< Floating points aliases
	typedef float       F32;
	typedef double      F64;
	typedef long double F128;

	///< Characters' aliases
	typedef char     C8;
	typedef char16_t C16;
	typedef char32_t C32;
	typedef wchar_t  W16;

	///< Result type and its values

	typedef unsigned long Result;	///<The type is used as function's result


	/*!
		\brief An enumeration which contains all allowed values of Result type
	*/

	enum E_RESULT_VALUE
	{
		RV_SUCCESS           = 0x0,	///< Successfully done
		RV_FAIL              = 0x1,	///< Critical error during execution
		RV_INVALID_ARGUMENTS = 0x2, ///< Some function's argument has invalid value
		RV_FILE_NOT_FOUND    = 0x4, ///< file was not found or some another its problem has happened
		RV_INCORRECT_CONFIG  = 0x8, ///< it means that some error has happened during the parsing of a config file
	};

	/*!
		\brief Macro defenitions for analyzing of result's value

		\param[in] resultValue Result's value

		\return A logical expression
	*/

	#define SUCCESS(resultType) (resultType == RV_SUCCESS)	/*(~(resultType & RV_FAIL))*/


	#pragma pack(push, 1)

	typedef struct TLexerErrorInfo
	{
		I32 mLine;
		I32 mPos;
	} TLexerErrorInfo;

	#pragma pack(pop)
}

#endif
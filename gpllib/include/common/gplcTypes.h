/*!
	\author Ildar Kasimov
	\date   09.07.2016
	\copyright

	\brief The file contains types' aliases

	\todo
*/

#ifndef GPLC_TYPES_H
#define GPLC_TYPES_H


#include <string>
#include <variant>


///< forward declarations for LLVM types
namespace llvm
{
	class Module;
	class Value;
	class Function;
	class Instruction;
	class Type;
}


namespace gplc
{
	///< Integers' aliases
	typedef char      I8;
	typedef short     I16;
	typedef int       I32;
	typedef long      IL32;
	typedef long long I64;

	typedef unsigned char      U8;
	typedef unsigned short     U16;
	typedef unsigned int       U32;
	typedef unsigned long      UL32;
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
		RV_SUCCESS                 = 0x0,  ///< Successfully done
		RV_FAIL                    = 0x1,  ///< Critical error during execution
		RV_INVALID_ARGUMENTS       = 0x2,  ///< Some function's argument has invalid value
		RV_FILE_NOT_FOUND          = 0x4,  ///< File was not found or some another its problem has happened
		RV_INCORRECT_CONFIG        = 0x8,  ///< It means that some error has happened during the parsing of a config file
		RV_INCORRECT_TOKEN         = 0x10, ///< This is lexer's error, which happens, when token's structure is incorrect or cannot be recognized.
		RV_UNEXPECTED_TOKEN        = 0x20, ///< This is parser's error, which happens, when it recognizes unexpected token.
		RV_UNRECOGNIZED_TOKENS_SEQ = 0x40, ///< SAn input tokens sequence doesn't match with any grammar rule
		RV_ALREADY_DEFINED_VAR     = 0x80, ///< Some variable has been already declared 
	};

	/*!
		\brief Macro defenitions for analyzing of result's value

		\param[in] resultValue Result's value

		\return A logical expression
	*/

	#define SUCCESS(resultType) (resultType == RV_SUCCESS)	/*(~(resultType & RV_FAIL))*/


	/*!
		\brief The TLexerErrorInfo structure

		Provides information about an appeared error.
	*/
	
	#pragma pack(push, 1)

	typedef struct TLexerErrorInfo
	{
		I32 mLine;
		I32 mPos;
	} TLexerErrorInfo;

	#pragma pack(pop)

	
	/*!
		\brief The TParserErrorInfo structure

		Provides information about an appeared error.
	*/

	#pragma pack(push, 1)

	typedef struct TParserErrorInfo
	{
		Result            mErrorCode;
		std::string       mMessage;
		U32               mPos;

		TParserErrorInfo* mpPrevError;
		TParserErrorInfo* mpNextError;
	} TParserErrorInfo;

	#pragma pack(pop)

	/*!
		\brief The E_COMPILER_TYPES enumration
	*/

	enum E_COMPILER_TYPES
	{
		CT_INT8,
		CT_INT16,
		CT_INT32,
		CT_INT64,
		CT_UINT8,
		CT_UINT16,
		CT_UINT32,
		CT_UINT64,
		CT_FLOAT,
		CT_DOUBLE,
		CT_STRING,
		CT_CHAR,
		CT_VOID,
		CT_BOOL,
		CT_POINTER,
		CT_STRUCT,
		CT_FUNCTION,
		CT_ENUM,
		CT_ARRAY,
		CT_LAST
	};


	enum E_SEMANTIC_ANALYSER_ERRORS: U16
	{
		SAE_IDENTIFIER_ALREADY_DECLARED,
		SAE_UNDECLARED_IDENTIFIER,
		SAE_REDUNDANT_LOOP_STATEMENT,
		SAE_LOGIC_EXPR_IS_EXPECTED,
		SAE_INCOMPATIBLE_TYPE_OF_ASSIGNED_LAMBDA,
		SAE_INCOMPATIBLE_TYPES_INSIDE_EXPR,
	};


	typedef std::variant<llvm::Module*, llvm::Value*, llvm::Function*, llvm::Instruction*, llvm::Type*, std::string> TLLVMIRData;
}

#endif
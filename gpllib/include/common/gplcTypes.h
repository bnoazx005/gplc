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
		\brief The TParserErrorInfo structure

		Provides information about an appeared error.
	*/

	#pragma pack(push, 1)

	typedef struct TParserErrorInfo
	{
		Result      mErrorCode;

		std::string mMessage;

		U32         mPos;

		U32         mLine;
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
		CT_ALIAS,
		CT_LAST
	};


	enum class E_MESSAGE_TYPE : U8
	{
		MT_WARNING,
		MT_ERROR,
		MT_INFO,
	};


	enum E_PARSER_ERRORS : U16
	{
		PE_INVALID_ENUMERATOR_NAME,				/// Some enumerator with the enumeration have a name that is duplicate for the previous one
		PE_INVALID_ENUMERATOR_VALUE,			/// Enumerators can be initialized only with basic literals: numbers, strings, chars, logic, etc
	};


	enum E_SEMANTIC_ANALYSER_MESSAGE: U16
	{
		SAE_IDENTIFIER_ALREADY_DECLARED,
		SAE_UNDECLARED_IDENTIFIER,
		SAE_REDUNDANT_LOOP_STATEMENT,
		SAE_LOGIC_EXPR_IS_EXPECTED,
		SAE_INCOMPATIBLE_TYPE_OF_ASSIGNED_LAMBDA,
		SAE_INCOMPATIBLE_TYPES_INSIDE_EXPR,
		SAE_SINGLE_FUNC_IDENTIFIER_IS_EXPECTED,
		SAE_FUNC_MULTIPLE_PARAM_PER_DECL_ARE_NOT_ALLOWED,
		SAE_UNDEFINED_TYPE,
		SAE_INTERRUPT_STATEMENT_OUTSIDE_LOOP_IS_NOT_ALLOWED,
		SAE_BLOCKING_LOOP,
		SAE_TRY_TO_ACCESS_UNDEFINED_FIELD,
	};


	typedef std::variant<llvm::Module*, llvm::Value*, llvm::Function*, llvm::Type*, std::string, U32> TLLVMIRData;


	enum E_ATTRIBUTES_VALUES : U32
	{
		AV_STATIC            = 0x1,
		AV_FUNC_ARG_DECL     = 0x2,
		AV_ENTRY_POINT       = 0x4,
		AV_STRUCT_FIELD_DECL = 0x8,
		AV_NATIVE_FUNC		 = 0x10,
		AV_RVALUE            = 0x20,
		AV_POINTER           = 0x40,
		AV_AGGREGATE_TYPE    = 0x80,
	};


	constexpr U32 SignificantAttributesMask = AV_STATIC | AV_RVALUE | AV_POINTER | AV_AGGREGATE_TYPE;


	enum E_LEXER_ERRORS : U32
	{
		LE_INVALID_END_OF_MULTILINE_COMMENT,
	};


	typedef struct TLexerErrorInfo
	{
		E_LEXER_ERRORS mErrorType;

		U32            mPos;			// horizontal position

		U32            mLine;			// vertical position

	} TLexerErrorInfo;


	typedef struct TSemanticAnalyserMessageInfo
	{
		E_SEMANTIC_ANALYSER_MESSAGE mMessage;

		E_MESSAGE_TYPE              mType;
	} TSemanticAnalyserMessageInfo;


	typedef U32 TSymbolHandle;

	constexpr TSymbolHandle InvalidSymbolHandle = 0;
}

#endif
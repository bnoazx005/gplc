#include "utils/Utils.h"
#include "lexer/gplcTokens.h"


namespace gplc
{
	std::string CMessageOutputUtils::MessageTypeToString(E_MESSAGE_TYPE messageType)
	{
		switch (messageType)
		{
			case E_MESSAGE_TYPE::MT_ERROR:
				return "Error";
			case E_MESSAGE_TYPE::MT_WARNING:
				return "Warning";
			case E_MESSAGE_TYPE::MT_INFO:
				return "Info";
		}
	}

	std::string CMessageOutputUtils::LexerMessageToString(E_LEXER_ERRORS message)
	{
		switch (message)
		{
			case LE_INVALID_END_OF_MULTILINE_COMMENT:
				return "The end of multi-line comment wasn't found";
		}

		return {};
	}

	std::string CMessageOutputUtils::ParserMessageToString(const TParserErrorInfo& info)
	{
		switch (info.mType)
		{
			case PE_INVALID_ENUMERATOR_NAME:
				return "Invalid enumerator's name";	
			case PE_INVALID_ENUMERATOR_VALUE:
				return "Invalid enumerator's value";
			case PE_UNEXPECTED_TOKEN:
				{
					auto details = std::get<TParserErrorInfo::TUnexpectedTokenInfo>(info.mDetails);

					std::string outputMessage = std::string("An unexpected token was found at ")
																	.append(std::to_string(info.mPos))
																	.append(". ")
																	.append(TokenTypeToString(details.mActualToken))
																	.append(" instead of ")
																	.append(TokenTypeToString(details.mExpectedToken));

					return outputMessage;
				}
			case PE_INVALID_ENVIRONMENT:
				return "Some of input arguments of IParser::Parse method are invalid";
			case PE_INVALID_TYPE:
				return "Invalid type has found";
		}

		return {};
	}

	std::string CMessageOutputUtils::SemanticAnalyserMessageToString(E_SEMANTIC_ANALYSER_MESSAGE message)
	{
		switch (message)
		{
			case SAE_IDENTIFIER_ALREADY_DECLARED:
				return "Identifier's already been declared";
			case SAE_UNDECLARED_IDENTIFIER:
				return "Undeclared identifier was detected";
			case SAE_REDUNDANT_LOOP_STATEMENT:
				return "Possibly redundant infinite loop is found";
			case SAE_LOGIC_EXPR_IS_EXPECTED:
				return "Logical expression is expected here";
			case SAE_INCOMPATIBLE_TYPE_OF_ASSIGNED_LAMBDA:
				return "Type mismatch of assigned lambda";
			case SAE_INCOMPATIBLE_TYPES_INSIDE_EXPR:
				return "Type mismatch within the expression";
			case SAE_SINGLE_FUNC_IDENTIFIER_IS_EXPECTED:
				return "The only function can be defined at once";
			case SAE_FUNC_MULTIPLE_PARAM_PER_DECL_ARE_NOT_ALLOWED:
				return "Multiple variables per declaration are not allowed here";
			case SAE_UNDEFINED_TYPE:
				return "Undefined type is found";
			case SAE_INTERRUPT_STATEMENT_OUTSIDE_LOOP_IS_NOT_ALLOWED:
				return "Cannot use 'break' or 'continue' outside of loop's block";
			case SAE_BLOCKING_LOOP:
				return "The infinite loop does not contain any break operator";
			case SAE_TRY_TO_ACCESS_UNDEFINED_FIELD:
				return "Try to get an access to undeclared field";
		}

		return {};
	}
}
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


	///< Result type and its values

	typedef unsigned long Result;	///<The type is used as function's result


	/*!
		\brief An enumeration which contains all allowed values of Result type
	*/

	enum E_RESULT_VALUE
	{
		RV_SUCCESS = 0x0,	///< Successfully done
		RV_FAIL    = 0x1,	///< Critical error during execution
	};

	/*!
		\brief Macro defenitions for analyzing of result's value

		\param[in] resultValue Result's value

		\return A logical expression
	*/

	#define SUCCESS(resultType) (resultType == RV_SUCCESS)	/*(~(resultType & RV_FAIL))*/
}

#endif
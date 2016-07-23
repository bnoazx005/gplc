/*!
	\author Ildar Kasimov
	\date   22.07.2016
	\copyright

	\brief The file contains CSymTable declaration

	\todo
*/
#ifndef GPLC_SYM_TABLE_H
#define GPLC_SYM_TABLE_H


namespace gplc
{

	struct TTypeDescription; ///< Forward declarations

	/*!
		\brief ISymTable interface
	*/

	class ISymTable
	{
		private:

		#pragma pack(push, 1)

			/*!
				\brief The TSymTableEntry structure
			*/

			struct TSymTableEntry
			{
				TSymTableEntry*                           mParentScope;

				std::vector<TSymTableEntry*>              mNestedScopes;

				std::map<std::wstring, TTypeDescription*> mVariables;
			};

		#pragma pack(pop)

		public:
			ISymTable();
			virtual ~ISymTable();

			virtual Result EnterScope() = 0;

			virtual Result LeaveScope() = 0;

			virtual Result AddVariable() = 0;

			//virtual 
		protected:
	};
}

#endif
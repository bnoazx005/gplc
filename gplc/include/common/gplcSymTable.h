/*!
	\author Ildar Kasimov
	\date   22.07.2016
	\copyright

	\brief The file contains CSymTable declaration

	\todo
*/
#ifndef GPLC_SYM_TABLE_H
#define GPLC_SYM_TABLE_H


#include "gplcTypes.h"
#include <vector>
#include <map>


namespace gplc
{
	class CType;


	/*!
		\brief ISymTable interface
	*/

	class ISymTable
	{
		protected:

		#pragma pack(push, 1)

			/*!
				\brief The TSymTableEntry structure
			*/

			struct TSymTableEntry
			{
				TSymTableEntry*                      mParentScope;

				std::vector<TSymTableEntry*>         mNestedScopes;

				std::map<std::wstring, const CType*> mVariables;
			};

		#pragma pack(pop)

		public:
			ISymTable();
			virtual ~ISymTable();

			virtual Result EnterScope() = 0;

			virtual Result LeaveScope() = 0;

			virtual Result AddVariable() = 0;

			virtual const CType* LookUp(const std::wstring& variableName) const = 0;
		protected:
			ISymTable(const ISymTable& table);
	};


	/*!
		\brief CSymTable class
	*/

	class CSymTable : public ISymTable
	{
		public:
			CSymTable();
			virtual ~CSymTable();

			virtual Result EnterScope();

			virtual Result LeaveScope();

			virtual Result AddVariable(const std::wstring& variableName, const CType* typeDesc);

			virtual const CType* LookUp(const std::wstring& variableName) const;
		protected:
			CSymTable(const CSymTable& table);

			const CType* _lookUp(const TSymTableEntry* entry, const std::wstring& variableName) const;
		protected:
			TSymTableEntry* mpGlobalScopeEntry;

			TSymTableEntry* mpCurrScopeEntry;
	};
}

#endif
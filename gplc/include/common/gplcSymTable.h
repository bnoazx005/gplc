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
				TSymTableEntry*                     mParentScope;

				std::vector<TSymTableEntry*>        mNestedScopes;

				std::map<std::string, const CType*> mVariables;
			};

		#pragma pack(pop)

		public:
			ISymTable();
			virtual ~ISymTable();

			virtual Result EnterScope() = 0;

			virtual Result LeaveScope() = 0;

			virtual Result AddVariable(const std::string& variableName, const CType* typeDesc) = 0;

			virtual const CType* LookUp(const std::string& variableName) const = 0;
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

			Result EnterScope() override;

			Result LeaveScope() override;

			Result AddVariable(const std::string& variableName, const CType* typeDesc) override;

			const CType* LookUp(const std::string& variableName) const override;
		protected:
			CSymTable(const CSymTable& table);

			const CType* _lookUp(const TSymTableEntry* entry, const std::string& variableName) const;

			void _removeScope(TSymTableEntry** scope);
		protected:
			TSymTableEntry* mpGlobalScopeEntry;

			TSymTableEntry* mpCurrScopeEntry;
	};
}

#endif
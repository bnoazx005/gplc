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
#include <unordered_map>
#include "utils/CResult.h"


namespace gplc
{
	class CBaseValue;
	class CType;


	/*!
		\brief There are few main types of entities which a symbol can refers to:
				- Unnamed scope (nested local scopes, global scope)
				- Named scope (structs, enums, modules, etc)

		The main difference between two of these is the named scope's variables are visible
		everywhere inside of that scope, no matter where they are declared before or after 
		their usage. Unnamed scopes work in manner of C style scopes, in other words you 
		should declare or define variable before its usage.
	*/
/*
	class ISymbolInfo
	{
		public:
			virtual ~ISymbolInfo() = default;


		protected:
			ISymbolInfo() = default;
			ISymbolInfo(const ISymbolInfo& symbolInfo) = default;
	};*/


	struct TSymbolDesc
	{
		CBaseValue* mpValue;

		CType*        mpType;
	};


	/*!
		\brief ISymTable interface
	*/

	class ISymTable
	{
		protected:
			struct TSymTableEntry;

			typedef std::unordered_map<std::string, TSymbolDesc>     TSymbolsMap;

			typedef std::unordered_map<std::string, TSymTableEntry*> TNamedScopesMap;

			/*!
				\brief The TSymTableEntry structure
			*/

			struct TSymTableEntry
			{
				TSymTableEntry*              mParentScope;

				std::vector<TSymTableEntry*> mNestedScopes;

				TSymbolsMap                  mVariables;

				TNamedScopesMap              mNamedScopes;
			};

		public:
			ISymTable();
			virtual ~ISymTable();

			virtual Result Lock() = 0;

			virtual Result Unlock() = 0;

			virtual Result EnterNamedScope(const std::string& scopeName) = 0;
			virtual Result EnterScope() = 0;

			virtual Result LeaveScope() = 0;

			virtual Result AddVariable(const std::string& variableName, const TSymbolDesc& typeDesc) = 0;

			virtual const TSymbolDesc* LookUp(const std::string& variableName) const = 0;

			virtual bool IsLocked() const = 0;
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

			Result Lock() override;

			Result Unlock() override;

			Result EnterNamedScope(const std::string& scopeName) override;
			Result EnterScope() override;

			Result LeaveScope() override;

			Result AddVariable(const std::string& variableName, const TSymbolDesc& typeDesc) override;
			
			const TSymbolDesc* LookUp(const std::string& variableName) const override;

			bool IsLocked() const override;
		protected:
			CSymTable(const CSymTable& table);

			bool _internalLookUp(TSymTableEntry* entry, const std::string& variableName) const;

			const TSymbolDesc* _lookUp(TSymTableEntry* entry, const std::string& variableName) const;

			void _removeScope(TSymTableEntry** scope);
		protected:
			TSymTableEntry* mpGlobalScopeEntry;

			TSymTableEntry* mpCurrScopeEntry;

			bool            mIsLocked;
	};
}

#endif
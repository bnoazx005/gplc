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
	   
	struct TSymbolDesc
	{
		std::string   mName;

		CBaseValue*   mpValue;

		CType*        mpType;
	};


	/*!
		\brief ISymTable interface
	*/

	class ISymTable
	{
		protected:
			struct TSymTableEntry;

			typedef std::unordered_map<std::string, TSymbolHandle>   TSymbolsMap;

			typedef std::unordered_map<std::string, TSymTableEntry*> TNamedScopesMap;

			typedef std::vector<std::pair<bool, TSymbolDesc>>        TSymbolsArray;

			/*!
				\brief The TSymTableEntry structure
			*/

			struct TSymTableEntry
			{
				TSymTableEntry*              mParentScope;

				std::vector<TSymTableEntry*> mNestedScopes;

				TSymbolsMap                  mVariables;

				TNamedScopesMap              mNamedScopes;

				CType*                       mpType;	///< A type of a named scope, equals to nullptr for unnamed scopes
			};

		public:
			ISymTable();
			virtual ~ISymTable();

			virtual Result Lock() = 0;

			virtual Result Unlock() = 0;

			virtual Result CreateNamedScope(const std::string& scopeName) = 0;
			virtual Result CreateScope() = 0;

			virtual Result VisitNamedScope(const std::string& scopeName) = 0;
			virtual Result VisitScope() = 0;

			virtual Result LeaveScope() = 0;

			virtual TSymbolHandle AddVariable(const TSymbolDesc& typeDesc) = 0;

			virtual const TSymbolDesc* LookUp(const std::string& variableName) const = 0;
			virtual TSymbolDesc* LookUp(TSymbolHandle symbolHandle) const = 0;

			virtual TSymTableEntry* LookUpNamedScope(const std::string& scopeName) const = 0;

			virtual bool IsLocked() const = 0;

			virtual TSymbolHandle GetSymbolHandleByName(const std::string& variable) const = 0;
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

			Result CreateNamedScope(const std::string& scopeName) override;
			Result CreateScope() override;

			Result VisitNamedScope(const std::string& scopeName) override;
			Result VisitScope() override;

			Result LeaveScope() override;

			TSymbolHandle AddVariable(const TSymbolDesc& typeDesc) override;
			
			const TSymbolDesc* LookUp(const std::string& variableName) const override;
			TSymbolDesc* LookUp(TSymbolHandle symbolHandle) const override;

			TSymTableEntry* LookUpNamedScope(const std::string& scopeName) const override;

			bool IsLocked() const override;

			TSymbolHandle GetSymbolHandleByName(const std::string& variable) const override;
		protected:
			CSymTable(const CSymTable& table);

			bool _internalLookUp(TSymTableEntry* entry, const std::string& variableName) const;

			TSymbolHandle _lookUp(TSymTableEntry* entry, const std::string& variableName) const;

			void _removeScope(TSymTableEntry** scope);
		protected:
			TSymbolsArray   mSymbols; ///< All symbols in all scopes are stored here

			TSymTableEntry* mpGlobalScopeEntry;

			TSymTableEntry* mpCurrScopeEntry;

			bool            mIsLocked;
	};
}

#endif
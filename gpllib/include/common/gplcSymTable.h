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
#include <functional>
#include "utils/CResult.h"


namespace gplc
{
	class CASTExpressionNode;
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
		std::string         mName;

		CASTExpressionNode* mpValue;

		CType*              mpType;
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

			typedef std::function<void(ISymTable*)>                  TSymTableTransactionCallback;

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

				I32                          mScopeIndex;
			};

		public:
			ISymTable();
			virtual ~ISymTable();

			virtual Result Lock() = 0;

			virtual Result Unlock() = 0;

			virtual Result CreateNamedScope(const std::string& scopeName) = 0;
			virtual Result CreateScope() = 0;

			virtual Result VisitNamedScope(const std::string& scopeName) = 0;

			/*!
				\brief The method works the same as VisitNamedScope, but in the end restore original state of a table after
				transaction is evaluated. The best primer of usage is resolving access operator or something like that.
			*/

			virtual Result VisitNamedScopeWithRestore(const std::string& scopeName, const TSymTableTransactionCallback& transaction) = 0;
			virtual Result VisitScope() = 0;

			virtual Result LeaveScope() = 0;

			virtual TSymbolHandle AddVariable(const TSymbolDesc& typeDesc) = 0;

			virtual const TSymbolDesc* LookUp(const std::string& variableName) const = 0;
			virtual TSymbolDesc* LookUp(TSymbolHandle symbolHandle) = 0;
			virtual const TSymbolDesc* LookUp(TSymbolHandle symbolHandle) const = 0;

			virtual TSymTableEntry* LookUpNamedScope(const std::string& scopeName) const = 0;

			virtual bool IsLocked() const = 0;

			virtual TSymbolHandle GetSymbolHandleByName(const std::string& variable) const = 0;

			virtual std::string RenameReservedIdentifier(const std::string& identifier) const = 0;

			virtual CType* GetParentScopeType() const = 0;
			virtual CType* GetCurrentScopeType() const = 0;

			virtual void DumpScopesStructure() const = 0;
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
			Result VisitNamedScopeWithRestore(const std::string& scopeName, const TSymTableTransactionCallback& transaction) override;
			Result VisitScope() override;

			Result LeaveScope() override;

			TSymbolHandle AddVariable(const TSymbolDesc& typeDesc) override;
			
			const TSymbolDesc* LookUp(const std::string& variableName) const override;
			TSymbolDesc* LookUp(TSymbolHandle symbolHandle) override;
			const TSymbolDesc* LookUp(TSymbolHandle symbolHandle) const override;

			TSymTableEntry* LookUpNamedScope(const std::string& scopeName) const override;

			bool IsLocked() const override;

			TSymbolHandle GetSymbolHandleByName(const std::string& variable) const override;

			std::string RenameReservedIdentifier(const std::string& identifier) const override;
			
			CType* GetParentScopeType() const override;
			CType* GetCurrentScopeType() const override;

			void DumpScopesStructure() const override;
		protected:
			CSymTable(const CSymTable& table);

			bool _internalLookUp(TSymTableEntry* entry, const std::string& variableName) const;

			TSymbolHandle _lookUp(TSymTableEntry* entry, const std::string& variableName) const;

			void _removeScope(TSymTableEntry** scope);
		protected:
			TSymbolsArray   mSymbols; ///< All symbols in all scopes are stored here

			TSymTableEntry* mpGlobalScopeEntry;

			TSymTableEntry* mpCurrScopeEntry;

			TSymTableEntry* mpPrevScopeEntry;

			bool            mIsLocked;

			I32             mLastVisitedScopeIndex;

			I32             mPrevVisitedScopeIndex; ///< \note The field is only updated when visiting VisitNamedScope

			bool            mIsReadMode;
	};
}

#endif
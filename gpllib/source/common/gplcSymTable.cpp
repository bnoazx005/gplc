/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains defenition of symbols' table

	\todo
*/

#include "common/gplcSymTable.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcValues.h"
#include <cassert>
#include <iostream>


namespace gplc
{

	/*!
		ISymTable defenition
	*/

	ISymTable::ISymTable()
	{
	}

	ISymTable::ISymTable(const ISymTable& table)
	{
	}

	ISymTable::~ISymTable()
	{
	}


	/*!
		CSymTable defenition
	*/

	CSymTable::CSymTable() :
		mpGlobalScopeEntry(nullptr), mpCurrScopeEntry(nullptr), mIsLocked(false), mLastVisitedScopeIndex(-1)
	{
		mpGlobalScopeEntry = new TSymTableEntry();

		mpGlobalScopeEntry->mParentScope = nullptr;

		mpCurrScopeEntry = mpGlobalScopeEntry;
		mpPrevScopeEntry = mpCurrScopeEntry;

		mPrevVisitedScopeIndex = -1;
	}

	CSymTable::CSymTable(const CSymTable& table):
		ISymTable(table), mIsLocked(false), mLastVisitedScopeIndex(-1)
	{
	}

	CSymTable::~CSymTable()
	{
		_removeScope(&mpGlobalScopeEntry);
	}

	Result CSymTable::Lock()
	{
		if (mIsLocked)
		{
			return RV_FAIL;
		}

		mIsLocked = true;

		return RV_SUCCESS;
	}

	Result CSymTable::Unlock()
	{
		if (!mIsLocked)
		{
			return RV_FAIL;
		}

		mIsLocked = false;

		return RV_SUCCESS;
	}

	Result CSymTable::CreateNamedScope(const std::string& scopeName)
	{
		if (mIsLocked || !mpCurrScopeEntry)
		{
			return RV_FAIL;
		}

		mIsReadMode = false;

		TSymTableEntry* pNestedTable = new TSymTableEntry();

		auto iter = mpCurrScopeEntry->mNamedScopes.find(scopeName);

		if (iter != mpCurrScopeEntry->mNamedScopes.cend())
		{
			return RV_FAIL;
		}

		mpCurrScopeEntry->mNamedScopes.insert({ scopeName, pNestedTable });

		pNestedTable->mParentScope = mpCurrScopeEntry;

		mpCurrScopeEntry = pNestedTable;

		mpCurrScopeEntry->mScopeIndex = -1;

		return RV_SUCCESS;
	}

	Result CSymTable::CreateScope()
	{
		if (mIsLocked || !mpCurrScopeEntry)
		{
			return RV_FAIL;
		}

		mIsReadMode = false;

		TSymTableEntry* pNestedTable = new TSymTableEntry();

		U32 scopeIndex = mpCurrScopeEntry->mNestedScopes.size();

		mpCurrScopeEntry->mNestedScopes.push_back(pNestedTable);

		pNestedTable->mParentScope = mpCurrScopeEntry;

		mpCurrScopeEntry = pNestedTable;

		mpCurrScopeEntry->mScopeIndex = scopeIndex;

		return RV_SUCCESS;
	}

	Result CSymTable::VisitNamedScope(const std::string& scopeName)
	{
		if (mIsLocked || !mpCurrScopeEntry)
		{
			return RV_FAIL;
		}

		mpPrevScopeEntry = mpCurrScopeEntry;

		if (mpCurrScopeEntry->mScopeIndex >= 0)
		{
			mPrevVisitedScopeIndex = mLastVisitedScopeIndex;
		}
		
		mIsReadMode = true;

		auto pCurrScope = mpCurrScopeEntry;
		
		while (pCurrScope && (pCurrScope->mNamedScopes.find(scopeName) == pCurrScope->mNamedScopes.cend()) && pCurrScope->mParentScope)
		{
			pCurrScope = pCurrScope->mParentScope;
		}

		if (!pCurrScope)
		{
			return RV_FAIL;
		}

		mpCurrScopeEntry = pCurrScope->mNamedScopes[scopeName];

		mLastVisitedScopeIndex = -1;

		assert(mpCurrScopeEntry);

		return RV_SUCCESS;
	}

	Result CSymTable::VisitNamedScopeWithRestore(const std::string& scopeName, const ISymTable::TSymTableTransactionCallback& transaction)
	{
		auto pCurrScopeEntry = mpCurrScopeEntry;

		I32 prevScopeIndex = mLastVisitedScopeIndex;

		Result result = VisitNamedScope(scopeName);

		if (!SUCCESS(result))
		{
			return result;
		}

		transaction(this);

		// restore previous state
		mpCurrScopeEntry = pCurrScopeEntry;

		mLastVisitedScopeIndex = prevScopeIndex;

		return RV_SUCCESS;
	}

	Result CSymTable::VisitScope()
	{
		if (mIsLocked || !mpCurrScopeEntry || mpCurrScopeEntry->mNestedScopes.size() < 1)
		{
			return RV_FAIL;
		}
		
		mIsReadMode = true;

		mpPrevScopeEntry = mpCurrScopeEntry;

		mpCurrScopeEntry = mpCurrScopeEntry->mNestedScopes[mLastVisitedScopeIndex + 1];

		mLastVisitedScopeIndex = -1;

		return RV_SUCCESS;
	}

	Result CSymTable::LeaveScope()
	{
		if (mIsLocked || !mpCurrScopeEntry->mParentScope) //we stay in a global scope
		{
			return RV_FAIL;
		}
		
		I32 currScopeIndex = mpCurrScopeEntry->mScopeIndex;

		bool isUnnamedScope = currScopeIndex >= 0;

		mpCurrScopeEntry = mpCurrScopeEntry->mParentScope;

		// \note move to next neighbour scope if we currently stay in unnamed one and there is this next neighbour
		if (mIsReadMode && isUnnamedScope && (mpCurrScopeEntry->mNestedScopes.size() > currScopeIndex))
		{
			mLastVisitedScopeIndex = currScopeIndex;
		}

		// \note restore previous pointer when came back from VisitNamedScope
		if (mIsReadMode && !isUnnamedScope)
		{
			mLastVisitedScopeIndex = mPrevVisitedScopeIndex;
			mPrevVisitedScopeIndex = -1;
		}

		return RV_SUCCESS;
	}

	TSymbolHandle CSymTable::AddVariable(const TSymbolDesc& typeDesc)
	{
		if (mIsLocked)
		{
			return InvalidSymbolHandle;
		}

		std::string identifier = RenameReservedIdentifier(typeDesc.mName);

		if (identifier == "_lang_entry_main")
		{
			CFunctionType* pMainFuncType = dynamic_cast<CFunctionType*>(typeDesc.mpType);

			pMainFuncType->SetAttributes(pMainFuncType->GetAttributes() | AV_ENTRY_POINT);
		}

		if (_internalLookUp(mpCurrScopeEntry, identifier))
		{
			return InvalidSymbolHandle;
		}

		// this trick is used to assign a name for a function pointer, for other types it does nothing
		if (typeDesc.mpType && typeDesc.mpType->GetType() == CT_FUNCTION)
		{
			typeDesc.mpType->SetName(identifier);			
		}

		TSymbolHandle symbolHandle = mSymbols.size() + 1;

		mSymbols.push_back({ true, typeDesc }); // true means that the record is valid 

		mpCurrScopeEntry->mVariables.insert({ identifier, symbolHandle });
		
		return symbolHandle;
	}

	const TSymbolDesc* CSymTable::LookUp(const std::string& variableName) const
	{
		TSymbolHandle handle = _lookUp(mpCurrScopeEntry, RenameReservedIdentifier(variableName));

		/*! \note when we work with aggregate types such as structures or modules we need to go into their scopes.
			This situation makes current local variables invisible for us from that point,
			so we also check previous active scope, which is our local scope 
		*/
		if ((handle == InvalidSymbolHandle) && (mpCurrScopeEntry->mScopeIndex < 0))
		{
			handle = _lookUp(mpPrevScopeEntry, RenameReservedIdentifier(variableName));
		}

		return handle != InvalidSymbolHandle ? (&mSymbols[handle - 1].second) : nullptr;
	}

	TSymbolDesc* CSymTable::LookUp(TSymbolHandle symbolHandle)
	{
		if (symbolHandle == InvalidSymbolHandle || symbolHandle > mSymbols.size())
		{
			return nullptr;
		}

		TSymbolsArray::value_type& value = mSymbols[symbolHandle - 1];

		return value.first /* is valid */ ? &value.second : nullptr;
	}

	const TSymbolDesc* CSymTable::LookUp(TSymbolHandle symbolHandle) const
	{
		if (symbolHandle == InvalidSymbolHandle || symbolHandle > mSymbols.size())
		{
			return nullptr;
		}

		const TSymbolsArray::value_type& value = mSymbols.at(symbolHandle - 1);

		return value.first /* is valid */ ? &value.second : nullptr;
	}

	CSymTable::TSymTableEntry* CSymTable::LookUpNamedScope(const std::string& scopeName) const
	{
		const TSymTableEntry* pCurrEntry = mpCurrScopeEntry;

		auto findScope = [](const TSymTableEntry* pCurrEntry, const std::string& scopeName) -> TSymTableEntry*
		{
			auto& currNamedScopes = pCurrEntry->mNamedScopes;

			auto iter = currNamedScopes.cbegin();

			if ((iter = currNamedScopes.find(scopeName)) != currNamedScopes.cend())
			{
				return iter->second;
			}

			return nullptr;
		};
		
		TSymTableEntry* pFoundResult = findScope(pCurrEntry, scopeName);

		if (pFoundResult)
		{
			return pFoundResult;
		}

		while (pCurrEntry->mParentScope)
		{
			pCurrEntry = pCurrEntry->mParentScope;
			
			if (pFoundResult = findScope(pCurrEntry, scopeName))
			{
				return pFoundResult;
			}
		}

		return nullptr;
	}
	
	bool CSymTable::IsLocked() const
	{
		return mIsLocked;
	}

	TSymbolHandle CSymTable::GetSymbolHandleByName(const std::string& variable) const
	{
		auto handle = _lookUp(mpCurrScopeEntry, variable);

		/*! \note when we work with aggregate types such as structures or modules we need to go into their scopes.
			This situation makes current local variables invisible for us from that point,
			so we also check previous active scope, which is our local scope
		*/
		if ((handle == InvalidSymbolHandle) && (mpCurrScopeEntry->mScopeIndex < 0))
		{
			handle = _lookUp(mpPrevScopeEntry, variable);
		}

		return handle;
	}

	std::string CSymTable::RenameReservedIdentifier(const std::string& identifier) const
	{
		if (identifier == "main")
		{
			return "_lang_entry_main";
		}

		if (identifier == "print")
		{
			return "puts";
		}

		return identifier;
	}

	CType* CSymTable::GetParentScopeType() const
	{
		auto pParentScope = mpCurrScopeEntry->mParentScope;

		return pParentScope ? pParentScope->mpType : nullptr;
	}

	CType* CSymTable::GetCurrentScopeType() const
	{
		auto pCurrentScope = mpCurrScopeEntry;

		return pCurrentScope ? pCurrentScope->mpType : nullptr;
	}

	void CSymTable::DumpScopesStructure() const
	{
		std::function<void(const TSymTableEntry*, const TSymTableEntry*, U32)> _printScopeInfo = 
		[this, &_printScopeInfo](const TSymTableEntry* pCurrTableEntry,  const TSymTableEntry* pActiveEntry, U32 currLevel)
		{
			if (!pCurrTableEntry)
			{
				return;
			}

			CType* pCurrScopeType = pCurrTableEntry->mpType;

			for (U32 i = 0; i < currLevel; ++i)
			{
				std::cout << "  ";
			}

			std::cout << ((pCurrTableEntry == pActiveEntry) ? "*" : "")
					  << (pCurrScopeType ? pCurrScopeType->GetName() : "<unnamed scope>") 
					  << ": " << (pCurrScopeType ? pCurrScopeType->ToShortAliasString() : "scope") << std::endl;

			for (auto& currVariableInfo : pCurrTableEntry->mVariables)
			{
				for (U32 i = 0; i < currLevel + 1; ++i)
				{
					std::cout << "  ";
				}

				auto pCurrVariableType = LookUp(currVariableInfo.second)->mpType;

				std::cout << currVariableInfo.first << ": " << (pCurrVariableType ? pCurrVariableType->ToShortAliasString() : "unknown") << std::endl;
			}

			for (auto pCurrNestedScope : pCurrTableEntry->mNestedScopes)
			{
				_printScopeInfo(pCurrNestedScope, pActiveEntry, currLevel + 1);
			}

			for (auto pCurrNamedScope : pCurrTableEntry->mNamedScopes)
			{
				_printScopeInfo(pCurrNamedScope.second, pActiveEntry, currLevel + 1);
			}
		};

		_printScopeInfo(mpGlobalScopeEntry, mpCurrScopeEntry, 0);
	}

	TSymbolHandle CSymTable::_lookUp(TSymTableEntry* entry, const std::string& variableName) const
	{
		const TSymbolsMap& table = entry->mVariables;
				
		TSymbolsMap::const_iterator iter;

		if ((iter = table.find(variableName)) != table.cend())
		{
			return iter->second;
		}

		//search in outter scopes
		TSymTableEntry* pCurrSymTable = entry->mParentScope;

		TSymbolHandle currSymbolHandle = InvalidSymbolHandle;

		while (pCurrSymTable)
		{
			const TSymbolsMap& currTable = pCurrSymTable->mVariables;

			for (auto& currVariable : currTable)
			{
				if (currSymbolHandle = _lookUp(pCurrSymTable, variableName))
				{
					return currSymbolHandle;
				}
			}

			pCurrSymTable = pCurrSymTable->mParentScope;
		}

		return InvalidSymbolHandle;
	}

	bool CSymTable::_internalLookUp(TSymTableEntry* entry, const std::string& variableName) const
	{
		TSymbolsMap& table = entry->mVariables;

		return table.find(variableName) != table.cend();
	}

	void CSymTable::_removeScope(TSymTableEntry** scope)
	{
		if (*scope == nullptr)
		{
			return;
		}

		TSymTableEntry* pScope = *scope;

		TSymbolsMap currTable = pScope->mVariables;

		if (!currTable.empty())
		{
			TSymbolsMap::iterator var = currTable.begin();
			
			currTable.clear();
		}

		std::vector<TSymTableEntry*> scopes = pScope->mNestedScopes;
		
		for (std::vector<TSymTableEntry*>::iterator nestedScope = scopes.begin(); nestedScope != scopes.end(); nestedScope++)
		{
			_removeScope(&(*nestedScope));
		}

		scopes.clear();

		delete *scope;

		*scope = nullptr;
	}
}
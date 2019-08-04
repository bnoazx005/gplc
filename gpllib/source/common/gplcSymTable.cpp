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
		mpGlobalScopeEntry(nullptr), mpCurrScopeEntry(nullptr), mIsLocked(false)
	{
		mpGlobalScopeEntry = new TSymTableEntry();

		mpGlobalScopeEntry->mParentScope = nullptr;

		mpCurrScopeEntry = mpGlobalScopeEntry;
	}

	CSymTable::CSymTable(const CSymTable& table):
		ISymTable(table), mIsLocked(false)
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
		if (mIsLocked || mpCurrScopeEntry == nullptr)
		{
			return RV_FAIL;
		}

		TSymTableEntry* pNestedTable = new TSymTableEntry();

		auto iter = mpCurrScopeEntry->mNamedScopes.find(scopeName);

		if (iter != mpCurrScopeEntry->mNamedScopes.cend())
		{
			return RV_FAIL;
		}

		mpCurrScopeEntry->mNamedScopes.insert({ scopeName, pNestedTable });

		pNestedTable->mParentScope = mpCurrScopeEntry;

		mpCurrScopeEntry = pNestedTable;

		return RV_SUCCESS;
	}

	Result CSymTable::CreateScope()
	{
		if (mIsLocked || mpCurrScopeEntry == nullptr)
		{
			return RV_FAIL;
		}

		TSymTableEntry* pNestedTable = new TSymTableEntry();

		mpCurrScopeEntry->mNestedScopes.push_back(pNestedTable);

		pNestedTable->mParentScope = mpCurrScopeEntry;

		mpCurrScopeEntry = pNestedTable;

		return RV_SUCCESS;
	}

	Result CSymTable::VisitNamedScope(const std::string& scopeName)
	{
		if (mIsLocked || mpCurrScopeEntry == nullptr || (mpCurrScopeEntry->mNamedScopes.find(scopeName) == mpCurrScopeEntry->mNamedScopes.cend()))
		{
			return RV_FAIL;
		}

		mpCurrScopeEntry = mpCurrScopeEntry->mNamedScopes[scopeName];

		return RV_SUCCESS;
	}

	Result CSymTable::VisitScope()
	{
		if (mIsLocked || mpCurrScopeEntry == nullptr || mpCurrScopeEntry->mNestedScopes.size() < 1)
		{
			return RV_FAIL;
		}

		mpCurrScopeEntry = mpCurrScopeEntry->mNestedScopes.front();

		return RV_SUCCESS;
	}

	Result CSymTable::LeaveScope()
	{
		if (mIsLocked || mpCurrScopeEntry->mParentScope == nullptr) //we stay in a global scope
		{
			return RV_FAIL;
		}
		
		mpCurrScopeEntry = mpCurrScopeEntry->mParentScope;

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
		return _lookUp(mpCurrScopeEntry, variable);
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
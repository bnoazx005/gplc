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

	Result CSymTable::EnterNamedScope(const std::string& scopeName)
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

	Result CSymTable::EnterScope()
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

	Result CSymTable::LeaveScope()
	{
		if (mIsLocked || mpCurrScopeEntry->mParentScope == nullptr) //we stay in a global scope
		{
			return RV_FAIL;
		}
		
		mpCurrScopeEntry = mpCurrScopeEntry->mParentScope;

		return RV_SUCCESS;
	}

	Result CSymTable::AddVariable(const std::string& variableName, const TSymbolDesc& typeDesc)
	{
		if (mIsLocked)
		{
			return RV_SUCCESS;
		}

		std::string identifier { variableName };

		// rename "main into "_lang_entry_main"
		// \todo TEMP code, replace this with proper solution later
		if (variableName == "main")
		{
			identifier = "_lang_entry_main";

			CFunctionType* pMainFuncType = dynamic_cast<CFunctionType*>(typeDesc.mpType);

			pMainFuncType->SetAttributes(pMainFuncType->GetAttributes() | AV_ENTRY_POINT);
		}

		if (_internalLookUp(mpCurrScopeEntry, identifier))
		{
			return RV_FAIL;
		}

		// this trick is used to assign a name for a function pointer, for other types it does nothing
		if (typeDesc.mpType)
		{
			typeDesc.mpType->SetName(identifier);			
		}

		mpCurrScopeEntry->mVariables.insert({ identifier, typeDesc });
		
		return RV_SUCCESS;
	}

	const TSymbolDesc* CSymTable::LookUp(const std::string& variableName) const
	{
		std::string identifier{ variableName };

		// rename "main into "_lang_entry_main"
		// \todo TEMP code, replace this with proper solution later
		if (variableName == "main")
		{
			identifier = "_lang_entry_main";
		}

		return _lookUp(mpCurrScopeEntry, identifier);
	}

	bool CSymTable::IsLocked() const
	{
		return mIsLocked;
	}

	const TSymbolDesc* CSymTable::_lookUp(TSymTableEntry* entry, const std::string& variableName) const
	{
		const TSymbolsMap& table = entry->mVariables;
				
		TSymbolsMap::const_iterator iter;

		if ((iter = table.find(variableName)) != table.cend())
		{
			return &iter->second;
		}

		//search in outter scopes
		TSymTableEntry* pCurrSymTable = entry->mParentScope;

		const TSymbolDesc* pCurrDesc = nullptr;

		while (pCurrSymTable)
		{
			const TSymbolsMap& currTable = pCurrSymTable->mVariables;

			for (auto& currVariable : currTable)
			{
				if (pCurrDesc = _lookUp(pCurrSymTable, variableName))
				{
					return pCurrDesc;
				}
			}

			pCurrSymTable = pCurrSymTable->mParentScope;
		}

		return nullptr;
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
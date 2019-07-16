/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains defenition of symbols' table

	\todo
*/

#include "common/gplcSymTable.h"
#include "common/gplcTypeSystem.h"
#include "common/gplcLiterals.h"
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

		if (_internalLookUp(mpCurrScopeEntry, variableName))
		{
			return RV_FAIL;
		}

		mpCurrScopeEntry->mVariables.insert({ variableName, typeDesc });
		
		return RV_SUCCESS;
	}

	const TSymbolDesc* CSymTable::LookUp(const std::string& variableName) const
	{
		return _lookUp(mpCurrScopeEntry, variableName);
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
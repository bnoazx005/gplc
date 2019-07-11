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

	CSymTable::CSymTable():
		mpGlobalScopeEntry(nullptr), mpCurrScopeEntry(nullptr)
	{
		mpGlobalScopeEntry = new TSymTableEntry();

		mpGlobalScopeEntry->mParentScope = nullptr;

		mpCurrScopeEntry = mpGlobalScopeEntry;
	}

	CSymTable::CSymTable(const CSymTable& table):
		ISymTable(table)
	{
	}

	CSymTable::~CSymTable()
	{
		_removeScope(&mpGlobalScopeEntry);
	}

	Result CSymTable::EnterScope()
	{
		if (mpCurrScopeEntry == nullptr)
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
		if (mpCurrScopeEntry->mParentScope == nullptr) //we stay in a global scope
		{
			return RV_FAIL;
		}
		
		mpCurrScopeEntry = mpCurrScopeEntry->mParentScope;

		return RV_SUCCESS;
	}

	Result CSymTable::AddVariable(const std::string& variableName, const TSymbolDesc& typeDesc)
	{
		if (_lookUp(mpCurrScopeEntry, variableName))
		{
			return RV_FAIL;
		}

		mpCurrScopeEntry->mVariables.insert({ variableName, typeDesc });
		
		return RV_SUCCESS;
	}

	TSymbolDesc* CSymTable::LookUp(const std::string& variableName) const
	{
		return _lookUp(mpCurrScopeEntry, variableName);
	}

	TSymbolDesc* CSymTable::_lookUp(TSymTableEntry* entry, const std::string& variableName) const
	{
		TSymbolsMap& table = entry->mVariables;
				
		if (table.find(variableName) != table.cend())
		{
			return &table[variableName];
		}

		//search in outter scopes
		TSymTableEntry* pCurrSymTable = mpCurrScopeEntry->mParentScope;

		while (pCurrSymTable && !_lookUp(pCurrSymTable, variableName))
		{
			pCurrSymTable = pCurrSymTable->mParentScope;
		}

		return nullptr;
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
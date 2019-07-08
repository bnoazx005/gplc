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
		mpCurrScopeEntry->mNestedScopes.push_back(new TSymTableEntry());

		TSymTableEntry* pParent = mpCurrScopeEntry;

		mpCurrScopeEntry = mpCurrScopeEntry->mNestedScopes.front();

		mpCurrScopeEntry->mParentScope = pParent;

		if (mpCurrScopeEntry == nullptr)
		{
			return RV_FAIL;
		}

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
		if (_lookUp(mpCurrScopeEntry, variableName) || _lookUp(mpGlobalScopeEntry, variableName))
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

	const TSymbolDesc* CSymTable::_lookUp(const TSymTableEntry* entry, const std::string& variableName) const
	{
		TSymbolsMap table = entry->mVariables;

		auto varDesc = table.cend();
		
		if ((varDesc = table.find(variableName)) != table.cend())
		{
			return &varDesc->second;
		}

		//search in inner scopes
		std::vector<TSymTableEntry*> nestedScopes = entry->mNestedScopes;

		U32 numOfNestedScopes = nestedScopes.size();

		const TSymbolDesc* pCurrDesc;

		for (U32 i = 0; i < numOfNestedScopes; i++)
		{
			if ((pCurrDesc = _lookUp(nestedScopes[i], variableName)) != nullptr)
			{
				return pCurrDesc;
			}
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
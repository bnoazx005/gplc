/*!
	\author Ildar Kasimov
	\date   23.07.2016
	\copyright

	\brief The file contains defenition of symbols' table

	\todo
*/

#include "common\gplcSymTable.h"
#include "common\gplcTypeSystem.h"


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

	Result CSymTable::AddVariable(const std::string& variableName, const CType* typeDesc)
	{
		mpCurrScopeEntry->mVariables.insert(std::make_pair(variableName, typeDesc));

		return RV_FAIL;
	}
	
	const CType* CSymTable::LookUp(const std::string& variableName) const
	{
		return _lookUp(mpGlobalScopeEntry, variableName);
	}

	const CType* CSymTable::_lookUp(const TSymTableEntry* entry, const std::string& variableName) const
	{
		std::map<std::string, const CType*> table = entry->mVariables;

		std::map<std::string, const CType*>::const_iterator varDesc = table.cend();

		if ((varDesc = table.find(variableName)) != table.cend())
		{
			return (*varDesc).second;
		}

		//search in inner scopes
		std::vector<TSymTableEntry*> nestedScopes = entry->mNestedScopes;

		U32 numOfNestedScopes = nestedScopes.size();

		const CType* pTmpTypeNode = nullptr;

		for (U32 i = 0; i < numOfNestedScopes; i++)
		{
			if ((pTmpTypeNode = _lookUp(nestedScopes[i], variableName)) != nullptr)
			{
				return pTmpTypeNode;
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

		std::map<std::string, const CType*> currTable = pScope->mVariables;

		if (!currTable.empty())
		{
			std::map<std::string, const CType*>::iterator var = currTable.begin();

			for (var = currTable.begin(); var != currTable.end(); var++)
			{
				delete (*var).second;
			}

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
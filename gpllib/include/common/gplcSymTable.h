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
	class CBaseLiteral;
	class CType;


	struct TSymbolDesc
	{
		CBaseLiteral* mpValue;

		CType*        mpType;
	};


	/*!
		\brief ISymTable interface
	*/

	class ISymTable
	{
		protected:
			typedef std::unordered_map<std::string, TSymbolDesc> TSymbolsMap;

			/*!
				\brief The TSymTableEntry structure
			*/

			struct TSymTableEntry
			{
				TSymTableEntry*              mParentScope;

				std::vector<TSymTableEntry*> mNestedScopes;

				TSymbolsMap                  mVariables;
			};

		public:
			ISymTable();
			virtual ~ISymTable();

			virtual Result Lock() = 0;

			virtual Result Unlock() = 0;

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
#include <catch.hpp>
#include <gplc.h>


TEST_CASE("CSymTable tests")
{
	gplc::ISymTable* pSymTable = new gplc::CSymTable();
	
	const gplc::CType* pTypeDesc = nullptr;

	SECTION("Global scope test")
	{
		pSymTable->AddVariable(L"x", new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0));
		pSymTable->AddVariable(L"y", new gplc::CType(gplc::CT_INT16, gplc::BTS_INT16, 0x0));
		pSymTable->AddVariable(L"z", new gplc::CType(gplc::CT_INT64, gplc::BTS_INT64, 0x0));

		pTypeDesc = pSymTable->LookUp(L"x");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->GetType() == gplc::CT_INT32);

		pTypeDesc = pSymTable->LookUp(L"y");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->GetType() == gplc::CT_INT16);

		pTypeDesc = pSymTable->LookUp(L"z");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->GetType() == gplc::CT_INT64);

		pTypeDesc = pSymTable->LookUp(L"unknown");

		REQUIRE(pTypeDesc == nullptr);
	}

	SECTION("Nested scopes test")
	{
		pSymTable->AddVariable(L"x", new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0));

		pSymTable->EnterScope();
		pSymTable->AddVariable(L"x", new gplc::CType(gplc::CT_INT16, gplc::BTS_INT16, 0x0));
		pSymTable->LeaveScope();

		pSymTable->AddVariable(L"z", new gplc::CType(gplc::CT_INT64, gplc::BTS_INT64, 0x0));

		pTypeDesc = pSymTable->LookUp(L"x");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->GetType() == gplc::CT_INT32);

		pTypeDesc = pSymTable->LookUp(L"x");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->GetType() == gplc::CT_INT32);

		pTypeDesc = pSymTable->LookUp(L"z");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->GetType() == gplc::CT_INT64);

		pTypeDesc = pSymTable->LookUp(L"unknown");

		REQUIRE(pTypeDesc == nullptr);
	}

	delete pSymTable;
}
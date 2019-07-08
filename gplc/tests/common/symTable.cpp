#include <catch.hpp>
#include <gplc.h>


TEST_CASE("CSymTable tests")
{
	gplc::ISymTable* pSymTable = new gplc::CSymTable();
	
	const gplc::TSymbolDesc* pTypeDesc = nullptr;

	SECTION("Global scope test")
	{
		gplc::TSymbolDesc d{ nullptr, new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0) };

		pSymTable->AddVariable("x", d);
		pSymTable->AddVariable("y", { nullptr, new gplc::CType(gplc::CT_INT16, gplc::BTS_INT16, 0x0) });
		pSymTable->AddVariable("z", { nullptr, new gplc::CType(gplc::CT_INT64, gplc::BTS_INT64, 0x0) });

		pTypeDesc = pSymTable->LookUp("x");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->mpType->GetType() == gplc::CT_INT32);

		pTypeDesc = pSymTable->LookUp("y");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->mpType->GetType() == gplc::CT_INT16);

		pTypeDesc = pSymTable->LookUp("z");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->mpType->GetType() == gplc::CT_INT64);

		pTypeDesc = pSymTable->LookUp("unknown");

		REQUIRE(pTypeDesc == nullptr);
	}

	SECTION("Nested scopes test")
	{
		pSymTable->AddVariable("x", { nullptr, new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0) });

		pSymTable->EnterScope();
		pSymTable->AddVariable("x", { nullptr, new gplc::CType(gplc::CT_INT16, gplc::BTS_INT16, 0x0) });
		pSymTable->LeaveScope();

		pSymTable->AddVariable("z", { nullptr, new gplc::CType(gplc::CT_INT64, gplc::BTS_INT64, 0x0) });

		pTypeDesc = pSymTable->LookUp("x");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->mpType->GetType() == gplc::CT_INT32);

		pTypeDesc = pSymTable->LookUp("x");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->mpType->GetType() == gplc::CT_INT32);

		pTypeDesc = pSymTable->LookUp("z");

		REQUIRE(pTypeDesc != nullptr);
		REQUIRE(pTypeDesc->mpType->GetType() == gplc::CT_INT64);

		pTypeDesc = pSymTable->LookUp("unknown");

		REQUIRE(pTypeDesc == nullptr);
	}

	delete pSymTable;
}
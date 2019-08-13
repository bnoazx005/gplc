#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("CSymTable tests")
{
	gplc::ISymTable* pSymTable = new gplc::CSymTable();
	
	auto checkAsserts = [](const gplc::TSymbolDesc* pCurrDesc, gplc::E_COMPILER_TYPES expectedType)
	{
		REQUIRE(pCurrDesc != nullptr);
		REQUIRE(pCurrDesc->mpType->GetType() == expectedType);
	};

	SECTION("Global scope test")
	{
		pSymTable->AddVariable(gplc::TSymbolDesc { "x", nullptr, new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0) });
		pSymTable->AddVariable(gplc::TSymbolDesc { "y", nullptr, new gplc::CType(gplc::CT_INT16, gplc::BTS_INT16, 0x0) });
		pSymTable->AddVariable(gplc::TSymbolDesc { "z", nullptr, new gplc::CType(gplc::CT_INT64, gplc::BTS_INT64, 0x0) });

		checkAsserts(pSymTable->LookUp("x"), gplc::CT_INT32);
		checkAsserts(pSymTable->LookUp("y"), gplc::CT_INT16);
		checkAsserts(pSymTable->LookUp("z"), gplc::CT_INT64);
				
		{
			auto pTypeDesc = pSymTable->LookUp("unknown");

			REQUIRE(!pTypeDesc);
		}
	}

	SECTION("Nested scopes test")
	{
		pSymTable->AddVariable({ "x", nullptr, new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0) });
		pSymTable->AddVariable({ "global", nullptr, new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0) });

		pSymTable->CreateScope();

		pSymTable->AddVariable({ "x", nullptr, new gplc::CType(gplc::CT_INT16, gplc::BTS_INT16, 0x0) });
		checkAsserts(pSymTable->LookUp("x"), gplc::CT_INT16); 
		checkAsserts(pSymTable->LookUp("global"), gplc::CT_INT32);

		pSymTable->LeaveScope();

		pSymTable->AddVariable({ "z", nullptr, new gplc::CType(gplc::CT_INT64, gplc::BTS_INT64, 0x0) });

		checkAsserts(pSymTable->LookUp("x"), gplc::CT_INT32);
		checkAsserts(pSymTable->LookUp("z"), gplc::CT_INT64);

		{
			auto pTypeDesc = pSymTable->LookUp("unknown");

			REQUIRE(!pTypeDesc);
		}
	}

	SECTION("TestEnterNamedScope_ProcessNamedScopes_BuildsCorrectTree")
	{
		pSymTable->AddVariable({ "x", nullptr, new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0) });
		pSymTable->AddVariable({ "global", nullptr, new gplc::CType(gplc::CT_INT32, gplc::BTS_INT32, 0x0) });

		pSymTable->CreateNamedScope("Foo");

		pSymTable->AddVariable({ "x", nullptr, new gplc::CType(gplc::CT_INT16, gplc::BTS_INT16, 0x0) });
		checkAsserts(pSymTable->LookUp("x"), gplc::CT_INT16);
		checkAsserts(pSymTable->LookUp("global"), gplc::CT_INT32);

		pSymTable->LeaveScope();

		pSymTable->AddVariable({ "z", nullptr, new gplc::CType(gplc::CT_INT64, gplc::BTS_INT64, 0x0) });

		checkAsserts(pSymTable->LookUp("x"), gplc::CT_INT32);
		checkAsserts(pSymTable->LookUp("z"), gplc::CT_INT64);
	}

	SECTION("TestLock_LockTableAndTryToInsertVariable_DoesNothing")
	{
		REQUIRE(SUCCESS(pSymTable->Lock()));
		REQUIRE(pSymTable->IsLocked());

		REQUIRE(pSymTable->AddVariable({ "x", }) == InvalidSymbolHandle);
		REQUIRE(!pSymTable->LookUp("x"));

		pSymTable->Unlock();
	}

	SECTION("TestUnlock_UnlockPrevioslyLockedTable_UnlocksTable")
	{
		REQUIRE(SUCCESS(pSymTable->Lock()));
		REQUIRE(pSymTable->IsLocked());
		REQUIRE(SUCCESS(pSymTable->Unlock()));

		REQUIRE(pSymTable->AddVariable({ "x", }) != InvalidSymbolHandle);
		REQUIRE(pSymTable->LookUp("x"));
	}
	
	delete pSymTable;
}
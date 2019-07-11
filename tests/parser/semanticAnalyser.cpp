#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("CSemanticAnalyser's tests")
{
	ISemanticAnalyser* pSemanticAnalyser = new CSemanticAnalyser();

	REQUIRE(pSemanticAnalyser != nullptr);

	SECTION("TestAnalyze_PassNullptr_ReturnsFalse")
	{
		REQUIRE(!pSemanticAnalyser->Analyze(nullptr, new CSymTable()));
		REQUIRE(!pSemanticAnalyser->Analyze(new CASTBlockNode(), nullptr));
		REQUIRE(!pSemanticAnalyser->Analyze(nullptr, nullptr));
	}

	SECTION("TestAnalyze_PassDoubledInitializedVariable_ReturnsFalse")
	{
		auto pIdentifiersList = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));
		pIdentifiersList->AttachChild(new CASTIdentifierNode("x"));

		REQUIRE(!pSemanticAnalyser->Analyze(new CASTDeclarationNode(pIdentifiersList, new CASTNode(NT_INT32)), new CSymTable()));
	}

	delete pSemanticAnalyser;
}
#include <catch2/catch.hpp>
#include <gplc.h>


using namespace gplc;


TEST_CASE("CCCodeGenerator tests")
{
	ICodeGenerator* pCodeGenerator = new CCCodeGenerator();

	auto checkAsserts = [](TLLVMIRData data, const std::string& expectedStr)
	{
		REQUIRE(std::get<std::string>(data) == expectedStr);
	};

	SECTION("TestGenerate_PassEmptyProgram_ReturnsBytecode")
	{
		checkAsserts(pCodeGenerator->Generate(new CASTSourceUnitNode(), new CSymTable()), "");
	}

	SECTION("TestGenerate_PassDeclarations_ReturnsCorrectOutput")
	{
		auto pSymTable = new CSymTable();
		 
		pSymTable->AddVariable("x", { new CIntLiteral(0), new CType(CT_INT64, BTS_INT64, 0x0) });
		pSymTable->AddVariable("y", { new CIntLiteral(0), new CType(CT_INT64, BTS_INT64, 0x0) });
		pSymTable->AddVariable("f", { new CNullLiteral(), new CFunctionType({}, new CType(CT_INT32, BTS_INT32, 0x0), 0x0) });

		CASTSourceUnitNode* pProgram = new CASTSourceUnitNode();

		CASTNode* pIdentifiers = new CASTNode(NT_IDENTIFIERS_LIST);

		pIdentifiers->AttachChild(new CASTIdentifierNode("x"));
		pIdentifiers->AttachChild(new CASTIdentifierNode("y"));

		CASTNode* pFIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pFIdentifier->AttachChild(new CASTIdentifierNode("f"));

		pProgram->AttachChild(new CASTDeclarationNode(pIdentifiers, new CASTTypeNode(NT_INT32)));
		pProgram->AttachChild(new CASTDeclarationNode(pFIdentifier, new CASTFunctionDeclNode(new CASTFunctionClosureNode(), new CASTFunctionArgsNode(), new CASTTypeNode(NT_INT32))));

		checkAsserts(pCodeGenerator->Generate(pProgram, pSymTable), "long x = 0;\nlong y = 0;\nint (*f)() = NULL;\n");
	}
/*
	SECTION("TestGenerate_PassFuncDefinition_ReturnsCorrectOutput")
	{
		auto pSymTable = new CSymTable();

		pSymTable->AddVariable("f", { new CNullLiteral(), new CFunctionType({}, new CType(CT_INT32, BTS_INT32, 0x0), 0x0) });

		CASTSourceUnitNode* pProgram = new CASTSourceUnitNode();
		
		CASTNode* pFIdentifier = new CASTNode(NT_IDENTIFIERS_LIST);

		pFIdentifier->AttachChild(new CASTIdentifierNode("f"));

		CASTFunctionDeclNode* pFuncDecl = new CASTFunctionDeclNode(new CASTFunctionClosureNode(), new CASTFunctionArgsNode(), new CASTTypeNode(NT_INT32));

		CASTDeclarationNode* pDecl = new CASTDeclarationNode(pFIdentifier, pFuncDecl);

		CASTBlockNode* pFuncBody = new CASTBlockNode();

		pProgram->AttachChild(new CASTFuncDefinitionNode(pDecl, pFuncDecl, pFuncBody));

		checkAsserts(pCodeGenerator->Generate(pProgram, pSymTable), "int (*f)() = &lambda;\n");
	}*/

	SECTION("TestGenerate_PassEmptyBlock_ReturnsCorrectOutput")
	{
		auto pSymTable = new CSymTable();
		CASTSourceUnitNode* pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTBlockNode());

		checkAsserts(pCodeGenerator->Generate(pProgram, pSymTable), "{\n}\n");
	}

	SECTION("TestGenerate_PassloopStatement_ReturnsCorrectOutput")
	{
		auto pSymTable = new CSymTable();
		CASTSourceUnitNode* pProgram = new CASTSourceUnitNode();

		pProgram->AttachChild(new CASTLoopStatementNode(new CASTBlockNode()));

		checkAsserts(pCodeGenerator->Generate(pProgram, pSymTable), "while (true)\n{\n}\n");
	}

	delete pCodeGenerator;
}
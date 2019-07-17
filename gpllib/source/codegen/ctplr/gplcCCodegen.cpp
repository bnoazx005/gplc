#include "codegen/ctplr/gplcCCodegen.h"
#include "common/gplcSymTable.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcLiterals.h"
#include "codegen/ctplr/gplcCTypeVisitor.h"
#include "common/gplcTypeSystem.h"
#include "codegen/ctplr/gplcCLiteralVisitor.h"


namespace gplc
{
	TLLVMIRData CCCodeGenerator::Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable)
	{
		if (!pSymTable)
		{
			return {};
		}

		mpSymTable = pSymTable;

		mpTypeVisitor    = new CCTypeVisitor();
		mpLiteralVisitor = new CCLiteralVisitor();

		std::string result { std::get<std::string>(pNode->Accept(this)) };

		delete mpTypeVisitor;
		delete mpLiteralVisitor;

		return result;
	}

	TLLVMIRData CCCodeGenerator::VisitProgramUnit(CASTSourceUnitNode* pProgramNode)
	{
		if (pProgramNode->GetChildrenCount() < 1)
		{
			return "";
		}

		auto pStatements = pProgramNode->GetStatements();

		std::string result {};

		for (auto pCurrStatement : pStatements)
		{
			result.append(std::get<std::string>(pCurrStatement->Accept(this)));
		}

		return result;
	}

	TLLVMIRData CCCodeGenerator::VisitDeclaration(CASTDeclarationNode* pNode)
	{
		auto pIdentifiers = pNode->GetIdentifiers()->GetChildren();

		std::string result;

		const TSymbolDesc* pCurrSymbolDesc = nullptr;

		CType* pType = nullptr;

		for (auto pCurrIdentifier : pIdentifiers)
		{
			pCurrSymbolDesc = mpSymTable->LookUp(dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName());

			pType = pCurrSymbolDesc->mpType;

			result.append(std::get<std::string>(pType->Accept(mpTypeVisitor)))
				  .append(" ")
				  .append(std::get<std::string>(pCurrIdentifier->Accept(this)))
				  .append(" = ")
				  .append(std::get<std::string>(pCurrSymbolDesc->mpValue->Accept(mpLiteralVisitor)))
				  .append(";\n");
		}

		return result;
	}

	TLLVMIRData CCCodeGenerator::VisitIdentifier(CASTIdentifierNode* pNode)
	{
		return pNode->GetName();
	}

	TLLVMIRData CCCodeGenerator::VisitLiteral(CASTLiteralNode* pNode)
	{
		return pNode->GetValue()->Accept(mpLiteralVisitor);
	}

	TLLVMIRData CCCodeGenerator::VisitUnaryExpression(CASTUnaryExpressionNode* pNode)
	{
		return pNode->GetData()->Accept(this);
	}

	TLLVMIRData CCCodeGenerator::VisitBinaryExpression(CASTBinaryExpressionNode* pNode)
	{
		std::string result;

		result.append(std::get<std::string>(pNode->GetLeft()->Accept(this)));
		
		switch (pNode->GetOpType())
		{
			case TT_PLUS:
				result.append(" + ");
				break;
			case TT_MINUS:
				result.append(" - ");
				break;
			case TT_STAR:
				result.append(" * ");
				break;
			case TT_SLASH:
				result.append(" / ");
				break;
			case TT_EQ:
				result.append(" == ");
				break;
			case TT_NE:
				result.append(" != ");
				break;
			case TT_GT:
				result.append(" > ");
				break;
			case TT_GE:
				result.append(" >= ");
				break;
			case TT_LT:
				result.append(" < ");
				break;
			case TT_LE:
				result.append(" <= ");
				break;
			case TT_AND:
				result.append(" && ");
				break;
			case TT_OR:
				result.append(" || ");
				break;
		}

		result.append(std::get<std::string>(pNode->GetRight()->Accept(this)));

		return result;
	}

	TLLVMIRData CCCodeGenerator::VisitAssignment(CASTAssignmentNode* pNode)
	{
		std::string result;

		result.append(std::get<std::string>(pNode->GetLeft()->Accept(this)))
			  .append(" = ")
			  .append(std::get<std::string>(pNode->GetRight()->Accept(this)));

		return result;
	}

	TLLVMIRData CCCodeGenerator::VisitStatementsBlock(CASTBlockNode* pNode)
	{
		return std::string("{\n")
						.append(std::get<std::string>(pNode->Accept(this)))
						.append("}\n");
	}

	TLLVMIRData CCCodeGenerator::VisitIfStatement(CASTIfStatementNode* pNode)
	{
		return std::string("if (")
						.append(std::get<std::string>(pNode->GetCondition()->Accept(this)))
						.append(")\n")
						.append(std::get<std::string>(pNode->GetThenBlock()->Accept(this)))
						.append(std::get<std::string>(pNode->GetElseBlock()->Accept(this)));
	}

	TLLVMIRData CCCodeGenerator::VisitLoopStatement(CASTLoopStatementNode* pNode)
	{
		return std::string("while (true)\n").append(std::get<std::string>(pNode->GetBody()->Accept(this)));
	}

	TLLVMIRData CCCodeGenerator::VisitWhileLoopStatement(CASTWhileLoopStatementNode* pNode)
	{
		return std::string("while (")
						.append(std::get<std::string>(pNode->GetCondition()->Accept(this)))
						.append(")\n")
						.append(std::get<std::string>(pNode->GetBody()->Accept(this)));
	}

	TLLVMIRData CCCodeGenerator::VisitFunctionDeclaration(CASTFunctionDeclNode* pNode)
	{
		return {};
	}

	TLLVMIRData CCCodeGenerator::VisitFunctionClosure(CASTFunctionClosureNode* pNode)
	{
		return {};
	}

	TLLVMIRData CCCodeGenerator::VisitFunctionArgs(CASTFunctionArgsNode* pNode)
	{
		return {};
	}

	TLLVMIRData CCCodeGenerator::VisitFunctionCall(CASTFunctionCallNode* pNode)
	{
		// \todo 
		return std::string(std::get<std::string>(pNode->GetIdentifier()->Accept(this)));
	}

	TLLVMIRData CCCodeGenerator::VisitReturnStatement(CASTReturnStatementNode* pNode)
	{
		return std::string("return ").append(std::get<std::string>(pNode->GetExpr()->Accept(this)));
	}

	TLLVMIRData CCCodeGenerator::VisitDefinitionNode(CASTDefinitionNode* pNode)
	{
		return {};
	}

	TLLVMIRData CCCodeGenerator::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode)
	{
		return {};
	}
}
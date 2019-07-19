#include "codegen/ctplr/gplcCCodegen.h"
#include "common/gplcSymTable.h"
#include "parser/gplcASTNodes.h"
#include "common/gplcLiterals.h"
#include "codegen/ctplr/gplcCTypeVisitor.h"
#include "common/gplcTypeSystem.h"
#include "codegen/ctplr/gplcCLiteralVisitor.h"
#include <cmath>


namespace gplc
{
	TLLVMIRData CCCodeGenerator::Generate(CASTSourceUnitNode* pNode, ISymTable* pSymTable)
	{
		if (!pSymTable)
		{
			return {};
		}

		mpSymTable = pSymTable;

		mGlobalDeclarationsContext = std::string();
		mGlobalDefinitionsContext  = std::string();

		mpTypeVisitor    = new CCTypeVisitor();
		mpLiteralVisitor = new CCLiteralVisitor();
		mpTypeResolver   = new CTypeResolver();

		std::string result { std::get<std::string>(pNode->Accept(this)) };

		delete mpTypeResolver;
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

		return mGlobalDeclarationsContext + mGlobalDefinitionsContext + result;
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

			result.append(std::get<std::string>(pType->Accept(mpTypeVisitor)));

			if (pType->GetType() != CT_FUNCTION)
			{
				result.append(" ").append(std::get<std::string>(pCurrIdentifier->Accept(this)));
			}
			
			// don't need initialization because we built function arguments list
			if ((pNode->GetAttributes() & AV_FUNC_ARG_DECL) != AV_FUNC_ARG_DECL)
			{
				result.append(" = ")
					.append(std::get<std::string>(pCurrSymbolDesc->mpValue->Accept(mpLiteralVisitor)))
					.append(";\n");
			}
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
		std::string result("{\n");

		auto pStatements = pNode->GetStatements();
		
		for (auto pCurrStatement : pStatements)
		{
			result.append(std::get<std::string>(pCurrStatement->Accept(this)));
		}

		return result.append("}\n");
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
		auto pArgs = pNode->GetChildren();

		std::string result{};

		for (auto iter = pArgs.cbegin(); iter != pArgs.cend(); ++iter)
		{
			result.append(std::get<std::string>((*iter)->Accept(this)));

			if (iter + 1 != pArgs.cend())
			{
				result.append(", ");
			}
		}

		return result;
	}

	TLLVMIRData CCCodeGenerator::VisitFunctionCall(CASTFunctionCallNode* pNode)
	{
		// \todo 
		std::string result = std::string("(*")
										.append(std::get<std::string>(pNode->GetIdentifier()->Accept(this)))
										.append(")(");

		auto pArgs = pNode->GetArgs()->GetChildren();

		for (int i = 0; i < pArgs.size(); ++i)
		{
			result.append(std::get<std::string>(pArgs[i]->Accept(this))).append(i < pArgs.size() - 1 ? ", " : "");
		}

		return result.append(")");
	}

	TLLVMIRData CCCodeGenerator::VisitReturnStatement(CASTReturnStatementNode* pNode)
	{
		return std::string("return ").append(std::get<std::string>(pNode->GetExpr()->Accept(this))).append(";\n");
	}

	TLLVMIRData CCCodeGenerator::VisitDefinitionNode(CASTDefinitionNode* pNode)
	{
		auto pIdentifiers = pNode->GetDeclaration()->GetChildren();

		std::string result;

		const TSymbolDesc* pCurrSymbolDesc = nullptr;

		CType* pType = nullptr;

		for (auto pCurrIdentifier : pIdentifiers)
		{
			pCurrSymbolDesc = mpSymTable->LookUp(dynamic_cast<CASTIdentifierNode*>(pCurrIdentifier)->GetName());

			pType = pCurrSymbolDesc->mpType;

			result.append(std::get<std::string>(pType->Accept(mpTypeVisitor)));

			if (pType->GetType() != CT_FUNCTION)
			{
				result.append(" ").append(std::get<std::string>(pCurrIdentifier->Accept(this)));
			}

			result.append(" = ")
				.append(std::get<std::string>(pCurrSymbolDesc->mpValue->Accept(mpLiteralVisitor)))
				.append(";\n");
		}

		return result;
	}

	TLLVMIRData CCCodeGenerator::VisitFunctionDefNode(CASTFuncDefinitionNode* pNode)
	{
		CFunctionType* pLambdaType = dynamic_cast<CFunctionType*>(mpTypeResolver->Resolve(pNode->GetLambdaTypeInfo(), mpSymTable));

		pLambdaType->SetAttributes(AV_STATIC);
		pLambdaType->SetName(_generateAnonymousLambdaName(pLambdaType));

		std::string lambdaDeclaration    = std::get<std::string>(pLambdaType->Accept(mpTypeVisitor));
		std::string lambdaFullDefinition = lambdaDeclaration + "\n" + std::get<std::string>(pNode->GetValue()->Accept(this));

		mGlobalDeclarationsContext.append(lambdaDeclaration).append(";\n");
		mGlobalDefinitionsContext.append(lambdaFullDefinition).append("\n");

		auto pFuncDeclaration = pNode->GetDeclaration();

		auto pFuncIdentifierNode = dynamic_cast<CASTIdentifierNode*>(pFuncDeclaration->GetIdentifiers()->GetChildren()[0]);

		const TSymbolDesc* pFuncDesc = mpSymTable->LookUp(pFuncIdentifierNode->GetName());

		std::string result{ std::get<std::string>(pFuncDesc->mpType->Accept(mpTypeVisitor)).append(" = &").append(pLambdaType->GetName()).append(";\n") };

		if ((pFuncDesc->mpType->GetAttributes() & AV_ENTRY_POINT) == AV_ENTRY_POINT)
		{
			result.append("int main(int argc, char** argv)\n{\n return (*_lang_entry_main)();\n}\n");
		}

		return result;
	}

	std::string CCCodeGenerator::_generateAnonymousLambdaName(const CFunctionType* pLambdaType) const
	{
		std::string name = std::string("lambda").append(pLambdaType->GetReturnValueType()->ToShortAliasString());

		for (auto pCurrArgType : pLambdaType->GetArgsTypes())
		{
			name.append(pCurrArgType->ToShortAliasString());
		}

		// random salt
		return name.append("_").append(std::to_string(rand()));
	}
}
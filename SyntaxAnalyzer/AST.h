#pragma once

#include "SyntaxToken.h"
#include "AstTag.h"
#include <string>

/*
	SyntaxToken ---<> SyntaxNode ---> StatementNode 
						|		\
						|		 \
					   \|/		  \---> ExpressionNode
					   ...
*/


struct SyntaxNode
{
	SyntaxNode(AstTag type) : SyntaxToken(std::string{}, type) {}
	std::string get_value(){ return _value;}
	protected:
		std::string _value; // optional, possible can be use for evaluation
};


struct SimpleStatementNode : SyntaxNode
{
	SimpleStatementNode(SyntaxToken id, BinaryExpressionNode expr) :
		SyntaxNode(AstTag::STMT), _id_child(&id), _expr_child(&expr)
	{
		_assign_child = new SyntaxToken{ ":=", AstTag::ASSIGN_TOKEN };
		_semicolon_child = new SyntaxToken{ ":=", AstTag::SEMICOLON_TOKEN };
	}

	private:
		SyntaxToken* _id_child;
		SyntaxToken* _assign_child;
		BinaryExpressionNode* _expr_child;
		SyntaxToken* _semicolon_child;
};


struct BinaryExpressionNode : SyntaxNode
{
	BinaryExpressionNode(BinaryExpressionNode trans, TernaryExpressionNode add_sub) :
		SyntaxNode(AstTag::BINARY_EXPRESSION), _left_child(trans), _right_child(add_sub) {}

	private:
		SyntaxNode _left_child;
		SyntaxNode _right_child;
};


struct TernaryExpressionNode : SyntaxNode
{
	TernaryExpressionNode(SyntaxToken left, BinaryExpressionNode mid, TernaryExpressionNode right) : 
		SyntaxNode(AstTag::TERNARY_EXPRESSION), _left_child(left), _mid_child(mid), _right_child(right) {}

private:
	SyntaxToken _left_child;
	SyntaxNode _mid_child;
	SyntaxNode _right_child;
};


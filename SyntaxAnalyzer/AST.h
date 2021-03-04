#pragma once

#include "SyntaxToken.h"
#include "AstTag.h"
#include "SyntaxTag.h"
#include <string>
#include <iostream>

/*
	SyntaxToken ---<> SyntaxNode ---> StatementNode 
						|		\
						|		 \
					   \|/		  \---> ExpressionNode
					   ...
*/


struct SyntaxNode
{
	AstTag _node_type;
	std::string _value; // optional, possible can be use for evaluation

	SyntaxNode() : _node_type(AstTag::SYNTAX_TOKEN) {}
	SyntaxNode(const SyntaxNode& node) : _node_type(node._node_type), _value(node._value) {}
	SyntaxNode(AstTag type) : _node_type(type) {}
	std::string get_value() { return _value;}
	std::string node_type_to_string()
	{
		switch (_node_type)
		{
			case AstTag::STMT:
				return "STMT";
				break;
			case AstTag::SYNTAX_TOKEN:
				return "SYNTAX_TOKEN";
				break;
			case AstTag::EXPRESSION:
				return "EXPRESSION";
				break;
			case AstTag::ADD_SUB:
				return "ADD_SUB";
				break;
			case AstTag::FACTOR:
				return "FACTOR";
				break;
			case AstTag::TRANS:
				return "TRANS";
				break;
			case AstTag::MUL_DIV:
				return "MUL_DIV";
				break;
		}
		return "UNKNOWN_AST_TAG";
	}

	virtual int num_of_children()
	{
		return 0;
	}

	virtual SyntaxNode* get_child(int index)
	{
		return new SyntaxNode{};
	}
	
};

struct SyntaxToken : SyntaxNode
{
	std::string lexeme;
	SyntaxTag token_type;

	int line;
	int symbol_pos;

	SyntaxToken() = default;

	SyntaxToken(const SyntaxToken& tok) :
		lexeme(tok.lexeme), token_type(tok.token_type), line(tok.line), symbol_pos(tok.symbol_pos) {}

	SyntaxToken(std::string lex, SyntaxTag lex_type)
		: lexeme(lex), token_type(lex_type), line(), symbol_pos() {}

	SyntaxToken(std::string lex, SyntaxTag lex_type, int f_line, int f_pos)
		: lexeme(lex), token_type(lex_type), line(f_line), symbol_pos(f_pos) {}

	std::string token_type_to_string()
	{
		switch (token_type)
		{
		case SyntaxTag::END_OF_FILE:
			return "END_OF_FILE";
			break;
		case SyntaxTag::ID_TOKEN:
			return "ID_TOKEN";
			break;
		case SyntaxTag::ASSIGN_TOKEN:
			return "ASSIGN_TOKEN";
			break;
		case SyntaxTag::LP_TOKEN:
			return "LP_TOKEN";
			break;
		case SyntaxTag::RP_TOKEN:
			return "RP_TOKEN";
			break;
		case SyntaxTag::PLUS_TOKEN:
			return "PLUS_TOKEN";
			break;
		case SyntaxTag::MINUS_TOKEN:
			return "MINUS_TOKEN";
			break;
		case SyntaxTag::STAR_TOKEN:
			return "STAR_TOKEN";
			break;
		case SyntaxTag::SLASH_TOKEN:
			return "SLASH_TOKEN";
			break;
		case SyntaxTag::FLOAT_NUMBER:
			return "FLOAT_NUMBER";
			break;
		case SyntaxTag::SEMICOLON_TOKEN:
			return "SEMICOLON_TOKEN";
			break;
		}
		return "UNKNOWN_TOKEN";
	}
};



struct SimpleStatementNode : SyntaxNode
{
	SimpleStatementNode() :
		SyntaxNode(AstTag::STMT) {}

	SimpleStatementNode(SyntaxToken& id, SyntaxToken& assign, SyntaxNode& expr, SyntaxToken& semi) :
		SyntaxNode(AstTag::STMT)
	{
		_children.push_back(id);
		_children.push_back(assign);
		_children.push_back(expr);
		_children.push_back(semi);
	}

	~SimpleStatementNode()
	{
		_children.clear();
	}

	void add_child(const SyntaxNode& child)
	{
		_children.push_back(child);
	}

	void add_child(SyntaxNode&& child)
	{
		_children.emplace_back(child);
	}

	SyntaxNode* get_child(int index) override
	{
		return &_children[index];
	}

	int num_of_children() override
	{
		return _children.size();
	}

	private:
		std::vector<SyntaxNode> _children;
};


struct ExpressionNode : SyntaxNode
{
	ExpressionNode() :
		SyntaxNode(AstTag::EXPRESSION) {}

	ExpressionNode(AstTag tag) :
		SyntaxNode(tag) {}

	ExpressionNode(AstTag tag, ExpressionNode& trans, ExpressionNode& add_sub) : SyntaxNode(tag)
	{
		_children.push_back(trans);
		_children.push_back(add_sub);
	}

	~ExpressionNode()
	{
		_children.clear();
	}

	void add_child(const SyntaxNode& child)
	{
		_children.push_back(child);
	}

	void add_child(SyntaxNode&& child)
	{
		_children.push_back(child);
	}

	SyntaxNode* get_child(int index) override
	{
		return &_children[index];
	}
	
	int num_of_children() override
	{
		return _children.size();
	}

	private:
		std::vector<SyntaxNode> _children;
};


struct AstDrawer
{

	void draw_tree(SyntaxNode* node, bool is_root, bool is_last = true, std::string indent = "")
	{
		// just line	|
		// last child	\___
		// mid child	|---
		
		if (!node && is_root)
		{
			std::cout << "[AST DRAWER ERROR] -> root node is nullptr\n";
			return;
		}

		std::string tree = is_last ? "\\___ " : "|---";
		
		std::cout << indent << tree << node->node_type_to_string();

		if (node->_node_type == AstTag::SYNTAX_TOKEN)
		{
			/*SyntaxToken& tok = dynamic_cast<SyntaxToken&>(*node);
			if (&tok)
				std::cout << " <" << &tok.lexeme << ", " << &tok.token_type_to_string() << "> ";*/
		}
		std::cout << std::endl;
		
		indent = is_last ? "    " : "|   ";
		int count_of_child = node->num_of_children();
		for (int curr_child = 0; curr_child < count_of_child; curr_child++)
		{
			is_last = curr_child == node->num_of_children() - 1;
			draw_tree(node->get_child(curr_child), false, is_last, indent);
		}
	}
	
};




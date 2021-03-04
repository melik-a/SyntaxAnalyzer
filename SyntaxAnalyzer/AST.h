#pragma once

#include "SyntaxToken.h"
#include "AstTag.h"

#include <string>
#include <iostream>


struct AstNode
{
	AstTag _node_type;
	std::string _value; // optional, possible can be use for evaluation

	AstNode() : _node_type(AstTag::SYNTAX_TOKEN) {}
	
	AstNode(AstTag tag) : _node_type(tag) {}
	
	AstNode(AstTag tag, AstNode& trans, AstNode& add_sub) : _node_type(tag)
	{
		_children.push_back(trans);
		_children.push_back(add_sub);
	}

	/*AstNode(const AstNode& other) : _node_type(other._node_type), _value(other._value)
	{
		_children = other._children;
	}

	AstNode(AstNode&& other) : _node_type(other._node_type), _value(other._value)
	{
		_children = other._children;
	}*/

	~AstNode()
	{
		_children.clear();
	}

	std::string get_value() 
	{ 
		return _value; 
	}

	void add_child(AstNode* child)
	{
		_children.push_back(*child);
	}

	AstNode get_child(int index)
	{
		return _children[index];
	}
	
	int num_of_children()
	{
		return _children.size();
	}

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

	virtual void print_node()
	{
		std::cout << "<" << node_type_to_string()<<", " << _value << "> ";
	}

	private:
		std::vector<AstNode> _children;
};


struct AstDrawer
{

	void draw_tree(AstNode* node, bool is_root, bool is_last = true, std::string indent = "")
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
		
		std::cout << indent << tree;
		node->print_node();

		if (node->_node_type == AstTag::SYNTAX_TOKEN)
		{
			
		}
		std::cout << std::endl;
		
		indent += is_last ? "    " : "|   ";
		int count_of_child = node->num_of_children();
		for (int curr_child = 0; curr_child < count_of_child; curr_child++)
		{
			is_last = curr_child == node->num_of_children() - 1;
			draw_tree(&(node->get_child(curr_child)), false, is_last, indent);
		}
	}
	
};




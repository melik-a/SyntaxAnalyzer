#pragma once

#include "AstNode.h"

struct AstDrawer
{
	void draw_tree(AstNode* node, bool is_root, bool is_last = true, std::string indent = "")
	{
		// just line	|
		// last child	|___
		// mid child	|---

		if (!node && is_root)
		{
			std::cout << "[AST DRAWER ERROR] -> root node is nullptr\n";
			return;
		}

		std::string tree = is_last ? "|___" : "|---";

		std::cout << indent << tree;
		node->print_node();
		std::cout << std::endl;

		indent += is_last ? "    " : "|    ";

		int count_of_child = node->num_of_children();
		for (int curr_child = 0; curr_child < count_of_child; curr_child++)
		{
			is_last = (curr_child == node->num_of_children() - 1);
			draw_tree(node->get_child(curr_child), false, is_last, indent);
		}
	}
};
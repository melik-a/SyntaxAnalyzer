#include "Parser.h"
#include <fstream>

std::ostream& operator << (std::ostream& os, SyntaxToken tok)
{
	//os << tok.lexeme << "\t" << tok.token_type_to_string();
	os << " <" << tok.lexeme << ", " << tok.token_type_to_string() << "> ";
	return os;
}

bool Parser::get_lexems_from_file(std::string lex_file)
{
	std::ifstream file(lex_file);
	if (!file)
	{
		std::cout << "\nsomething went wrong. invalid file name or path" << std::endl;
		throw "error -> invalid file name or path";
		return false;
	}

	std::string lexeme = "";
	std::string lexeme_type = "";
	SyntaxTag token_tag;

	while (!file.eof())
	{
		std::getline(file, lexeme,'\t');
		std::getline(file, lexeme_type, '\n');
		if (lexeme_type == "ID_TOKEN")
			token_tag = SyntaxTag::ID_TOKEN;
		else if (lexeme_type == "ASSIGN_TOKEN")
			token_tag = SyntaxTag::ASSIGN_TOKEN;
		else if (lexeme_type == "FLOAT_NUMBER")
			token_tag = SyntaxTag::FLOAT_NUMBER;
		else if (lexeme_type == "LP_TOKEN")
			token_tag = SyntaxTag::LP_TOKEN;
		else if (lexeme_type == "RP_TOKEN")
			token_tag = SyntaxTag::RP_TOKEN;
		else if (lexeme_type == "MINUS_TOKEN")
			token_tag = SyntaxTag::MINUS_TOKEN;
		else if (lexeme_type == "PLUS_TOKEN")
			token_tag = SyntaxTag::PLUS_TOKEN;
		else if (lexeme_type == "SEMICOLON_TOKEN")
		{
			token_tag = SyntaxTag::SEMICOLON_TOKEN;
			_lines++;
		}
		else if (lexeme_type == "STAR_TOKEN")
			token_tag = SyntaxTag::STAR_TOKEN;
		else if (lexeme_type == "SLASH_TOKEN")
			token_tag = SyntaxTag::SLASH_TOKEN;
		else if (lexeme_type == "EPS")
			token_tag = SyntaxTag::EPS;
		else
			token_tag = SyntaxTag::UNKNOWN_TOKEN;
		_lexems.push_back(SyntaxToken(lexeme, token_tag));
		//std::cout << lexeme << "\t" << lexeme_type << " \n";
	}
	file.close();
	return true;
}


SyntaxToken Parser::peek_token(int pos)
{
	size_t last = _lexems.size() - 1;
	if (pos <= last)
		return _lexems[pos];
	return _lexems[last];
}

SyntaxToken Parser::current_token()
{
	return _lexems[_current];
}

SyntaxToken Parser::next_token()
{
	return _lexems[++_current];
}

SyntaxToken Parser::lookahead()
{
	int ahead = _current + 1;
	return _lexems[ahead];
}


void Parser::parse()
{
	AstDrawer drawer;
	for (int line = 0; line < _lines; line++)
	{
		_root = new SimpleStatementNode();
		stmt();
		drawer.draw_tree(_root, true);
		delete _root;
	}
}


bool Parser::stmt()
{
	// STMT -> ID := EXPR;
	if (current_token().token_type == SyntaxTag::ID_TOKEN)
	{
		_root->add_child(current_token());
		std::cout << current_token();
		if (next_token().token_type == SyntaxTag::ASSIGN_TOKEN)
		{
			_root->add_child(current_token());
			std::cout << current_token();
			ExpressionNode* expr_node = new ExpressionNode();
			if (expr(expr_node))
			{
				_root->add_child(*expr_node);
				//delete expr_node;

				if (next_token().token_type == SyntaxTag::SEMICOLON_TOKEN)
				{
					_root->add_child(current_token());
					std::cout << current_token() << std::endl;
					next_token();
					return true;
				}
			}
		}
		else
			std::cout << " ERROR -> ASSIGN_TOKEN not found after ID_TOKEN " << std::endl;
	}
	else
	{
		std::cout << "ERROR -> statement doesn't start with an ID_TOKEN " << std::endl;
	}
	return false;
}

bool Parser::expr(ExpressionNode* expr_node)
{
	// EXPR -> TRANS ADD_SUB
	ExpressionNode* trans_node = new ExpressionNode(AstTag::TRANS);
	if (trans(trans_node))
	{
		expr_node->add_child(*trans_node);
		//delete trans_node;

		ExpressionNode* add_sub_node = new ExpressionNode(AstTag::ADD_SUB);
		if (add_sub(add_sub_node))
		{
			expr_node->add_child(*add_sub_node);
			//delete add_sub_node;
		}
		return true;
	}
	return false;
}

bool Parser::add_sub(ExpressionNode* add_sub_node)
{
	// ADD_SUB -> + TRANS ADD_SUB

	SyntaxToken word = lookahead();
	if (word.token_type == SyntaxTag::PLUS_TOKEN || word.token_type == SyntaxTag::MINUS_TOKEN)
	{
		add_sub_node->add_child(current_token());
		std::cout << next_token();
		ExpressionNode* trans_node = new ExpressionNode(AstTag::TRANS);
		if (trans(trans_node))
		{
			add_sub_node->add_child(*trans_node);
			//delete trans_node;

			ExpressionNode* add_sub_node_in = new ExpressionNode(AstTag::ADD_SUB);
			if (add_sub(add_sub_node_in))
			{
				add_sub_node->add_child(*add_sub_node_in);
				//delete add_sub_node_in;
			}
			return true;
		}
	}
	else if (word.token_type == SyntaxTag::EPS)
	{
		std::cout << "ADD_SUB -> EPS" << std::endl;
		return true;
	}
	return false;
}

bool Parser::trans(ExpressionNode* trans_node)
{
	// TRANS -> FACTOR MUL_DIV
	ExpressionNode* factor_node = new ExpressionNode(AstTag::FACTOR);
	if (factor(factor_node))
	{
		trans_node->add_child(*factor_node);
		//delete factor_node;

		ExpressionNode* mul_div_node = new ExpressionNode(AstTag::MUL_DIV);
		if (mul_div(mul_div_node))
		{
			trans_node->add_child(*mul_div_node);
			//delete mul_div_node;
		}
		return true;
	}
	return false;
}

bool Parser::mul_div(ExpressionNode* mul_div_node)
{
	// MUL_DIV -> FACTOR MUL_DIV
	SyntaxToken word = lookahead();
	if (word.token_type == SyntaxTag::STAR_TOKEN || word.token_type == SyntaxTag::SLASH_TOKEN)
	{
		mul_div_node->add_child(current_token());
		std::cout << next_token();
		ExpressionNode* factor_node = new ExpressionNode(AstTag::FACTOR);
		if (factor(factor_node))
		{
			mul_div_node->add_child(*factor_node);
			//delete factor_node;

			ExpressionNode* mul_div_node_in = new ExpressionNode(AstTag::MUL_DIV);
			if (mul_div(mul_div_node_in))
			{
				mul_div_node->add_child(*mul_div_node_in);
				//delete mul_div_node_in;
			}
			return true;
		}
	}
	else if (word.token_type == SyntaxTag::EPS)
	{
		std::cout << "ADD_SUB -> EPS" << std::endl;
		return true;
	}
	return false;
}

bool Parser::factor(ExpressionNode* factor_node)
{
	// FACTOR -> ( EXPR ) | FLOAT_NUM | ID_TOKEN
	if (next_token().token_type == SyntaxTag::LP_TOKEN)
	{
		factor_node->add_child(current_token());
		std::cout << current_token();
		ExpressionNode* expr_node = new ExpressionNode(AstTag::EXPRESSION);
		if (expr(expr_node))
		{
			factor_node->add_child(*expr_node);
			//delete expr_node;

			if (next_token().token_type == SyntaxTag::RP_TOKEN)
			{
				factor_node->add_child(current_token());
				std::cout << current_token();
				return true;
			}
		}
	}
	else if (current_token().token_type == SyntaxTag::FLOAT_NUMBER)
	{
		factor_node->add_child(current_token());
		std::cout << current_token();
		return true;
	}
	else if (current_token().token_type == SyntaxTag::ID_TOKEN)
	{
		factor_node->add_child(current_token());
		std::cout << current_token();
		return true;
	}
	return false;
}



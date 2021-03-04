#include "../include/Parser.h"
#include "../include/AstDrawer.h"
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

	std::cout << "input file:\n";
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
		else
			token_tag = SyntaxTag::UNKNOWN_TOKEN;
		_lexems.push_back(SyntaxToken(lexeme, token_tag));
		std::cout << lexeme << "\t" << lexeme_type << " \n";
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
		_root = new AstNode(AstTag::STMT);
		std::cout << "\nparsed line " << line + 1 << ":\n";
		stmt();
		std::cout << "\nparsed ast " << ":\n";
		drawer.draw_tree(_root, true);
		delete _root;
	}
}


bool Parser::stmt()
{
	// STMT -> ID := EXPR;
	if (current_token().token_type == SyntaxTag::ID_TOKEN)
	{
		_root->add_child(new SyntaxToken(current_token()));
		std::cout << current_token();
		if (next_token().token_type == SyntaxTag::ASSIGN_TOKEN)
		{
			_root->add_child(new SyntaxToken(current_token()));
			std::cout << current_token();
			AstNode* expr_node = new AstNode(AstTag::EXPRESSION);
			if (expr(expr_node))
			{
				_root->add_child(expr_node);
				//delete expr_node;

				if (next_token().token_type == SyntaxTag::SEMICOLON_TOKEN)
				{
					_root->add_child(new SyntaxToken(current_token()));
					std::cout << current_token() << std::endl;
					next_token();
					return true;
				}
			}
		}
		else
			std::cout << " ERROR -> unexpected token " << std::endl;
	}
	else
	{
		// to do: add list of errors for printing after parsing
		std::cout << "ERROR -> unexpected token: expected - ID_TOKEN, got - " << current_token() << std::endl;
	}
	return false;
}

bool Parser::expr(AstNode* expr_node)
{
	// EXPR -> TRANS ADD_SUB
	AstNode* trans_node = new AstNode(AstTag::TRANS);
	if (trans(trans_node))
	{
		expr_node->add_child(trans_node);
		AstNode* add_sub_node = new AstNode(AstTag::ADD_SUB);
		if (add_sub(add_sub_node))
			expr_node->add_child(add_sub_node);
		return true;
	}
	return false;
}

bool Parser::add_sub(AstNode* add_sub_node)
{
	// ADD_SUB -> + TRANS ADD_SUB
	SyntaxToken word = lookahead();
	if (word.token_type == SyntaxTag::PLUS_TOKEN || word.token_type == SyntaxTag::MINUS_TOKEN)
	{
		add_sub_node->add_child(new SyntaxToken(current_token()));
		std::cout << next_token();
		AstNode* trans_node = new AstNode(AstTag::TRANS);
		if (trans(trans_node))
		{
			add_sub_node->add_child(trans_node);
			AstNode* add_sub_node_nested = new AstNode(AstTag::ADD_SUB);
			if (add_sub(add_sub_node_nested))
				add_sub_node->add_child(add_sub_node_nested);
			return true;
		}
	}
	return false;
}

bool Parser::trans(AstNode* trans_node)
{
	// TRANS -> FACTOR MUL_DIV
	AstNode* factor_node = new AstNode(AstTag::FACTOR);
	if (factor(factor_node))
	{
		trans_node->add_child(factor_node);
		AstNode* mul_div_node = new AstNode(AstTag::MUL_DIV);
		if (mul_div(mul_div_node))
			trans_node->add_child(mul_div_node);
		return true;
	}
	return false;
}

bool Parser::mul_div(AstNode* mul_div_node)
{
	// MUL_DIV -> FACTOR MUL_DIV
	SyntaxToken word = lookahead();
	if (word.token_type == SyntaxTag::STAR_TOKEN || word.token_type == SyntaxTag::SLASH_TOKEN)
	{
		mul_div_node->add_child(new SyntaxToken(current_token()));
		std::cout << next_token();
		AstNode* factor_node = new AstNode(AstTag::FACTOR);
		if (factor(factor_node))
		{
			mul_div_node->add_child(factor_node);
			AstNode* mul_div_node_nested = new AstNode(AstTag::MUL_DIV);
			if (mul_div(mul_div_node_nested))
				mul_div_node->add_child(mul_div_node_nested);
			return true;
		}
	}
	return false;
}

bool Parser::factor(AstNode* factor_node)
{
	// FACTOR -> ( EXPR ) | FLOAT_NUM | ID_TOKEN
	if (next_token().token_type == SyntaxTag::LP_TOKEN)
	{
		factor_node->add_child(new SyntaxToken(current_token()));
		std::cout << current_token();
		AstNode* expr_node = new AstNode(AstTag::EXPRESSION);
		if (expr(expr_node))
		{
			factor_node->add_child(expr_node);
			if (next_token().token_type == SyntaxTag::RP_TOKEN)
			{
				factor_node->add_child(new SyntaxToken(current_token()));
				std::cout << current_token();
				return true;
			}
		}
	}
	else if (current_token().token_type == SyntaxTag::FLOAT_NUMBER)
	{
		factor_node->add_child(new SyntaxToken(current_token()));
		std::cout << current_token();
		return true;
	}
	else if (current_token().token_type == SyntaxTag::ID_TOKEN)
	{
		factor_node->add_child(new SyntaxToken(current_token()));
		std::cout << current_token();
		return true;
	}
	return false;
}


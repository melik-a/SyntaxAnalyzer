#pragma once

#include <iostream>
#include <vector>

#include "SyntaxToken.h"
#include "AstNode.h"


/*	STMT - START POINT
	ID - IDENTIFICATOR
	ASSIGN - ASSIGN OPERATOR - :=
	EXPR - COMMON EXPRESSION
	TRANS - TRANSIT EXPRESSION
	ADD_SUB - ADDITION AND SUBTRACTION RIGH-RECURSIVE EXPRESSION
	MUL_DIV - MULTIPLICATION AND DIVISION RIGH-RECURSIVE EXPRESSION
	FACTOR - DEVIDED NON-TERMINAL
	EPS - EPSILON (EMPTY SET)

	
	grammar rules:
	
		STMT	->	ID := EXPR;
		EXPR	->	TRANS ADD_SUB
		ADD_SUB ->	+ TRANS ADD_SUB
				 |	- TRANS ADD_SUB
				 |	EPS
		TRANS	->	FACTOR MUL_DIV
		MUL_DIV ->	* FACTOR MUL_DIV
				 |	/ FACTOR MUL_DIV
				 |	EPS
		FACTOR	->	( EXPR )
				 |	FLOAT_NUM
				 |	ID

	parser type LL(1)
	algorithm - recursive descent

*/


/*	example of syntactically correct sentences:
	
	some_var := 0.9 * 10.5;
	another_one := some_var + 3.0;
	_ten := 10e-2;
	{ fafasfag asfasfa}
	result {fafasfag asfasfa} := (some_var *another_one) / (some_var - _ten) * _ten;
	

*/

/*	parsing expamples:
	
	source code line - some_var := 0.9 * 10.5;
	lexical analyzer output - <ID, some_var> <ASSIGN, := > <FLOAT_NUMBER, 0.9> <ARITHM_OPERATOR, * > <FLOAT_NUMBER, 10.5>
	syntax analyzer output -				
	
	STMT
	|
	|-- ID
	|-- ASSIGN
	|__ EXPR
		|-- TRANS
		|    |-- FACTOR
		|	 |	  |__ FLOAT_NUM
		|	 |__ MUL_DIV
		|		  |__ EPS
		|__ ADD_SUB
			 |-- TRANS
			 |    |-- FACTOR
			 |	  |	   |__ FLOAT_NUM
			 |	  |__ MUL_DIV
			 |		   |__ EPS
			 |__ ADD_SUB
	|__	SEPARATOR						

							
*/


struct Parser
{
	bool get_lexems_from_file(std::string lex_file);
	void parse();

	private:
		AstNode* _root;

		std::vector<SyntaxToken> _lexems;
		
		int _current{};
		int _lines{};

		SyntaxToken peek_token(int pos);
		SyntaxToken current_token();
		SyntaxToken next_token();
		SyntaxToken lookahead();
		

		bool stmt();
		bool expr(AstNode* expr_node);
		bool add_sub(AstNode* add_sub_node);
		bool trans(AstNode* trans_node);
		bool mul_div(AstNode* mul_div_node);
		bool factor(AstNode* factor_node);

};

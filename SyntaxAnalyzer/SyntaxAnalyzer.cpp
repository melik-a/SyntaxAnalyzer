// SyntaxAnalyzer.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "Parser.h"

int main()
{
	Parser parser;
	parser.get_lexems_from_file("demo.txt");
	parser.parse();
	
	system("pause");
	return 0;
}


#ifndef INTERPERTER_HPP
#define INTERPERTER_HPP

#include <stdio.h>
#include <string>

#include "Exception.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ASTtree.hpp"
#include "Constants.hpp"



typedef enum Mode {
	USER_MODE,
	TEST_MODE 
}Mode;

static void greetings();
static void help();
static bool runTests();

class Interpreter {
public:
	Interpreter();

	ASTree* run(char* expr);
	//ASTnode* evalExpression(ASTnode* root, double point);
private:
	char m_command[LINE_LIMIT+1];
	Lexer m_lexer;
	Parser m_parser;
};


#endif
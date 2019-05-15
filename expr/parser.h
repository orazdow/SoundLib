#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "symtable.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <string_view>


class ShuntingYard{
public:

	std::vector<IWORD>& tokens;
	SymTable& symtable;
	Stack<IWORD> operators;
	Queue<IWORD> output;
	Stack<IWORD> eval;
	ShuntingYard(std::vector<IWORD>& tokens, SymTable& symtable);

	void load();
	void printRPN();
	float evaluate();

};

#endif
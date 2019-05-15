#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "types.h"
#include <unordered_map>
#include <string_view>


class SymTable{
public:

	std::unordered_map<std::string_view, IWORD> table;
	float defval = 3000;

	SymTable();
	~SymTable();

	void make_var(char* id, float* val);
	void update_token(IWORD& word);
	IWORD get_var(char* id, int& found);
	
};

#endif 
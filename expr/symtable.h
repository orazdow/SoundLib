#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "types.h"
#include <unordered_map>


class symtable{
public:

	std::unordered_map<const char*, IWORD> table;

	symtable();
	~symtable();

	void make_var(const char* id, float* val);
	void update_token(IWORD& word);
	IWORD get_var(const char* id);
	
};

#endif 
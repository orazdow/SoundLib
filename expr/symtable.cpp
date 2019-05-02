
#include "symtable.h"

SymTable::SymTable(){
	make_var("bepp", &defval);
}

SymTable::~SymTable(){

}

void SymTable::make_var(char* id, float* val){
	printf("adding %s\n", id);
	table[id] =  make_word(val, id);
}

IWORD SymTable::get_var(char* id, int& found){
	auto res = table.find(id);
	if(res == table.end()){
		printf("symbol not found: %s\n", id);
		found = 0;
		return IWORD{0,NUL, 0};
	}else{
		found = 1;
		return res->second;
	}

}
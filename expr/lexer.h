#ifndef LEXER_H
#define LEXER_H

#include "types.h"

#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <string_view>

#define BUFF_LEN (1<<20)
#define VAR_LEN (32)
#define INC(a) a = (a < VAR_LEN-1) ? a+1 : a

struct InputBuff{
	long len;
	char* input;
	char* tokens;
	char* temp;
}; 

void alloc_buff(InputBuff* b, long len);
void realloc_buff(InputBuff* b, long len);
void clear_buff(InputBuff* b);
void free_buff(InputBuff*b);

bool isLetter(char c);
bool isNumber(char c);
bool isSeperator(char c);
bool isOperator(char c);
bool doubleOp(char a, char b);


class Lexer{
public:

	InputBuff buffer;
	char* sourcebuff;
	char* tokenbuff;
	char* temp;
	std::vector<char*> tokens;
	std::vector<IWORD> lexemes;

	enum ScanMode { BEGIN, ALPHATOK, NUMTOK, OPERATOR, DUMP, UNEXPECTED };
	ScanMode mode = ScanMode::BEGIN;
	ScanMode lastMode = ScanMode::BEGIN;

	std::unordered_map<std::string_view, Operator> operatorMap;	

	Lexer();
	~Lexer();

	bool load(const char str[]);
	void printLexemes(bool info = 0);
	bool scan();
	void dump(ScanMode mode);
	IWORD makeLexeme(char* str, Lexer::ScanMode mod, bool dot = false);

};

#endif
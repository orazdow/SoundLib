
#include "lexer.h"

/*************  buffer  ******************/
void alloc_buff(InputBuff* b, long len){
	b->input = (char*)calloc(len, 1);
	b->tokens = (char*)calloc(len, 1);
	b->temp = (char*)calloc(VAR_LEN, 1);
	b->len = len;
}

void realloc_buff(InputBuff* b, long len){
	b->input = (char*)realloc(b->input, len);
	b->tokens = (char*)realloc(b->tokens, len);
	b->len = len;
}

void clear_buff(InputBuff* b){
	memset(b->input, 0, b->len);
	memset(b->tokens, 0, b->len);
	memset(b->temp, 0, VAR_LEN);
}

void free_buff(InputBuff*b){ 
	free(b->input); free(b->tokens); free(b->temp); 
}

/***********  checks  ***************/
bool isLetter(char c){
	return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) || c == '_';
}

bool isNumber(char c){
	return (c >= 48 && c <= 57);
}

bool isSeperator(char c){
	return (c == '(' || c == ')' || c == ';'); // handle ',' '[' ']' '{' '}'
}

bool isOperator(char c){
	switch(c){
		case '-' : return true;
		case '+' : return true;
		case '*' : return true;
		case '/' : return true;
		case '=' : return true;
		case '>' : return true;
		case '<' : return true;
	}
	return false;
}

bool doubleOp(char a, char b){ //handle **, == , ++ -- >= <=
	if(a == '-' && b == '>'){
		return true;
	}
	return false;
}

/***************  Lexer  *****************/
Lexer::Lexer(){
	alloc_buff(&buffer, BUFF_LEN);
}

Lexer::~Lexer(){
	free_buff(&buffer);
}

bool Lexer::load(const char str[]){
	clear_buff(&buffer);
	long in_len = strlen(str);
	if(in_len > buffer.len){
		int factor = ceil(in_len/(float)buffer.len);
		realloc_buff(&buffer, buffer.len*factor);
		printf("increasing buffer %ux to %u\n", factor, buffer.len);
	}
	strcpy(buffer.input, str);
	sourcebuff = buffer.input;
	tokenbuff = buffer.tokens;
	temp = buffer.temp;
	return true;
}

bool Lexer::printLexemes(){
	for(auto p : tokens){
		printf("%s\n", p);
	}
	printf("\n");
	
}

/* tokenize buffer into lexems:  */
bool Lexer::scan(){

	char* p = sourcebuff;
	if(!p) return false;
	long buff_i = 0;
	long len = buffer.len-1;
	mode = ScanMode::BEGIN;
	int i = 0;
	char* tok = tokenbuff;
	bool dot = false;
	bool hold = false;

	while(*p != '\0' || hold){

		char c = *p; 

		switch(mode)
		{

			case ScanMode::BEGIN :
				
				if(c == ' '){ 
					p++;
				}
				else if(isLetter(c) || isNumber(c) || isOperator(c) || isSeperator(c)){ // add '"' string handling
						
					temp[i] = c;
					INC(i);
					p++;

					if(isLetter(c))
						mode = ScanMode::ALPHATOK; 
					else if(isNumber(c))
						mode = ScanMode::NUMTOK; 
					else if(isOperator(c))
						mode = ScanMode::OPERATOR;
					else if(isSeperator(c))
						mode = ScanMode::DUMP;
				}
				else{ mode = ScanMode::UNEXPECTED; }

				if(*p == '\0' && mode == ScanMode::DUMP){ hold = true; }
				
				break;

			case ScanMode::ALPHATOK :
				if(isLetter(c) || isNumber(c)){
					temp[i] = c;
					INC(i);
					p++;
					break;
				}
				else if(isOperator(c) || isSeperator(c) || c == ' '){ 
					mode = ScanMode::DUMP;
					break;
				}
				else{
					mode = ScanMode::UNEXPECTED;
					break;
				}

			break;

			case ScanMode::NUMTOK :
				if(isNumber(c) || (c == '.' && !dot)){
					temp[i] = c;
					INC(i);
					p++;
					if(c == '.'){ dot = true; }
					break;
				}else if(isOperator(c) || isSeperator(c) || c == ' '){
					dot = false;
					mode = ScanMode::DUMP;
					break;
				}
				else{ dot = false; mode = ScanMode::UNEXPECTED; break; }

			break;

			case ScanMode::OPERATOR : 
				if(isLetter(c) || isNumber(c) || isSeperator(c) || c == ' '){
					mode = ScanMode::DUMP;
					break;
				}
				else if(isOperator(c)){
					if(doubleOp(*(p-1), c)){
						temp[i] = c;
						INC(i);
						p++;			
					}
					mode = ScanMode::DUMP;
					break;
				}
				else{
					mode = ScanMode::UNEXPECTED;
					break;
				}
			break;

			case ScanMode::UNEXPECTED :
				printf("UNEXPECTED TOKEN: %c\n", c);
				return false;
			break;

			case ScanMode::DUMP :
				/* merge tokens or check for syntax errors here.. */
				temp[i] = '\0'; 
				strcpy(tok, temp); 
				tokens.push_back(tok);
				tok += (i+1);
				i = 0;

				mode = ScanMode::BEGIN;
				hold = false;

			break;

		}

		if(buff_i++ > len) return false;
	}

	return true;
}


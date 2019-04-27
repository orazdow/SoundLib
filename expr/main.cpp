
#include "expr.h"

int main(){

	std::string str;
	//std::string str(5000, '\0');
	str.insert(0, " var a = (3.5 + b); a->carrierOsc;");
	//printf("%s\n%lu\n", str.c_str(), str.size());

	Lexer lexer;
	lexer.load(str.c_str());

	if(lexer.scan())
        lexer.printLexemes();
    else printf("lexer error/n");



}
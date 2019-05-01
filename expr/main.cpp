
#include "expr.h"

int main(){

	std::string str;
	//std::string str(5000, '\0');

	char a[] = " var a = (3.5 + b); a->carrierOsc;";
	char b[] = " 5 * 6 + 7 ";
	char c[] = "3 + 4.4*(2+5) * 7";

	str.insert(0, c);
	//printf("%s\n%lu\n", str.c_str(), str.size());

	Lexer lexer;
	lexer.load(str.c_str());

	if(lexer.scan())
        lexer.printLexemes();
    else printf("lexer error/n");

    ShuntingYard sh(lexer.lexemes);
    sh.load();
    sh.printRPN();
    printf("evaluating\n");
    float r = sh.evaluate();
    printf("%f\n", r);

}
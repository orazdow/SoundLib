#ifndef EXPR_H
#define EXPR_H

#include "expr/types.h"
#include "expr/lexer.h"
#include "expr/symtable.h"
#include "expr/parser.h"
#include "soundlib.h"


class Expr : public Sig{
public:
  
	char vbuff[num_inlets*4];
	char* vstrings[num_inlets];    
	char* ptr = vbuff;
	// char vf[2] = "f";
	Lexer* lexer;
    SymTable symtable;
    ShuntingYard* sh;


	Expr(std::string str){

		init(num_inlets, true);

		// symtable.make_var(vf, inputs[0]);

		for(int i = 0; i < num_inlets; i++){
		    sprintf(ptr, "f%u", i);
		    vstrings[i] = ptr;  
		    symtable.make_var(vstrings[i], inputs[i]);
		    ptr += 3;
	 	}

	 	lexer = new Lexer(512);
 		lexer->load(str.c_str());
		if(lexer->scan()){
        	// lexer.printLexemes();
		}
    	else printf("lexer error\n");

	    sh = new ShuntingYard(lexer->lexemes, symtable);
	    sh->load();
	    // sh->printRPN();
	}

	void dsp(){
		output = sh->evaluate();
	}

	~Expr(){ delete sh; delete lexer;}

};


#endif
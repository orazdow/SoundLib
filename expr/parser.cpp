
#include "parser.h"


ShuntingYard::ShuntingYard(std::vector<IWORD>& _tokens, SymTable& _symtable) : tokens(_tokens), symtable(_symtable){}

float ShuntingYard::evaluate(){

	IWORD* w = output.peek();

	while(w != nullptr){
		w = output.pop();
		if(!w) break;

		if(w->type == IVAL || w->type == FVAL || w->type == UVAL || w->type == FPTR){
			eval.push(*w);
		}
		else if(w->type == STR){
			// ...
		}
		else if(w->type == OP){ //not handling unary op..
			IWORD* a = eval.pop();
			IWORD* b = eval.pop();
			IWORD r = w->value.op.fpointer(a, b); 
			eval.push(r);
		}
		// for(int i = 0; i < eval.size(); i++){
		// 	printf("%f ", eval.data[i].value.f);
		// } printf("\n");
	}
	output.reset_tail();
	eval.reset();
	return eval.data[0].value.f;
}


void ShuntingYard::printRPN(){

	int len = operators.size();
	for(int i = 0; i < len; i++){
		IWORD w = operators.data[i];
		printf("%s", w.str);
	} 
	if(len){printf(" | "); }
	len = output.length();
	for(int i = 0; i < len; i++){
		IWORD w = output.data[i];
		printf("%s", w.str);
	}   printf("\n");
}

void ShuntingYard::load(){

	for(IWORD& token : tokens){
	
		if(token.type == IVAL || token.type == FVAL || token.type == UVAL || token.type == FPTR){
			output.push(token); 
		}
		else if(token.type == STR){
			// printf("got string: %s\n", token.value.s);
			int found = 0;
			IWORD ret = symtable.get_var(token.value.s, found);
			if(found){
				output.push(ret);
			}else{
				printf("symbol not found: %s\n", token.value.s);
			}
			
		}
		else if(token.type == OP){ 

			if(token.value.op.type == W_OP_LP){
				operators.push(token); 
			}
			else if(token.value.op.type == W_OP_RP){

				IWORD* topp = operators.peek();
				while(topp != nullptr){ 

					if(topp->value.op.type != W_OP_LP){
						output.push(*operators.pop());
					}else{
						operators.pop();
						break;
					}

					topp = operators.peek();
				}   
			}
			else{	

				IWORD* topp = operators.peek();
				if(topp){ 

					Operator top = topp->value.op;

					bool swap = (top.precedence > token.value.op.precedence || 
								(top.precedence == token.value.op.precedence && !top.rightAssoc)) && 
								top.type != W_OP_LP;

					while(swap){
						
						output.push(*operators.pop()); 

						topp = operators.peek();

						if(!topp){ swap = false; }
						else{
							top = topp->value.op;
							swap = (top.precedence > token.value.op.precedence || 
								   (top.precedence == token.value.op.precedence && !top.rightAssoc)) && 
								   top.type != W_OP_LP;

						}
					}

				}
				
				operators.push(token); 
			}

		}

	}

	IWORD* w = nullptr;
	while(w = operators.pop()){
		output.push(*w);
	}

}

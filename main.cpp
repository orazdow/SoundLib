
#include "soundlib.h"
#include "stdio.h"

void printbus(Sig* n, uint inlet);
void printchilds(Sig* n);
void dfs(Sig* n, int lev);

int main(){


	    // printf("HIHIHI\n");
	    // return 0;

 sl_init();

 uint testinlet = 0;
    uint testinlet2 = 2;
    bool a1 = 1;
    bool a2 = 1;


    Sig n1(1);
    Sig n2(2);
    Sig n3(3);
    Sig n4(4);
    Sig n5(5);
    Sig n6(6);

    Sig n(0);


    n1.connect(&n, testinlet);
    n2.connect(&n, testinlet);
    n3.connect(&n, testinlet);

    if(a1)printbus(&n, testinlet); // 1,2,3

    n2.disconnect(&n, testinlet);
    // n3.output += 0.5;
    if(a1)printbus(&n, testinlet); // 1,3

    n2.connect(&n, testinlet); 

    if(a1)printbus(&n, testinlet); // 1,3,2

    n3.connect(&n, testinlet);
    n2.connect(&n, testinlet);
    n1.connect(&n, testinlet);
    n4.connect(&n, testinlet);
    if(a1)printbus(&n, testinlet); // 1,3,2,4

    n2.disconnect(&n, testinlet);
    n1.disconnect(&n, testinlet);
    if(a1)printbus(&n, testinlet); //3,4

    //return 0;
    // n4.disconnect(&n, testinlet);
    //   if(a1)printbus(&n, testinlet);
    // n3.disconnect(&n, testinlet);
    // if(a1)printbus(&n, testinlet);
    // return 0;

    n4.output += 0.14;
    n6.connect(&n, testinlet);
    n2.connect(&n, testinlet);
    n1.connect(&n, testinlet);
    n5.connect(&n, testinlet);
    if(a1)printbus(&n, testinlet); // 3,4,6,2,1,5

    n3.disconnect(&n, testinlet);
    n2.disconnect(&n, testinlet);

    if(a1)printbus(&n, testinlet); //4,6,1,5

    n6.disconnect(&n, testinlet);
    if(a1)printbus(&n, testinlet); // 4,1,5

    n5.disconnect(&n, testinlet);
    n4.disconnect(&n, testinlet);

    if(a1)printbus(&n, testinlet); // 1

    n1.disconnect(&n, testinlet);

    if(a1)printbus(&n, testinlet); // 
    // return 0;
    // ----------------------

    n1.connect(&n, testinlet2);
    n2.connect(&n, testinlet2);
    n3.connect(&n, testinlet2);
    n4.connect(&n, testinlet2);
    n5.connect(&n, testinlet2);

    if(a2)printbus(&n, testinlet2);

    n3.disconnect(&n, testinlet2);
    n4.disconnect(&n, testinlet2);

    if(a2)printbus(&n, testinlet2);

    n1.disconnect(&n, testinlet2);

    if(a2)printbus(&n, testinlet2);

    n4.connect(&n, testinlet2);
    n2.connect(&n, testinlet2);

    if(a2)printbus(&n, testinlet2);

    n1.connect(&n, testinlet2);

    if(a2)printbus(&n, testinlet2);

    n5.disconnect(&n, testinlet2);
    n3.connect(&n, testinlet2);

    if(a2)printbus(&n, testinlet2);

    n4.disconnect(&n, testinlet2);
    n2.disconnect(&n, testinlet2);

    if(a2)printbus(&n, testinlet2);

    n1.disconnect(&n, testinlet2);
    if(a2)printbus(&n, testinlet2);
    n3.disconnect(&n, testinlet2);
    if(a2)printbus(&n, testinlet2);
	return 0;
}

void printbus(Sig* n, uint inlet){ 
    if(auto_summing)
        n->sumInputs();
    printf("%f|", *n->inputs[inlet]);
    uint end = n->input_bus->float_map.addptr[inlet];
    uint n_summing = n->input_bus->n_summing;
    for(int i = 0; i < end; i++){
        printf("%f, ", *(n->input_bus->inputs[i+inlet*n_summing]));
    }
    printf("\n");
}

void printchilds(Sig* n){
    for(int i = 0; i < n->child_map->addptr; i++){
        printf("%u, ", n->child_map->nodes[i]->id);
    } printf("\n");
}

void dfs(Sig* n, int lev){
	printf("\nLev: %u, ID: %u, childs(num:id): ", lev, n->id);
	lev++;
	for(int i = 0; i < n->child_map->addptr; i++){
		if(i > 0){ printf(", "); }
		printf("%u:%u", i, n->child_map->nodes[i]->id);
	}
	for(int i = 0; i < n->child_map->addptr; i++){
		dfs(n->child_map->nodes[i], lev);
	}
}

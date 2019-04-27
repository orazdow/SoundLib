#ifndef TYPES_H
#define TYPES_H

#include <cstdio>
#include <cstdint>

typedef void(*op);

enum OPTYPE{
	W_NULL,
	W_OP_NOP,
	W_OP_ADD,
	W_OP_SUB,
	W_OP_MULT,
	W_OP_DIV,
	W_OP_USUB
};

enum WTYPE{
   NUL, OP, IVAL, UVAL, FVAL, SVAL, IDENT
};

struct Operator{
	uint8_t precedence;
	bool rightAssoc;
	OPTYPE type; 
	op fpointer;
};

union Value{
	float f;
	int i;
	unsigned int ui;
	char* str;
	Operator op;
};

struct IWORD{
	Value value;
	WTYPE type; 
};


template<class T>
class Stack{
public:

	T* data;
	size_t index = -1;
	size_t max;

	Stack(const size_t num){
		data = new T[num];
		max = num;
	}
	Stack() : Stack(512){}
	~Stack(){ delete data; }

	bool push(T el){
		if(index < max-1){
			data[++index] = el;
			return true;
		}else{
			printf("stack full\n");
			return false;
		}
	}

	T pop(){
		if(index >= 0)
			return data[index--];
		else{
			printf("stack empty\n");
			return NULL;
		} 
	}

	T peek(){
		if(index >= 0)
			return data[index];
		else{
			printf("stack empty\n");
			return NULL;
		} 
	}

};

template<class T>
class Queue{
public:

	T* data;
	int tail = 0, head = 0;
	int size;
	Queue(int _size = 512){ size = _size; }

	bool push(T el){
		if((head+1) % size != tail){
			data[head] = el;
			head = (head+1)%size;
			return true;
		}else{
			printf("queue full\n");
			return false;
		}

	}

	T pop(){
		if(tail != head){
			T rtn = data[tail];
			tail = (tail+1)%size;
			return rtn;
		}else{
			printf("queue empty\n");
			return NULL;
		}
	}	

	T peek(){
		if(tail != head){
			return data[tail];
		}else{
			printf("queue empty\n");
			return NULL;
		}		
	}
	
};

#endif
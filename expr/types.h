#ifndef TYPES_H
#define TYPES_H

#include <cstdio>
#include <cstdint>

struct IWORD;

typedef IWORD(*op)(IWORD* a, IWORD* b); // or void(*op)(uint& ip(null), Stack<IWORD>% stack)

enum OPTYPE{
	W_NULL,
	W_OP_NOP,
	W_OP_ADD,
	W_OP_SUB,
	W_OP_MULT,
	W_OP_DIV,
	W_OP_USUB,
	W_OP_LP,
	W_OP_RP
};

enum WTYPE{
   NUL, OP, IVAL, UVAL, FVAL, STR, FPTR, VPTR, IDENT
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
	char* s;
	float* fp;
	void* vp;
	Operator op;
};

struct IWORD{
	Value value;
	WTYPE type; 
	char* str;
};

static IWORD null_word = IWORD{0,NUL,0};

static IWORD OP_ADD(IWORD* a, IWORD* b){
	float _a = (a->type == FPTR) ? *(a->value.fp) : a->value.f;
	float _b = (b->type == FPTR) ? *(b->value.fp) : b->value.f;
	return IWORD{ _a + _b, FVAL, 0 };
}

static IWORD OP_MULT(IWORD* a, IWORD* b){
	float _a = (a->type == FPTR) ? *(a->value.fp) : a->value.f;
	float _b = (b->type == FPTR) ? *(b->value.fp) : b->value.f;
	return IWORD{ _a * _b, FVAL, 0 };
}

static IWORD OP_SUB(IWORD* a, IWORD* b){
	float _a = (a->type == FPTR) ? *(a->value.fp) : a->value.f;
	float _b = (b->type == FPTR) ? *(b->value.fp) : b->value.f;
	return IWORD{ _a - _b, FVAL, 0 };
}

static IWORD OP_DIV(IWORD* a, IWORD* b){
	float _a = (a->type == FPTR) ? *(a->value.fp) : a->value.f;
	float _b = (b->type == FPTR) ? *(b->value.fp) : b->value.f;
	return IWORD{ _a / _b, FVAL, 0 };
}

static IWORD make_word(float* fp, char* str){
	IWORD rtn = IWORD{0, FPTR, str};
	rtn.value.fp = fp;
	return rtn;
}
static IWORD make_word(void* vp, char* str){
	IWORD rtn = IWORD{0, FPTR, str};
	rtn.value.vp = vp;
	return rtn;
}
static IWORD make_word(float f, char* str){
	IWORD rtn = IWORD{0, FPTR, str};
	rtn.value.f = f;
	return rtn;
}
static IWORD make_word(int i, char* str){
	IWORD rtn = IWORD{0, FPTR, str};
	rtn.value.i = i;
	return rtn;
}


template<class T>
class Stack{
public:

	T* data;
	int index = -1;
	int max;

	Stack(const int num){
		data = new T[num]();
		max = num;
	}
	Stack() : Stack(512){}
	~Stack(){ delete data; }

	bool push(T el){
		if(index < max-1){
			data[++index] = el;
			return true;
		}else{
			return false;
		}
	}

	T* pop(){
		if(index >= 0)
			return &data[index--];
		else{
			return nullptr;
		} 
	}

	T* peek(){ 
		if(index >= 0){ 
			return &data[index];
		}
		else{ 
			return nullptr;
		} 
	}

	int size(){
		return index+1;
	}

};

template<class T>
class Queue{
public:

	T* data;
	int tail = 0, head = 0;
	int size;
	Queue(const int _size = 512){ 
		size = _size; 
		data = new T[size]();
	}
	~Queue(){ delete data; }

	bool push(T el){
		if((head+1) % size != tail){
			data[head] = el;
			head = (head+1)%size;
			return true;
		}else{
			return false;
		}
	}

	T* pop(){
		if(tail != head){
			T* rtn = &data[tail];
			tail = (tail+1)%size;
			return rtn;
		}else{
			return nullptr;
		}
	}	

	T* peek(){
		if(tail != head){
			return &data[tail];
		}else{
			return nullptr;
		}		
	}

	void tail_reset(){
		tail = 0;
	}

	int length(){
		if(head > tail){
			return head - tail;
		}else if(tail > head){
			return size - (tail-head);
		}else{
			return 0;
		}
	}
	
};


#endif
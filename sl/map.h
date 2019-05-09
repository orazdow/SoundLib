#ifndef MAP_H
#define MAP_H

#include "soundlib_glob.h"
#include <map>
#include <unordered_map>

/* can replace with unordered_map ..faster itertion for call() not really needed with work queue..*/
template <class T>
struct Node_map{ // template<> Map

	T** nodes;
	uint* keys;
	std::unordered_map<uint, uint> lookup;
	uint addptr = 0;
	uint len = 0;

	bool add(T* n, uint key){
		if(!lookup[key]){
			if(addptr == map_limit) addptr--;
			nodes[addptr] = n; 
			keys[addptr] = key;
			lookup[key] = addptr+1;
			inc_limit(addptr, len); 
			return true;
		}else{
			return false;
		}
    }

	bool remove(uint key){
		if(lookup[key]){
			uint delindex = lookup[key]-1;    
			if(addptr > 0) addptr--;
			for(int i = delindex; i < addptr; i++){
				nodes[i] = nodes[i+1];
				keys[i] = keys[i+1];
				lookup[keys[i]] = i+1;         
			}
			lookup.erase(key);
			return true;
		}else{
			return false;
		}
	}

	Node_map(const uint num){
		nodes = new T*[num];
		len = num;
		keys = new uint[num];
	}

	Node_map() : Node_map(map_limit){}	
	~Node_map(){delete[] keys; delete[] nodes;}

};

struct Float_map{ 

	float** floats;
	uint* addptr; 
	uint* keys; 
	uint n_inputs;
	uint n_summing;
	std::unordered_map<uint,uint[num_inlets]> lookup;

	void add(float* f, uint key, uint inlet){ 
		if(lookup[key][inlet] || addptr[inlet]>n_summing-1) return; 
		if(addptr[inlet] == n_summing) addptr[inlet]--; 
		uint index = inlet*n_summing + addptr[inlet];
		floats[index] = f; 
		keys[index] = key;
		lookup[key][inlet] = index+1; //shift+1, 0 == null
		if(addptr[inlet] < n_summing) addptr[inlet]++;
	}

	void remove(uint key, uint inlet){
		if(!lookup[key][inlet]) return;
		uint delindex = lookup[key][inlet]-1; //shift-1
		if(addptr[inlet]) addptr[inlet]--;
		uint endindex = inlet*n_summing + addptr[inlet];
		for(int i = delindex; i < endindex; i++){
			floats[i] = floats[i+1];
			keys[i] = keys[i+1];
			lookup[keys[i]][inlet] = i+1; //    
		}
		if(endindex < n_inputs*n_summing) floats[endindex] = &_null;     
		lookup[key][inlet] = 0;
	}

	void init(float** inputs, uint _n_inlets, uint _n_summing){
		floats = inputs;
		n_inputs = _n_inlets;
		n_summing = _n_summing;
		addptr = new uint[n_inputs]();
		keys = new uint[n_inputs*n_summing]();
	}
	
	~Float_map(){ delete[] addptr; delete[] keys; }

};


inline void sum(float** inputs, float* output, uint len){ 
	if(len){
		float out = 0; 
		for(int i = 0; i < len; i++){
			out += *inputs[i];
		} 
		*output = out;  //<!
	}
}

inline void mult(float** inputs, float* output, uint len){ 
	if(len){
		float out = 1; 
		for(int i = 0; i < len; i++){
			out *= *inputs[i];
		} 
		*output = out;  //<!
	}
}

struct SigBus{

	float** inputs;
	float* outputs;
	uint n_inlets;
	uint n_summing;
	Float_map float_map;

	void add(float* f, uint key, uint inlet = 0){
		float_map.add(f, key, inlet);
	}

	void remove(uint key, uint inlet = 0){
		float_map.remove(key, inlet);
	}

	// update float ptr without hashing
	void update(float* f, uint inlet, uint sum_offset = 0){
		inputs[inlet*n_summing+sum_offset] = f;
		outputs[inlet] = *inputs[inlet*n_summing+sum_offset];
	}

	void sum(uint inlet = 0){
			::sum(inputs+(inlet*n_summing), &outputs[inlet], float_map.addptr[inlet]);
	}

	void mult(uint inlet = 0){
			::mult(inputs+(inlet*n_summing), &outputs[inlet], float_map.addptr[inlet]);
	}

	void sumInputs(){
		for(int i = 0; i < n_inlets; i++)
				::sum(inputs+(i*n_summing), &outputs[i], float_map.addptr[i]);
	}

	void init(const uint _n_inlets, const uint _n_summing){
		n_inlets = _n_inlets;
		n_summing = _n_summing;
		inputs = new float*[n_inlets*n_summing];
		outputs = new float[n_inlets]();
		for(int i = 0; i < n_inlets*n_summing; i++)
			inputs[i] = &_null;
		float_map.init(inputs, n_inlets, n_summing);
	}

	SigBus(uint _n_inlets, uint _n_summing){ init(_n_inlets, _n_summing); }
	SigBus(){ init(num_inlets, num_summing); }
    ~SigBus(){ delete[] inputs; delete[] outputs;}

}; 

#endif
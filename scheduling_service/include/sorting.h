#pragma once

#include <vector>
using namespace std;

template <class A>
struct sorting
{
	const vector<A> & value_vector;
	const string & operator_sign;

	sorting(const vector<A> & val_vec, const string & op_sign) : value_vector(val_vec), operator_sign(op_sign) {}

	bool operator()(int i1, int i2){
	    if (operator_sign == "asc"){
	        return value_vector[i1] < value_vector[i2];
	    }
	    else if (operator_sign == "dec"){
	        return value_vector[i1] > value_vector[i2];
	    } 
		else{
			return 0;
		}
	}
};

#ifndef CONTAINER__H_
#define CONTAINER__H_
#include<iostream>

template<typename N>
N mod(N a){ return (a>0)? a:-a; }

template<typename N>
class cont{
	N* array=nullptr;
	unsigned long s=0, c=0;
public:
	cont(){}
	cont(N *arr, unsigned long am){
		for(unsigned long i=0;i<am;i++){
			push(arr[i]);
		}
	}
	void push(N data){
		if(s<=c){
			N* temp=new N[(s+1)*2];
			for(unsigned long i=0; i<s; i++) temp[i]=array[i];
			delete [] array; array=temp;s=(s+1)*2;
		}array[c]=data; c++;
	}
	void erase(unsigned long n){
		N* temp=new N[s];
		for(unsigned int i=0, j=0; i<c; i++) {
			if(i==n) continue; 
			temp[j]=array[i]; j++;
		}delete [] array; array=temp; c--;
	}//not optimised
	N& operator[](unsigned long a){
		return array[mod(c+a)%(c)];}
	
	unsigned long size(){ return c; }
	
	N* begin(){ return &array[0]; }
	N* end(){ return &array[c]; }
	
	void clear() { if(s) delete [] array; s=0; c=0; }
	
	bool get(N got){
		for(unsigned int i=0;i<c;i++) if(array[i]==got) return true;
		return false;
	}
	unsigned long getnum(N got){
		for(unsigned int i=0;i<c;i++) if(array[i]==got) return i;
		return -1;
	}
};


#endif
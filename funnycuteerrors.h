#include <iostream>
#include <random>

int randome(int amount){
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0, amount-1);
	return dist(mt);
}

std::string *funny=new std::string[15]{
	" Owo"," 0w o"," oWo"," @w@"," QwQ",
	" XwX","","","","",
	"","","","",""
};


void makeitcute(std::string *arr, int amount){
	for(int i=0; i<amount; i++){
		if(arr[i].find("Error")!=std::string::npos) 
			arr[i].insert(5,funny[randome(15)]);
	}
	/*for(int i=0; i<amount; i++){
		for(unsigned long j=0; j<arr[i].size(); j++){
			arr[i][j]=(arr[i][j]=='r'||arr[i][j]=='l')? 'w':arr[i][j];
		}
	}*/
}


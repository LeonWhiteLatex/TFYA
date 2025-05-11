#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <functional>
#include "container.h"
#include "funnycuteerrors.h"
//states
#define NUL 0
//represents space \t and \n
#define NUM 1
//0 to 9
#define ID 2
//A to Z and a to z
#define EQ 3
//12->10 will set internal trap state which will cause all
//defined (all 5 all 6 and '=' symbols) to act as if they were
//n= (+= -= *= /= &= |= ! =)
#define COMPR 4
//<> <= < > >= =
#define BIN_AR 5
//+ - * / and or
#define UN_AR 6
//not + -
#define LIST 7
//represents coma
#define QUIT 8
//represents semicolon
#define ONPAR 9
#define CNPAR 10
//represents normal paranthesis ()
#define OSPAR 11
#define CSPAR 12
//represents square paranthesis []
#define OCPAR 13
#define CCPAR 14
//represents curvy paranthesis {}
#define TYPE 15
//represents type symbols % ! $
#define OPERATOR 16
//if then else for while do 
//finding keyword its gonna set operator state
#define COLON 17

//periods
#define BNUMS '0' ... '1'
#define ONUMS '0' ... '7'
#define DNUMS '0' ... '9'
#define HNUMS 'A' ... 'F'
#define hNUMS 'a' ... 'f'
#define IDS 'A' ... 'Z'
#define IDs 'a' ... 'z'

void ERROR(int,int);
void WARNING(int,int);

class stcher{
	int state=NUL, pstate=NUL, nstate=NUL, n=18;
	
	std::function<void()>** lambdas = new std::function<void()>*[n];
	
	cont<std::string> lexem;
	public:
	stcher(){
		for(int i=0; i<n; i++)
		lambdas[i]=new std::function<void()>[n];
		for(int i=0; i<n; i++){
			lambdas[NUL][i] = []{ERROR(18,0);};
			lambdas[i][NUL] = []{return;};
		}
	}
	template<typename T>
	void addlam(int p, int n, T&& t){ lambdas[p][n]=t; }
	void restate(int no){
		if(no!=NUL){ 
			pstate=state; state=nstate; nstate=no;
			lambdas[pstate][state]();
		}
	}
	void fixstate(int no){
		if(no!=NUL) nstate=no;
	}
	int current(){ return nstate; } 
	std::string debug(){ return ""+std::to_string(pstate)+" "+std::to_string(state); }
};

int amountoferrors=0;

std::string *errormessages=new std::string[21]{
	"Error: semicolon might be missing at line: ",
	"Error: binary operator expected at line: ",
	"Error: attempt to assign to constant at line: ",
	"Error: an operator expected at line: ",
	"Error: something strange happened at line: ",
	"Error: cannot declare a variable at line: ",
	"Error: colon is missing at line: ",
	"Error: repeatance of binary operators not allowed, line: ",
	"Error: right operand might be missing at line: ",
	"Error: no variable to assign, line: ",
	"Error: sudden coma appearance at line: ",
	"Error: type is used as operand at line: ",
	"Error: list element missing at line: ",
	"Error: left operand is might be missing at line: ",
	"Error: sudden semicolon appearance at line: ",
	"Error: parenthesis incompatible intersections at line: ",
	"Error: not at type, line: ",
	"Error: variables redeclared, line: ",
	"Error: unexpected program entry, line: ",
	"Warning: empty parenthesis, line: ",
	"Warning: empty operator, line: "
},
*operators = new std::string[15]{
	"while", "if", "for", "then", "do", "output", "input", "else", "let", "and","or", "false", "end_else","true", "loop"
},
un_ar = "not";
cont<std::string> ops(operators,15);
stcher statemaster;

int main(int argc, char *argv[]){
	//opers.add(ops); 
	/*for(int iq=0; iq<numbera.size(); iq++) {
		std::cout<<opers.get(binarop[iq])->get()<<"\n";
	}*/
	makeitcute(errormessages, 21);
	std::string lek, coll="";
	bool bin=true, oct=false, dec=false, hex=false,
	init=false, exists=false,//a,b,c:type;
	equal=false/*[let]*/, //eql=false, eqi=false, eqf=false, 
	commentary=false,
	compr=false, comp2=false, comp3=false,
	dot=false, order=false, 
	FORop=false, 
	IFop=false, THENop=false, 
	ELSEop=false, ENDELSE=false,
	WHILEop=false, DOop=false, loopop=false,
	lbin, loct, ldec, lhex, 
	errortick=false, 
	out=false, in=false,
	num=false, id=false, others=false;
	//getline(std::cin, lek);
	char base=0;
	unsigned int line=0;
	int norm=0, squar=0, curvy=0, forsc=0;
	cont<std::string> peremennie;
	std::ifstream read("/home/leon/Documents/TFYA/TFYA/test.txt");
	std::vector<std::string> result;
    while(!read.eof()){
		getline(read,lek);
		//std::cout<<"n\n";
		result.push_back(lek);
	}lek="";
	std::string **reserve=new std::string*[result.size()];
	
	/*statemaster's list of restating*/{
		//number (123, 12.32, 12.322e+33)
		statemaster.addlam(NUL,OCPAR, [&]{});
		statemaster.addlam(NUM, NUM, [&]{if(!out) { ERROR(0,line); ERROR(1,line); }}); //15 13 X
		statemaster.addlam(NUM, ID, [&]{if(!out) {ERROR(0,line); ERROR(1,line);}}); //13 a X
		statemaster.addlam(NUM, EQ, [&]{if(equal) ERROR(9,line);equal=false;}); //11= ([let] 13= X) V
		statemaster.addlam(NUM, COMPR, [&]{if(equal) ERROR(9,line);equal=false;}); //20> ([let] 20> X) V
		statemaster.addlam(NUM, BIN_AR, [&]{if(equal) ERROR(9,line);equal=false;}); //12+ 12 or ([let] 5+ X) V
		statemaster.addlam(NUM, UN_AR, [&]{ERROR(0,line);ERROR(1,line);}); //5 not X
		statemaster.addlam(NUM, LIST, [&]{if(!out) ERROR(2,line);});//write( |5,| V
		statemaster.addlam(NUM, QUIT, [&]{});//5; V
		statemaster.addlam(NUM, ONPAR, [&]{if(!out){ERROR(0,line); ERROR(1,line);}});//5( X
		statemaster.addlam(NUM, CNPAR, [&]{});//5) V
		statemaster.addlam(NUM, OSPAR, [&]{ERROR(0,line);});//5[ X
		statemaster.addlam(NUM, CSPAR, [&]{ERROR(3,line);});//5] X
		statemaster.addlam(NUM, OCPAR, [&]{ERROR(0,line);});//5{ X
		statemaster.addlam(NUM, CCPAR, [&]{ERROR(0,line);});//5} X
		statemaster.addlam(NUM, TYPE, [&]{if(!(ENDELSE||loopop)){ERROR(6,line);ERROR(5,line);}loopop=false;});//5$ X
		statemaster.addlam(NUM, OPERATOR, [&]{if(!(THENop||ENDELSE))ERROR(0,line);});//5 if X
		statemaster.addlam(NUM, COLON, [&]{ERROR(5,line);});//5: X
		//words (aboba, for, bebra)
		statemaster.addlam(ID, NUM, [&]{if(!out){ERROR(0,line);ERROR(1,line);}});//ab 5 X
		statemaster.addlam(ID, ID, [&]{if(!(out||in)){ERROR(0,line);ERROR(1,line);}});//ab bd X
		statemaster.addlam(ID, EQ, [&]{equal=false;});//as= V
		statemaster.addlam(ID, COMPR, [&]{if(equal) ERROR(9,line); equal=false;});//as> V
		statemaster.addlam(ID, BIN_AR, [&]{if(equal) ERROR(9,line); equal=false;});//as+ V
		statemaster.addlam(ID, UN_AR, [&]{if(!out){ERROR(0,line);ERROR(1,line);}});//as not X
		statemaster.addlam(ID, LIST, [&]{if(!(init||out||in)) ERROR(10, line); if(init&&exists) ERROR(17,line);});//as, V
		statemaster.addlam(ID, QUIT, [&]{});//ad; V
		statemaster.addlam(ID, ONPAR, [&]{if(!out){ERROR(0,line);ERROR(1,line);}});//ad( X
		statemaster.addlam(ID, CNPAR, [&]{});//ad) V
		statemaster.addlam(ID, OSPAR, [&]{if(!(WHILEop||THENop||ELSEop)) ERROR(0,line);});//ad[ X
		statemaster.addlam(ID, CSPAR, [&]{ERROR(3,line);});//ad] X
		statemaster.addlam(ID, OCPAR, [&]{if(!WHILEop) ERROR(0,line);});//ad{ X
		statemaster.addlam(ID, CCPAR, [&]{ERROR(0,line);});//ad} X
		statemaster.addlam(ID, TYPE, [&]{if(!(ENDELSE||loopop)) ERROR(6,line);loopop=false;});//ad % X
		statemaster.addlam(ID, OPERATOR, [&]{if(!THENop&&!ELSEop&&!ENDELSE) ERROR(0,line);});//ad if X
		statemaster.addlam(ID, COLON, [&]{if(exists) ERROR(17,line);});//ad: V
		//equality (=)
		statemaster.addlam(EQ, NUM, [&]{});//=5 V
		statemaster.addlam(EQ, ID, [&]{});//=a V
		statemaster.addlam(EQ, EQ, [&]{ERROR(7,line);});//= =X
		statemaster.addlam(EQ, COMPR, [&]{ERROR(7,line);});//=> X
		statemaster.addlam(EQ, BIN_AR, [&]{ERROR(7,line);});//=- X
		statemaster.addlam(EQ, UN_AR, [&]{});//= not V
		statemaster.addlam(EQ, LIST, [&]{if(in||out) ERROR(8,line); else if(init){ERROR(5,line);}else{ERROR(4,line);}}); //=, X
		statemaster.addlam(EQ, QUIT, [&]{ERROR(8,line);}); //=; X
		statemaster.addlam(EQ, ONPAR, [&]{}); //=( V
		statemaster.addlam(EQ, CNPAR, [&]{ERROR(8,line);});//=) X
		statemaster.addlam(EQ, OSPAR, [&]{ERROR(8,line);ERROR(0,line);});//=[ X
		statemaster.addlam(EQ, CSPAR, [&]{ERROR(3,line);});//=] X
		statemaster.addlam(EQ, OCPAR, [&]{ERROR(8,line); ERROR(0,line);});// ={ X
		statemaster.addlam(EQ, CCPAR, [&]{ERROR(8,line); ERROR(0,line);});// =} X
		statemaster.addlam(EQ, TYPE, [&]{ERROR(11,line);}); //= % X
		statemaster.addlam(EQ, OPERATOR, [&]{ERROR(8,line); ERROR(0,line);});//= if X
		statemaster.addlam(EQ, COLON, [&]{ERROR(5,line);});//=: X
		//compare (<>, <=, >=, <, >) (>< X)
		statemaster.addlam(COMPR, NUM, [&]{}); //<5
		statemaster.addlam(COMPR, ID, [&]{}); //<asd
		statemaster.addlam(COMPR, EQ, [&]{ERROR(7,line);});  //<==
		statemaster.addlam(COMPR, COMPR, [&]{ERROR(7,line);}); //<<
		statemaster.addlam(COMPR, BIN_AR, [&]{ERROR(7,line);}); //<+
		statemaster.addlam(COMPR, UN_AR, [&]{}); //<not
		statemaster.addlam(COMPR, LIST, [&]{if(in||out) ERROR(8,line); else if(init){ERROR(5,line);}else{ERROR(4,line);}}); //<,
		statemaster.addlam(COMPR, QUIT, [&]{ERROR(8,line);}); //<;
		statemaster.addlam(COMPR, ONPAR, [&]{}); //<(
		statemaster.addlam(COMPR, CNPAR, [&]{ERROR(8,line);}); //<)
		statemaster.addlam(COMPR, OSPAR, [&]{ERROR(8,line);ERROR(0,line);}); //<[
		statemaster.addlam(COMPR, CSPAR, [&]{ERROR(3,line);}); //<]
		statemaster.addlam(COMPR, OCPAR, [&]{ERROR(8,line); ERROR(0,line);}); //<{
		statemaster.addlam(COMPR, CCPAR, [&]{ERROR(8,line); ERROR(0,line);}); //<}
		statemaster.addlam(COMPR, TYPE, [&]{ERROR(11,line);}); //<$
		statemaster.addlam(COMPR, OPERATOR, [&]{ERROR(8,line);ERROR(0,line);}); //<if
		statemaster.addlam(COMPR, COLON, [&]{ERROR(5,line);});//<:
		//arithmetics (+, -, *, /, and, or)
		statemaster.addlam(BIN_AR, NUM, [&]{});//*54
		statemaster.addlam(BIN_AR, ID, [&]{});//*nane
		statemaster.addlam(BIN_AR, EQ, [&]{ERROR(7,line);});//*=
		statemaster.addlam(BIN_AR, COMPR, [&]{ERROR(7,line);});//*<
		statemaster.addlam(BIN_AR, BIN_AR, [&]{ERROR(7,line);/*exceptions*/});//**
		statemaster.addlam(BIN_AR, UN_AR, [&]{});//*not
		statemaster.addlam(BIN_AR, LIST, [&]{if(in||out) ERROR(8,line); else if(init){ERROR(5,line);}else{ERROR(4,line);}});//*,
		statemaster.addlam(BIN_AR, QUIT, [&]{ERROR(8,line);});//*;
		statemaster.addlam(BIN_AR, ONPAR, [&]{});//*(
		statemaster.addlam(BIN_AR, CNPAR, [&]{ERROR(8,line);});//*)
		statemaster.addlam(BIN_AR, OSPAR, [&]{ERROR(8,line); ERROR(0,line);});//*[
		statemaster.addlam(BIN_AR, CSPAR, [&]{ERROR(3,line);});//*]
		statemaster.addlam(BIN_AR, OCPAR, [&]{ERROR(8,line); ERROR(0,line);});//*{
		statemaster.addlam(BIN_AR, CCPAR, [&]{ERROR(8,line); ERROR(0,line);});//*}
		statemaster.addlam(BIN_AR, TYPE, [&]{ERROR(11,line);});//*!
		statemaster.addlam(BIN_AR, OPERATOR, [&]{ERROR(8,line);ERROR(0,line);});//*for
		statemaster.addlam(BIN_AR, COLON, [&]{ERROR(5,line);});//+:
		//unary (not)
		statemaster.addlam(UN_AR, NUM, [&]{});//not 5
		statemaster.addlam(UN_AR, ID, [&]{if(equal) ERROR(9,line);});//not noe
		statemaster.addlam(UN_AR, EQ, [&]{ERROR(7,line);});//not =
		statemaster.addlam(UN_AR, COMPR, [&]{ERROR(7,line);});//not >
		statemaster.addlam(UN_AR, BIN_AR, [&]{ERROR(7,line);});//not =
		statemaster.addlam(UN_AR, UN_AR, [&]{});//not not
		statemaster.addlam(UN_AR, LIST, [&]{if(in||out) ERROR(8,line); else if(init){ERROR(5,line);}else{ERROR(4,line);}});//not ,
		statemaster.addlam(UN_AR, QUIT, [&]{ERROR(8,line);});//not ;
		statemaster.addlam(UN_AR, ONPAR, [&]{});//not (
		statemaster.addlam(UN_AR, CNPAR, [&]{ERROR(8,line);});//not )
		statemaster.addlam(UN_AR, OSPAR, [&]{ERROR(8,line); ERROR(0,line);});//not [
		statemaster.addlam(UN_AR, CSPAR, [&]{ERROR(3,line);});//not ]
		statemaster.addlam(UN_AR, OCPAR, [&]{ERROR(8,line); ERROR(0,line);});//not {
		statemaster.addlam(UN_AR, CCPAR, [&]{ERROR(8,line); ERROR(0,line);});//not }
		statemaster.addlam(UN_AR, TYPE, [&]{ERROR(11,line);});//not $
		statemaster.addlam(UN_AR, OPERATOR, [&]{ERROR(8,line);ERROR(0,line);});//not for
		statemaster.addlam(UN_AR, COLON, [&]{ERROR(5,line);});//not :
		//coma (,)
		statemaster.addlam(LIST, NUM, [&]{ERROR(2,line);});//,45
		statemaster.addlam(LIST, ID, [&]{if(!(init)) ERROR(10, line);});//,hello
		statemaster.addlam(LIST, EQ, [&]{ERROR(5,line); ERROR(13,line);});//,=
		statemaster.addlam(LIST, COMPR, [&]{ERROR(5,line); ERROR(13,line);});//,>
		statemaster.addlam(LIST, BIN_AR, [&]{ERROR(5,line); ERROR(13,line);});//,-
		statemaster.addlam(LIST, UN_AR, [&]{ERROR(10,line);});//,not
		statemaster.addlam(LIST, LIST, [&]{ERROR(12,line);});//,,
		statemaster.addlam(LIST, QUIT, [&]{ERROR(12,line);});//,;
		statemaster.addlam(LIST, ONPAR, [&]{if(init) ERROR(5,line); else ERROR(10,line);});//,(
		statemaster.addlam(LIST, CNPAR, [&]{WARNING(12,line);});//,)
		statemaster.addlam(LIST, OSPAR, [&]{ERROR(12,line); ERROR(0,line);});//,[
		statemaster.addlam(LIST, CSPAR, [&]{ERROR(3,line);});//,]
		statemaster.addlam(LIST, OCPAR, [&]{ERROR(12,line);ERROR(0,line);});//,{
		statemaster.addlam(LIST, CCPAR, [&]{ERROR(12,line); ERROR(0,line);});//,}
		statemaster.addlam(LIST, TYPE, [&]{ERROR(12,line); ERROR(6,line);});//,$
		statemaster.addlam(LIST, OPERATOR, [&]{ERROR(10,line);});//,while
		statemaster.addlam(LIST, COLON, [&]{ERROR(12,line);});//,:
		//semicolon (;)
		statemaster.addlam(QUIT, NUM, [&]{});//; 55
		statemaster.addlam(QUIT, ID, [&]{});//; gfg
		statemaster.addlam(QUIT, EQ, [&]{ERROR(13,line);});//; =
		statemaster.addlam(QUIT, COMPR, [&]{ERROR(13,line);});//;>
		statemaster.addlam(QUIT, BIN_AR, [&]{ERROR(13,line);});//;+
		statemaster.addlam(QUIT, UN_AR, [&]{});//;not
		statemaster.addlam(QUIT, LIST, [&]{ERROR(12,line);});//;,
		statemaster.addlam(QUIT, QUIT, [&]{});//;;
		statemaster.addlam(QUIT, ONPAR, [&]{});//;(
		statemaster.addlam(QUIT, CNPAR, [&]{ERROR(4,line);});//;)
		statemaster.addlam(QUIT, OSPAR, [&]{});//;[
		statemaster.addlam(QUIT, CSPAR, [&]{ERROR(3,line);});//;]
		statemaster.addlam(QUIT, OCPAR, [&]{});//;{
		statemaster.addlam(QUIT, CCPAR, [&]{});//;}
		statemaster.addlam(QUIT, TYPE, [&]{ERROR(6,line);});//;$
		statemaster.addlam(QUIT, OPERATOR, [&]{});//; if
		statemaster.addlam(QUIT, COLON, [&]{ERROR(12,line);ERROR(14,line);});//; :
		//opening parenthesis ( ( )
		statemaster.addlam(ONPAR, NUM, [&]{});
		statemaster.addlam(ONPAR, ID, [&]{});
		statemaster.addlam(ONPAR, EQ, [&]{ERROR(13,line);});//(=
		statemaster.addlam(ONPAR, COMPR, [&]{ERROR(13,line);});
		statemaster.addlam(ONPAR, BIN_AR, [&]{ERROR(13,line);});
		statemaster.addlam(ONPAR, UN_AR, [&]{});
		statemaster.addlam(ONPAR, LIST, [&]{if(in||out) ERROR(12,line); else ERROR(10,line);});//(,
		statemaster.addlam(ONPAR, QUIT, [&]{ERROR(14,line);});
		statemaster.addlam(ONPAR, ONPAR, [&]{});
		statemaster.addlam(ONPAR, CNPAR, [&]{WARNING(0,line);});
		statemaster.addlam(ONPAR, OSPAR, [&]{ERROR(4,line);});
		statemaster.addlam(ONPAR, CSPAR, [&]{ERROR(15,line);});
		statemaster.addlam(ONPAR, OCPAR, [&]{ERROR(15,line);});
		statemaster.addlam(ONPAR, CCPAR, [&]{ERROR(15,line);});
		statemaster.addlam(ONPAR, TYPE, [&]{ERROR(11,line);});
		statemaster.addlam(ONPAR, OPERATOR, [&]{if(!out) ERROR(14,line);});
		statemaster.addlam(ONPAR, COLON, [&]{ERROR(15,line);});//
		//closing parenthesis ( ) )
		statemaster.addlam(CNPAR, NUM, [&]{if(!(out||THENop||ENDELSE)){ERROR(1,line);ERROR(0,line);}});
		statemaster.addlam(CNPAR, ID, [&]{if(!(out||THENop||ENDELSE)){ERROR(1,line);ERROR(0,line);}});
		statemaster.addlam(CNPAR, EQ, [&]{ERROR(13,line);});
		statemaster.addlam(CNPAR, COMPR, [&]{});//)<
		statemaster.addlam(CNPAR, BIN_AR, [&]{});
		statemaster.addlam(CNPAR, UN_AR, [&]{if(!out){ERROR(1,line);ERROR(0,line);}});
		statemaster.addlam(CNPAR, LIST, [&]{ERROR(0,line); ERROR(12,line);});
		statemaster.addlam(CNPAR, QUIT, [&]{});
		statemaster.addlam(CNPAR, ONPAR, [&]{if(!out){ERROR(1,line);ERROR(0,line);}});
		statemaster.addlam(CNPAR, CNPAR, [&]{});
		statemaster.addlam(CNPAR, OSPAR, [&]{if(!WHILEop) ERROR(0,line);});
		statemaster.addlam(CNPAR, CSPAR, [&]{ERROR(3,line);});
		statemaster.addlam(CNPAR, OCPAR, [&]{if(!(WHILEop||FORop))ERROR(0,line);else{if(FORop&&forsc!=2)ERROR(12,line); FORop=false;}});
		statemaster.addlam(CNPAR, CCPAR, [&]{ERROR(0,line);});
		statemaster.addlam(CNPAR, TYPE, [&]{if(!ENDELSE) ERROR(5,line); ENDELSE=false;});
		statemaster.addlam(CNPAR, OPERATOR, [&]{if(!(THENop||ENDELSE||ELSEop||IFop)) {ERROR(0,line);}});
		statemaster.addlam(CNPAR, COLON, [&]{ERROR(5,line);});//
		//opening square parenthesis ([) //
		statemaster.addlam(OSPAR, NUM, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, ID, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, EQ, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, COMPR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, BIN_AR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, UN_AR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, LIST, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, QUIT, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, ONPAR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, CNPAR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, OSPAR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, CSPAR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, OCPAR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, CCPAR, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, TYPE, [&]{ERROR(3,line);});
		statemaster.addlam(OSPAR, OPERATOR, [&]{});
		statemaster.addlam(OSPAR, COLON, [&]{ERROR(3,line);});//
		//closing square parenthesis (])
		statemaster.addlam(CSPAR, NUM, [&]{ERROR(1,line);ERROR(9,line);ERROR(2,line);});
		statemaster.addlam(CSPAR, ID, [&]{});
		statemaster.addlam(CSPAR, EQ, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, COMPR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, BIN_AR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, UN_AR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, LIST, [&]{ERROR(9,line); ERROR(10,line);});
		statemaster.addlam(CSPAR, QUIT, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, ONPAR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, CNPAR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, OSPAR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, CSPAR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, OCPAR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, CCPAR, [&]{ERROR(0,line);});
		statemaster.addlam(CSPAR, TYPE, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, OPERATOR, [&]{ERROR(9,line);});
		statemaster.addlam(CSPAR, COLON, [&]{ERROR(9,line);});//
		//opening curvy parenthesis ({)
		statemaster.addlam(OCPAR, NUM, [&]{});
		statemaster.addlam(OCPAR, ID, [&]{});
		statemaster.addlam(OCPAR, EQ, [&]{ERROR(13,line);});
		statemaster.addlam(OCPAR, COMPR, [&]{ERROR(13,line);});
		statemaster.addlam(OCPAR, BIN_AR, [&]{ERROR(13,line);});
		statemaster.addlam(OCPAR, UN_AR, [&]{});
		statemaster.addlam(OCPAR, LIST, [&]{ERROR(12,line);});
		statemaster.addlam(OCPAR, QUIT, [&]{WARNING(2,line);});
		statemaster.addlam(OCPAR, ONPAR, [&]{});
		statemaster.addlam(OCPAR, CNPAR, [&]{ERROR(15,line);});
		statemaster.addlam(OCPAR, OSPAR, [&]{});
		statemaster.addlam(OCPAR, CSPAR, [&]{ERROR(15,line); ERROR(3,line);});
		statemaster.addlam(OCPAR, OCPAR, [&]{});
		statemaster.addlam(OCPAR, CCPAR, [&]{WARNING(0,line);});
		statemaster.addlam(OCPAR, TYPE, [&]{});
		statemaster.addlam(OCPAR, OPERATOR, [&]{});
		statemaster.addlam(OCPAR, COLON, [&]{ERROR(12,line);});//
		//closing curvy parenthesis (})
		statemaster.addlam(CCPAR, NUM, [&]{});
		statemaster.addlam(CCPAR, ID, [&]{});
		statemaster.addlam(CCPAR, EQ, [&]{ERROR(13,line);});
		statemaster.addlam(CCPAR, COMPR, [&]{ERROR(13,line);});
		statemaster.addlam(CCPAR, BIN_AR, [&]{ERROR(13,line);});
		statemaster.addlam(CCPAR, UN_AR, [&]{});
		statemaster.addlam(CCPAR, LIST, [&]{ERROR(12,line);});
		statemaster.addlam(CCPAR, QUIT, [&]{});
		statemaster.addlam(CCPAR, ONPAR, [&]{});
		statemaster.addlam(CCPAR, CNPAR, [&]{ERROR(15,line);});
		statemaster.addlam(CCPAR, OSPAR, [&]{});
		statemaster.addlam(CCPAR, CSPAR, [&]{ERROR(3,line); ERROR(15,line);});
		statemaster.addlam(CCPAR, OCPAR, [&]{});
		statemaster.addlam(CCPAR, CCPAR, [&]{});
		statemaster.addlam(CCPAR, TYPE, [&]{});
		statemaster.addlam(CCPAR, OPERATOR, [&]{});
		statemaster.addlam(CCPAR, COLON, [&]{ERROR(12,line);});//
		//types of variables and functions (%,!,$) //
		statemaster.addlam(TYPE, NUM, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, ID, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, EQ, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, COMPR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, BIN_AR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, UN_AR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, LIST, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, QUIT, [&]{});
		statemaster.addlam(TYPE, ONPAR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, CNPAR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, OSPAR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, CSPAR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, OCPAR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, CCPAR, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, TYPE, [&]{ERROR(0,line);});
		statemaster.addlam(TYPE, OPERATOR, [&]{if(!(THENop||IFop||WHILEop||ELSEop||ENDELSE)) ERROR(0,line);});
		statemaster.addlam(TYPE, COLON, [&]{ERROR(0,line);});//
		//operators (for, do, while, let, if, else, then...)
		statemaster.addlam(OPERATOR, NUM, [&]{});
		statemaster.addlam(OPERATOR, ID, [&]{});
		statemaster.addlam(OPERATOR, EQ, [&]{ERROR(13,line);});
		statemaster.addlam(OPERATOR, COMPR, [&]{ERROR(13,line);});
		statemaster.addlam(OPERATOR, BIN_AR, [&]{ERROR(13,line);});
		statemaster.addlam(OPERATOR, UN_AR, [&]{});
		statemaster.addlam(OPERATOR, LIST, [&]{ERROR(10,line);});
		statemaster.addlam(OPERATOR, QUIT, [&]{ERROR(4,line);});
		statemaster.addlam(OPERATOR, ONPAR, [&]{});
		statemaster.addlam(OPERATOR, CNPAR, [&]{ERROR(4,line);});
		statemaster.addlam(OPERATOR, OSPAR, [&]{if(!(THENop||ELSEop||ENDELSE)) ERROR(4,line);});
		statemaster.addlam(OPERATOR, CSPAR, [&]{if(!equal) ERROR(3,line);});
		statemaster.addlam(OPERATOR, OCPAR, [&]{});
		statemaster.addlam(OPERATOR, CCPAR, [&]{ERROR(8,line);ERROR(0,line);});
		statemaster.addlam(OPERATOR, TYPE, [&]{ERROR(8,line);ERROR(0,line);ERROR(6,line);});
		statemaster.addlam(OPERATOR, OPERATOR, [&]{});
		statemaster.addlam(OPERATOR, COLON, [&]{ERROR(5,line);});//
		
		//a colon (:)
		statemaster.addlam(COLON, NUM, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, ID, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, EQ, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, COMPR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, BIN_AR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, UN_AR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, LIST, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, QUIT, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, ONPAR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, CNPAR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, OSPAR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, CSPAR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, OCPAR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, CCPAR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, TYPE, [&]{});//
		statemaster.addlam(COLON, OPERATOR, [&]{ERROR(16,line);});//
		statemaster.addlam(COLON, COLON, [&]{ERROR(16,line);});//
		
		
	}
	//////////////lexic analizer//////////////
	unsigned int curvelevel=0;
	for(unsigned int Q=0; Q<result.size(); Q++){
		lek=result[Q]; line=Q+1;
		for(unsigned int i=0; i<lek.size(); i++){
			switch(lek[i]){
				case '(': norm++; break;
				case ')': norm--; if(norm<0) {ERROR(26,line); norm=0;} break;
				case '[': squar++; break;
				case ']': squar--; if(squar<0) {ERROR(26,line); squar=0;} break;
				case '{': curvy++; curvelevel=(curvelevel>curvy)?curvelevel:curvy;
				if(norm||squar) ERROR(15,line); 
				break;
				case '}': curvy--; if(curvy<0) {ERROR(26,line); curvy=0;} if(norm||squar) ERROR(15,line); break;
				default: break;
			}
		}
	}
	//////////////////////////////////////////////////////////////
	for(unsigned int i=0; i<result.size(); i++) reserve[i]=new std::string[curvelevel+2];
	for(int Q=0,curve=0; Q<result.size(); Q++){
		for(unsigned int i=0;i<result[Q].size();i++){
			if(result[Q][i]=='}') reserve[Q][curve]+=result[Q][i];
			curve+=(result[Q][i]=='}'&&curve>=0)? -1:0;
			reserve[Q][curve]+=result[Q][i];
			curve+=(result[Q][i]=='{')? 1:0;
			if(result[Q][i]=='{') {
				reserve[Q][curve]+=result[Q][i];
				reserve[Q][curve-1]+=1;
			}
		}
		//std::cout<<reserve[Q][0]<<"\n";
		//std::cout<<reserve[Q][1]<<"\n";
		//for(int i=0; i<reserve[Q][2].size(); i++)
			//std::cout<<reserve[Q][2][i]<<(int)reserve[Q][2][i]<<"\n";
	}
	if(curvy>0) ERROR(24,line); 
	if(squar>0) ERROR(24,line); 
	if(norm>0) ERROR(24,line); 
	line=0;
	norm=0; squar=0; curvy=0;
	//////////////////////////////////////////////////////////
	for(unsigned int Q=0,curve=0; curve<curvelevel+1; Q++){
		if(Q>=result.size()) { Q=0; curve++; }
		lek=reserve[Q][curve]; line=Q+1;
		//std::cout<<lek<<"\n";
		for(unsigned int i=0; i<lek.size(); i++){
			//std::cout<<"arf\n";
			retryIT:
			if(!(num||id||compr||others)) switch(lek[i]){
				case '.': dot=true;
				case DNUMS:
				statemaster.restate(NUM); 
				num=true; id=false; others=false; base=0;
				break;
				
				case '-': case '+': case '*': case '/':
				others=true;
				break;
				
				case IDS:case IDs: case '_':
				statemaster.restate(ID); 
				num=false; others=false; id=true; break;
				case '\t':case '\n':case ' ':
				num=false; id=false; others=false; break;
				
				case '<': case '>':
				statemaster.restate(COMPR);
				compr=true; break;
				
				case '=':
				statemaster.restate(EQ); break;
				
				case '(': norm++; statemaster.restate(ONPAR); break;
				case ')': norm--; if(norm<0) ERROR(14,line); norm=(norm<0)? 0:norm; statemaster.restate(CNPAR); break;
				case '[': squar++; statemaster.restate(OSPAR); break;
				case ']': squar--; if(squar<0) ERROR(14,line); squar=(squar<0)? 0:squar;statemaster.restate(CSPAR); break;
				case '{': curvy++; statemaster.restate(OCPAR); break;
				case '}': curvy--; curvy=(curvy<0)? 0:curvy; statemaster.restate(CCPAR); break;
				case ',': statemaster.restate(LIST); //std::cout<<"coma\n";
				break;
				case ';': in=false; out=false; init=false;
				if(!FORop) {norm=0; squar=0;}else {forsc++;}
				statemaster.restate(QUIT); break;
				case ':': statemaster.restate(COLON); //std::cout<<"colon\n";
				break;
				
				case '!': //std::cout<<"float\n";
				peremennie.clear(); goto cor;
				case '$': //std::cout<<"bool\n";
				peremennie.clear(); goto cor;
				case '%': //std::cout<<"int\n";
				peremennie.clear();
				cor:
				statemaster.restate(TYPE); 
				break;
				
				case 1: statemaster.restate(NUM); statemaster.restate(QUIT); break;
				
				default: others=true; break;
			}
			//************************************//
			if(num){
				if(lek[i]=='.') { if((dot||order)&&!errortick) { ERROR(11,line); errortick=true; } else dot=true; goto sympass;}
				if((lek[i]=='e'||lek[i]=='E')&&!errortick) { if(order) {ERROR(11,line); errortick=true; } else {order=true; dot=true;}goto sympass;}
				lbin = bin; loct=oct;  ldec=dec; lhex=hex;
				if((lek[i]=='-'||lek[i]=='+')&&(lek[i-1]=='e'||lek[i-1]=='E')){goto sympass;}
				if(bin) switch(lek[i]){
					case BNUMS: break;
					default: bin=false; oct=true; break;	
				}if(oct) switch(lek[i]){
					case ONUMS: break;
					default: oct=false; dec=true; break;
				}if(dec) switch(lek[i]){
					case DNUMS: break;
					default: dec=false; hex=true; break;
				}if(hex) switch(lek[i]){
					case HNUMS: case hNUMS: if(order&&!errortick) {ERROR(11,line); errortick=true;} 
					case DNUMS: break;
					default: hex=false; break;
				}sympass: if(!(hex||dec||oct||bin)){
					switch(lek[i]){
						case 'b': case 'B': 
						if(lbin) base='b';
						else ERROR(5,line); 
						break;
						case 'o': case 'O':
						if(loct||lbin) base='o';
						else ERROR(5,line); 
						break;
						case 'h': case 'H':
						if(lhex||loct||lbin||ldec) base='h';
						else ERROR(5,line); 
						break;
						case 'd': case 'D': case ' ':
						case '\n': case '\t':
						if(ldec||loct||lbin) base='d';
						else ERROR(11,line); 
						break;	
					}bin=true; num=false; dot=false; order=false; errortick=false;
					//std::cout<<coll<<"\n"; 
					coll=""; switch(lek[i]){
						case 'b': case 'B':
						case 'o': case 'O':
						case 'h': case 'H':
						case 'd': case 'D': break;
						default: goto retryIT; break;
					}
				}else coll+=lek[i];
			}
			//************************************//
			if(id){
				switch(lek[i]){
					case IDS: case IDs: case DNUMS: case '_':
					coll+=/**/lek[i]; break;
					default: 
					//
					//std::cout<<opers.get(coll)<<"\n";
					if(ops.get(coll)) {
						statemaster.fixstate(OPERATOR);
						switch(ops.getnum(coll)){
							case 0: if(DOop) {DOop=false; WHILEop=true;}else ERROR(14,line); break;
							case 1: IFop=true; break;
							case 2: FORop=true; break;
							case 3: IFop=false; THENop=true; break;
							case 4: DOop=true; break;
							case 5: out=true; break;
							case 6: in=true; break;
							case 7: if(!THENop) ERROR(14,line); THENop=false; ELSEop=true; break;
							case 8: equal=true; break;
							case 9: break;
							case 10: break;
							case 11: statemaster.fixstate(NUM);break;
							case 12: THENop=false; ELSEop=false; ENDELSE=true; statemaster.fixstate(TYPE); break;
							case 13: statemaster.fixstate(NUM);break;
							case 14: WHILEop=false; loopop=true; statemaster.fixstate(TYPE); break;
						}
						//std::cout<<((equal)?"equal":"compare")<<"\n";
						switch(ops.getnum(coll)+1){
							case 10: case 11: statemaster.fixstate(BIN_AR); break;
							case 12: case 14: statemaster.fixstate(NUM); break;
							default: break;
							
						}
					}else if(un_ar == coll) statemaster.fixstate(UN_AR);
					else
						if(equal) {
							//if(boolean.get(coll)) eql=true;
							//else if(integer.get(coll)) eqi=true;
							//else if(floating.get(coll)) eqf=true;
						}else { 
							//if(boolean.get(coll)||integer.get(coll)||floating.get(coll)) {exists=true;}
							if(!exists){if(!(out||in)) init=true;}
						}
					//std::cout<<coll<<" "<<init<<"\n";
					coll="";
					id=false;
					goto retryIT;
					break;
				}	
			}
			if(compr){
				switch(lek[i]){
					case '<': if(comp2||comp3) { ERROR(7,line); comp2=true;} else { comp2=true; break; }
					case '>': if(comp3&&!comp2){ ERROR(7,line); } else { comp3=true; if(!comp2){ break; } }
					case '=':
					default:
					compr=false; comp2=false; comp3=false;
					switch(lek[i]){
						case '<': case '>': case '=': break;
						default: goto retryIT; break;
					}
					break;
				}
			}
			//************************************//
			if(others||commentary){
				if(lek[i]=='/'&&lek[i+1]=='*') {commentary=true;}
				else if(lek[i-1]=='*'&&lek[i]=='/'&&commentary)
				{commentary=false; others=false; }
				else if(!commentary){ others=false; statemaster.restate(BIN_AR); }
				//goto retryIT;
			}
		}
	}if(WHILEop) ERROR(14,line);
	statemaster.restate(OCPAR);
	
	if(amountoferrors) std::cout<<
	"Compilation failed >w<\"\n";
	else std::cout<<
	"Compilation successfull ^w^\n";
}

void ERROR(int g, int line){
	std::cout<<errormessages[g]<<((g)?line:line-1)<<" "<<statemaster.debug()<<"\n"; 
	amountoferrors++;
}

void WARNING(int g, int line){
	std::cout<<errormessages[19+g]<<line<<" "<<g<<"\n";
}
#include <iostream>
#include <fstream>
#include "lexical_analysis.h"
#include "DFA.h"
#include "Syntax_analysis.h"
#include "SLR1.h"
using namespace std;

/*
E→TE’
E’→ATE’|ε
T→FT’
T’→MFT’ |ε
F→(E) | i
A → + | -
M → * | /
*/

int main() {
	////DFA
	//DFA Test1("c(a|b|d)*d");
	//Test1.ToDfa();
	//string simple{ "cdabddbad" };
	//cout << endl << endl;
	//cout << left <<  "输入字符串： " << setw(20) << simple;
	//cout << (Test1.DFA_Test1(simple) ? "合法字符串" : "非法字符串")<<endl;
	//simple= string{ "cdcbabad" };
	//cout << left << "输入字符串： " << setw(20) << simple;
	//cout << (Test1.DFA_Test1(simple) ? "合法字符串" : "非法字符串");

	////词法分析
	lex_ana temp("data.sy");
	temp.start_ana();


	//////LL(1)分析
	syntax_ana test;
	test.expression = temp.expression;	//将词法分析的结果给LL(1)分析器
	test.start_ana();

	//SLR(1)
	SLR test3;
	//SLR_production temp3 = { "S'",{"S"},-1 };
	//SLR_production temp4 = { "E",{"E","-","T"},0 };
	//auto a = test3.closure( temp3 );
	//for (const auto& i : a) {
	//	cout << i.left;
	//	cout << " -> ";
	//	for (auto j = i.right.begin();j != i.right.end();j++) {
	//		if (j - i.right.begin() == i.point + 1)
	//			cout << "·";
	//		cout << *j;
	//	}
	//	if(i.point+1==i.right.size())
	//		cout << "·";
	//	cout << endl;
	//}


	//test3.create_family();
	//test3.creat_ana_list();
	//test3.is_legal({"i","=","(","i","+","i",")","+","i"});
	//test3.is_legal({ "i","=","(","i","+","+",")","+","+","i" });
}
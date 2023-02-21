#pragma once
#include <vector>
#include <map>
#include <string>
#include "lexical_analysis.h"

using namespace std;
#define SYN_BLANK "BLANK"

struct production {		//存储产生式
	string left;
	vector<vector<string>> right;
};

struct single_production {		//存储单右部产生式
	string left;
	vector<string> right;
	single_production() {
		left = "0";
	}
	single_production(string a,vector<string> b):left(a),right(b){
	}
};

/*
E→TE’
E’→ATE’|ε
T→FT’
T’→MFT’ |ε
F→(E) | i
A → + | -
M → * | /
*/
class syntax_ana {
public:
	vector<vector<string>> expression;	//算数表达式
	set<string> First(string input);	//返回First集
	set<string> Follow(string input);	//返回Follow集
	bool is_legal(vector<string> input);	//判断串是否合法
	int is_toBlank(string input);
	void Select(production input);	//返回Select集
	const production& product(string input);	//输入产生式左部，返回产生式的const引用
	void start_ana();
	void syn_show();	//打印数据
	//void to_expression(vector<vector<string>> input);
	//void set_data(vector<production> &a, vector<string>&b, vector<string>&c, string start);
private:
	vector<production> LL_1{
		{"S",{{"M","H'"},{"a"}}},
		{"H",{{"L","S","o"},{SYN_BLANK}}},
		{"K",{{"d","M","L"},{SYN_BLANK}}},
		{"L",{{"e","H","f"}}},
		{"M",{{"b","L","M"},{"K"}}}
	};
	vector<single_production> single_LL_1;		//存储单右部产生式和select集

	vector<string> terminator{"a","b","c","d","e","o","$"};		//终结符
	vector<string> non_terminator{"S","M","H","L","K"};	//非终结符

	vector<vector<single_production>>	ana_list;	//分析表
	map<string, int> terminator_list, non_terminator_list;		//map形式的终结符和非终结符


	string start_state{ "S" };	//开始符号
	map<string,set<string>> my_first, my_follow;	//存储First集和Follow集 
	vector<set<string>> select;

	vector<int> false_location; // 错误的单词

	
};

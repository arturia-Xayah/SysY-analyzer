#pragma once
#include "Syntax_analysis.h"

struct SLR_production {
	string left;
	vector<string> right;
	int point = -1;		//原点位置，表示在该位置之后
	bool operator < (const SLR_production&  a)const {
		int i = (right < a.right) * 100 + (left < a.left) * 10 + (point < a.point);
		int j = (right > a.right) * 100 + (left > a.left) * 10 + (point > a.point);
		return i < j;
		
		//if (right < a.right)
		//	return 1;
		//else if (right == a.right) {
		//	if (left < a.left)
		//		return 1;
		//	else if (left < a.left) {
		//		if (point < a.point)
		//			return 1;
		//		else
		//			return 0;
		//	}
		//	else
		//		return 0;
		//}
		//else
		//	return 0;


		//if (left == a.left && right == a.right && point == a.point)
		//	return 0;
		//else
		//	return 1;
	}

	bool operator == (const SLR_production& a)const {
		if (left == a.left && right == a.right && point == a.point)
			return 1;
		else
			return 0;
	}
};


class SLR {
public:
	set<SLR_production> closure(SLR_production input);		//求项目集的闭包
	set<SLR_production> Go_to(set<SLR_production> start, string input);
	void create_family();	//构建项目集族
	vector<SLR_production> SLR_Find(string input);		//寻找左部相同的产生式
	void creat_ana_list();	//构造分析表
	bool is_legal(vector<string> input);	//开始分析
	SLR();
	~SLR();
	/*******Follow集*******/
	set<string> First(string input);	//返回First集
	set<string> Follow(string input);	//返回Follow集
	int is_toBlank(string input);
	const production& product(string input);	//输入产生式左部，返回产生式的const引用
private:
	vector<SLR_production> SLR_Data{
		{"S'",vector<string>{"S"}},
		{"S",vector<string>{"i","=","E"}},
		{"E",vector<string>{"E","+","T"}},
		{"E",vector<string>{"E","-","T"}},
		{"E",vector<string>{"T"}},
		{"T",vector<string>{"T","*","F"}},
		{"T",vector<string>{"T","/","F"}},
		{"T",vector<string>{"F"}},
		{"F",vector<string>{"(","E",")"}},
		{"F",vector<string>{"i"}}
	};
	vector<production> SLR_1;	//存储LL1形式的SLR1
	vector<string> terminator{ "+","-","*","/","(",")","i","=","$" };		//终结符
	vector<string> non_terminator{ "S'","S","E","T","F"};	//非终结符
	map<string, int> symbol_map;	//终结符与非终结符对应的表中序号
	map<string, int> terminator_list, non_terminator_list;		//map形式的终结符和非终结符

	vector<vector<pair<char,int>>> ana_list;	//分析表
	vector<set<SLR_production>> SLR_Family;		//存储项目集族
	vector<map<string,int>> SLR_Family_To;	//存储状态转移
	map<set<SLR_production>, int> SLR_Family_Map;

	string start_state{ "S'" };	//开始符号
	map<string, set<string>> my_first, my_follow;	//存储First集和Follow集

	vector<int> false_location; // 错误的单词
};

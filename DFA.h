#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <iomanip>

#define BLANK '/'
#define DFA_DIGIT string{"."}
#define DFA_WORD string{"^"}
#define DIGIT string{"0|1|2|3|4|5|6|7|8|9"}
#define WORD string{"a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|_"}

using namespace std;

struct state {	//NFA中的单个状态转移数据
	int start;
	int end;
	char receive;
};

struct DFA_state {	//临时存储DFA状态转移
	set<int> start;
	set<int> end;
	char receive;
};

template<class T, class B>
bool is_in(T t, B b) {	//查询b是否在容器t中
	if (find(t.begin(), t.end(), b) == t.end())
		return 0;
	else
		return 1;
}

template<class T>
void show_data(T t) {
	for (const auto& i : t) {
		cout << i;
	}
	cout << endl;
}

class DFA {
	vector<vector<state>> NFA{
		{{0,1,BLANK},{0,7,BLANK}},
		{{1,2,BLANK},{1,4,BLANK}},
		{{2,3,'a'}},
		{{3,6,BLANK}},
		{{4,5,'b'}},
		{{5,6,BLANK}},
		{{{6,1,BLANK},{6,7,BLANK}}},
		{{7,8,'a'}},
		{{8,9,'b'}},
		{{9,10,'b'}},
		{}
	};
	vector<vector<int>> My_DFA;

	map<set<int>,int> state_list;	//存放子集构造法构造的子集
	vector<char> input_char{'0','1'};	//存放该DFA可接受的字符（如a，b）
	map<char, int> input_map;	//存放该DFA可接受的字符转为int（如（a，0），（b，1））
	vector<int> receive_NFA{10};	//NFA接收状态表
	vector<int> receive_DFA;	//DFA接收状态表
	vector<string> keyword{ "auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while" };

public:
	DFA(string data);
	~DFA();
	set<int> Closure(int start);
	set<int> Closure(set<int>& my_set); 	//返回状态集合my_set的闭包
	set<int> Edge(int start, char input);
	set<int> Edge(set<int> my_set, char input);
	set<int> Move(set<int>& my_set, char input);	//返回状态集合my_set的input边转换
	bool DFA_Test(string input);	//返回该字符串能否被DFA识别
	bool DFA_Test1(string input);	//返回该字符串能否被DFA识别
	int DFA_Class(string input);	//返回该字符串的类型，0错误，1保留字，2变量名，3十进制数，4十六进制数
	void ToDfa();
	void Show_Dfa();
	
};


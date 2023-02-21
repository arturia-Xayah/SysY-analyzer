#pragma once
#include <vector>
#include <map>
#include <string>
#include "lexical_analysis.h"

using namespace std;
#define SYN_BLANK "BLANK"

struct production {		//�洢����ʽ
	string left;
	vector<vector<string>> right;
};

struct single_production {		//�洢���Ҳ�����ʽ
	string left;
	vector<string> right;
	single_production() {
		left = "0";
	}
	single_production(string a,vector<string> b):left(a),right(b){
	}
};

/*
E��TE��
E����ATE��|��
T��FT��
T����MFT�� |��
F��(E) | i
A �� + | -
M �� * | /
*/
class syntax_ana {
public:
	vector<vector<string>> expression;	//�������ʽ
	set<string> First(string input);	//����First��
	set<string> Follow(string input);	//����Follow��
	bool is_legal(vector<string> input);	//�жϴ��Ƿ�Ϸ�
	int is_toBlank(string input);
	void Select(production input);	//����Select��
	const production& product(string input);	//�������ʽ�󲿣����ز���ʽ��const����
	void start_ana();
	void syn_show();	//��ӡ����
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
	vector<single_production> single_LL_1;		//�洢���Ҳ�����ʽ��select��

	vector<string> terminator{"a","b","c","d","e","o","$"};		//�ս��
	vector<string> non_terminator{"S","M","H","L","K"};	//���ս��

	vector<vector<single_production>>	ana_list;	//������
	map<string, int> terminator_list, non_terminator_list;		//map��ʽ���ս���ͷ��ս��


	string start_state{ "S" };	//��ʼ����
	map<string,set<string>> my_first, my_follow;	//�洢First����Follow�� 
	vector<set<string>> select;

	vector<int> false_location; // ����ĵ���

	
};

#pragma once
#include "Syntax_analysis.h"

struct SLR_production {
	string left;
	vector<string> right;
	int point = -1;		//ԭ��λ�ã���ʾ�ڸ�λ��֮��
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
	set<SLR_production> closure(SLR_production input);		//����Ŀ���ıհ�
	set<SLR_production> Go_to(set<SLR_production> start, string input);
	void create_family();	//������Ŀ����
	vector<SLR_production> SLR_Find(string input);		//Ѱ������ͬ�Ĳ���ʽ
	void creat_ana_list();	//���������
	bool is_legal(vector<string> input);	//��ʼ����
	SLR();
	~SLR();
	/*******Follow��*******/
	set<string> First(string input);	//����First��
	set<string> Follow(string input);	//����Follow��
	int is_toBlank(string input);
	const production& product(string input);	//�������ʽ�󲿣����ز���ʽ��const����
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
	vector<production> SLR_1;	//�洢LL1��ʽ��SLR1
	vector<string> terminator{ "+","-","*","/","(",")","i","=","$" };		//�ս��
	vector<string> non_terminator{ "S'","S","E","T","F"};	//���ս��
	map<string, int> symbol_map;	//�ս������ս����Ӧ�ı������
	map<string, int> terminator_list, non_terminator_list;		//map��ʽ���ս���ͷ��ս��

	vector<vector<pair<char,int>>> ana_list;	//������
	vector<set<SLR_production>> SLR_Family;		//�洢��Ŀ����
	vector<map<string,int>> SLR_Family_To;	//�洢״̬ת��
	map<set<SLR_production>, int> SLR_Family_Map;

	string start_state{ "S'" };	//��ʼ����
	map<string, set<string>> my_first, my_follow;	//�洢First����Follow��

	vector<int> false_location; // ����ĵ���
};

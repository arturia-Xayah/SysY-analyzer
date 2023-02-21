#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "DFA.h"



class lex_ana {
private:
	vector<string> code_data;
	vector<string> keyword{ "auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while"};
	vector<char> OP{ '+','-','*','/','%','<','>','!','=' };	//�����
	vector<char> ge{ '(' , ')' , '{' , '}' , ';' , ',' , '[' , ']'};	//�ָ���

	vector<pair<char, char>> True_OP{ 
		pair<char, char> ('=','='),
		pair<char, char>('!','='),
		pair<char, char>('>','='),
		pair<char, char>('<','='),
		pair<char, char>('(',')'),
		pair<char, char>('{','}'),
		pair<char, char>('*','*'),
		pair<char, char>('+','+'),
		pair<char, char>('-','-')
	};		//�Ϸ����������ȵ������

	map<char, int> Hex_Map{pair<char,int>('0',0),pair<char,int>('1',1) ,pair<char,int>('2',2) ,pair<char,int>('3',3) ,pair<char,int>('4',4),
		pair<char,int>('5',5) ,pair<char,int>('6',6) ,pair<char,int>('7',7) ,pair<char,int>('8',8) ,pair<char,int>('9',9),
		pair<char,int>('a',10) ,pair<char,int>('A',10) ,pair<char,int>('b',11) ,pair<char,int>('B',11) ,
		pair<char,int>('c',12) ,pair<char,int>('C',12) ,pair<char,int>('d',13) ,pair<char,int>('D',13) ,
		pair<char,int>('e',14) ,pair<char,int>('E',14) ,pair<char,int>('f',15) ,pair<char,int>('F',15) , };	//16����ת10����
	DFA my_DFA{ DFA_WORD + "+(" + DFA_DIGIT + "|" + DFA_WORD + ")*" };
public:
	vector<vector<string>> expression;	//�������ʽ
	lex_ana(string path);
	~lex_ana();
	void start_ana();
	void Remove_Comments();	//ȥ��ע��
	void lex_show_data();
};

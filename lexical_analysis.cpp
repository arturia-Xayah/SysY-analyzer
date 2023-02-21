#include "lexical_analysis.h"

using namespace std;


lex_ana::lex_ana(string path) {
	ifstream p(path);
	if (!p.is_open()) {
		cout << "���ļ�ʧ�ܣ�" << endl;
	}
	string temp;
	while (getline(p, temp)) {
		code_data.push_back(temp);
	}

}

lex_ana::~lex_ana() {

}

void lex_ana::Remove_Comments() {	//ȥ��ע��
	bool is_note = 0;
	vector<string> putout;
	int line = 0;
	for (const auto& i : code_data) {
		line++;
		string temp;
		for (auto j = i.begin();j != i.end();j++) {
			if (i.end() - j >= 2) {
				if (is_note==0 && *j == '/' && *(j + 1) == '/') {		//����ע��
					//temp = string(i.begin(), j-1);
					break;
				}
				else if (*j == '/' && *(j + 1) == '*') {
					if (is_note)
						cout <<"Error:"<< "[ע��ȱ��*/] at line [" << line <<"]"<<endl;
					else {
						is_note = 1;
					}
				}
				else if (*j == '*' && *(j + 1) == '/') {
					if (is_note) {
						is_note = 0;
						j += 2;
						if (j == i.end())
							break;
					}
					else {
						cout << "Error:" << "[ע��ȱ��/*] at line [" << line << "]" << endl;
					}
				}
			}
			if (!is_note)
				temp.push_back(*j);
			
		}
		putout.push_back(temp);
	}
	code_data = putout;
}

void lex_ana::lex_show_data() {
	for (const auto& i : code_data)
		show_data(i);
}

void lex_ana::start_ana() {
	Remove_Comments();
	my_DFA.ToDfa();
	lex_show_data();
	int line = 0;
	for (auto i = code_data.begin();i != code_data.end();i++) {
		line++;
		string::iterator start;	//���ڲ��ҵ���
		string::iterator end;
		bool flag = 0;	//�����ж��Ƿ����������ʽ
		vector<string> express;
		for (auto j = i->begin();j != i->end();) {
			while (j != i->end() && (*j == ' '|| *j=='	' )) {	//���ӿո�
				j++;
				
			}
			if (j == i->end())
				break;

			if (is_in(ge, *j)) {	//���ҵ��ָ���
				cout << "< �ָ��� " <<find(ge.begin(),ge.end(),*j)-ge.begin() <<" , " <<*j << " >" << endl;
				j++;
				flag = 0;
			}
			else if (is_in(OP, *j)) {	//���ҵ������
				if (j!=i->end()-1 && is_in(OP, *(j+1))) {		//�ж��Ƿ����������������
					pair<char, char> temp(*j,*(j+1));
					if (!is_in(True_OP, temp)) {	//�Ƿ������
						cout << "Error: [" << *j<< * (j + 1) << "] [������Ƿ�] at line [" << line << "]" << endl;
						j += 2;
						continue;
					}
						
				}
				cout << "< ����� " << find(ge.begin(), ge.end(), *j) - ge.begin() << " , " << *j << " >" << endl;

				if (*j == '=')
					flag = 1;
				else if (is_in(string{"+-*/"}, *j) && flag)
					express.push_back(string{ *j });
				j++;
			}
			else {	//�����ǣ�����ҵ���
				start = j++;	//jָ�򵥴ʺ�һ��Ԫ�صĵ�ַ
				
				if (j != i->end()) {
					while (is_in(WORD + DIGIT, *j)) {
						j++;
						if (j == i->end())
							break;
					}
				}

				end = j;
				string temp(start, end);
				int temp_num = 0;	//�������
				switch (my_DFA.DFA_Class(temp))
				{
				case 0:
					cout << "Error: ["<< temp << "] [���ƷǷ�] at line [" << line << "]" << endl;
					break;
				case 1:
					cout << "< ������ " << find(keyword.begin(), keyword.end(), temp) - keyword.begin() << " , " << temp << " >" << endl;
					break;
				case 2:
					cout << "< ��ʶ�� "  << " , " << temp << " >" << endl;
					if(flag)
						express.push_back(temp);
					break;
				case 3:	//ʮ����
					for (auto t = temp.begin();t != temp.end();t++)
						temp_num = *t - '0'+temp_num*10;
					cout << "< NUM " <<  ", " << temp_num << " >" << endl;
					if (flag)
						express.push_back(to_string(temp_num));
					break;
				case 4:	//ʮ������
					for (auto t = temp.begin()+2;t != temp.end();t++)
						temp_num = Hex_Map[*t] + temp_num * 16;
					cout << "< NUM " << ", " << temp_num << " >" << endl;
					if (flag)
						express.push_back(to_string(temp_num));
					break;
				default:
					break;
				}
			}
		}
		if(!express.empty())
			expression.push_back(express);
	}
}
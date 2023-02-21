#include "DFA.h"
#include <queue>

bool is_high(char a,char b) {	//�������ȼ��Ƚ�,a>b��return 1  *>+>|
	map<char, int> temp{ pair<char,int>('*',3),pair<char,int>('+',2),pair<char,int>('|',1) };
	if (temp[a] > temp[b])
		return 1;
	else
		return 0;
}

DFA::DFA(string data) {	//a+(a|b)*      
	string temp1;
	for (auto i = data.begin();i != data.end();i++) {	//��������ʽ�����+
		temp1.push_back(*i);
		if (i + 1 != data.end()) {
			if (!is_in(string{ "+|(" }, *i) && !is_in(string{ "+*|)" }, *(i+1)))
				temp1.push_back('+');
		}
	}
	data = temp1;

	NFA.clear();
	NFA.resize(0);
	input_char.clear();
	receive_NFA.clear();
	string keywords{"|()*+"};	//������ʽ�ı����ַ�
	vector<char> key_words;		//�����ַ�ջ+  (|)*
	vector<char> rec_words;		//��׺ʽ��
	for (const auto& i : data) {	//ת��Ϊ��׺ʽ
		if (is_in(keywords, i)) {
			if (key_words.empty())
				key_words.push_back(i);
			else if (i == '(')
				key_words.push_back(i);
			else if (i == ')') {
				while (key_words.back() != '(') {
					rec_words.push_back(key_words.back());
					key_words.pop_back();
				}
				key_words.pop_back();
			}
			else if(is_high(i,key_words.back()))	//���ȼ�����ջ���ķ���
				key_words.push_back(i);
			else {
				while (!is_high(i, key_words.back())) {	 //���ȼ�С��ջ���ķ���
					rec_words.push_back(key_words.back());
					key_words.pop_back();
					if (key_words.empty())
						break;
				}
				key_words.push_back(i);
			}
		}
		else
			rec_words.push_back(i);
	}
	while (!key_words.empty()) {
		rec_words.push_back(key_words.back());
		key_words.pop_back();
	}
	cout << "��׺���ʽ:";
	show_data(rec_words);

	//����׺���ʽת��ΪNFA				aab|*+     BLANKΪռλ��
	vector<char> ana_stack;		//����ջ
	vector<pair<int,int>> state_stack;		//״̬�洢ջ
	int now_state = 1;		//��ǰ��״̬
	int last_state = 1;
	for (auto i = rec_words.begin();i != rec_words.end();i++) {
		pair<int, int> a(last_state, now_state), b(last_state, now_state);
		switch (*i)
		{
		case '|':
			NFA.resize(now_state + 4);
			if (*(ana_stack.end() - 2) == BLANK) {
				a = state_stack.back();
				state_stack.pop_back();
			}
			if (ana_stack.back() == BLANK) {
				b = state_stack.back();
				state_stack.pop_back();
			}
			//a
			NFA[now_state + 1].push_back({ now_state + 1,a.first,BLANK });
			NFA[a.second].push_back({ a.second,now_state + 2,*(ana_stack.end()-2) });

			//b
			NFA[now_state + 1].push_back({ now_state + 1,b.first,BLANK });
			NFA[b.second].push_back({ b.second,now_state + 2,ana_stack.back() });

			last_state = now_state+1;
			now_state+=2;
			state_stack.push_back(pair<int, int>(last_state, now_state));
			last_state = ++now_state;
			ana_stack.pop_back();
			ana_stack.pop_back();
			ana_stack.push_back(BLANK);
			break;
		case '*':
			NFA.resize(now_state + 4);

			if (ana_stack.back() == BLANK) {
				a = state_stack.back();
				state_stack.pop_back();
			}

			NFA[now_state + 1].push_back({ now_state + 1,a.first,BLANK });
			NFA[now_state + 1].push_back({ now_state + 1,now_state + 2,BLANK });
			NFA[a.second].push_back({ a.second,now_state+2,BLANK });
			NFA[a.second].push_back({ a.second ,a.first,ana_stack.back() });
			last_state = now_state + 1;
			now_state += 2;
			state_stack.push_back(pair<int, int>(last_state, now_state));
			last_state = ++now_state;
			ana_stack.pop_back();
			ana_stack.push_back(BLANK);
			break;
		case '+':
			NFA.resize(now_state + 5);

			if (*(ana_stack.end() - 2) == BLANK) {
				a = state_stack.back();
				state_stack.pop_back();
			}
			if (ana_stack.back() == BLANK) {
				b = state_stack.back();
				state_stack.pop_back();
			}

			//a
			NFA[now_state + 1].push_back({ now_state + 1,a.first,*(ana_stack.end() - 2) });

			//b
			NFA[b.second].push_back({ b.second,now_state + 2,ana_stack.back() });

			//a b����
			//if(a.second!=b.first)
			NFA[a.second].push_back({ a.second,b.first,BLANK });

			last_state = now_state + 1;
			now_state += 2;
			state_stack.push_back(pair<int, int>(last_state, now_state));
			last_state = ++now_state;
			ana_stack.pop_back();
			ana_stack.pop_back();
			ana_stack.push_back(BLANK);
			break;
		default:
			ana_stack.push_back(*i);
			if(!is_in(input_char,*i))
				input_char.push_back(*i);
			break;
		}
			
	}
	NFA[0].push_back({ 0,state_stack.front().first,BLANK });
	receive_NFA.push_back(state_stack.front().second);
	//show_data(input_char);
	//show_data(receive_NFA);
	//for (const auto& i : NFA) {
	//	for (const auto& j : i) {
	//		cout << j.start<<","<<j.end << "," << j.receive << "   ";
	//	}
	//	cout << endl;
	//}
}

DFA::~DFA() {

}

set<int> DFA::Edge(int start, char input) {	//����start����input�ߵ����״̬�ļ���
	set<int> putout;
	auto temp = NFA[start];
	if (temp.empty())
		return putout;
	for (auto &i : temp) {
		if (i.receive == input) {
			putout.insert(i.end);
		}
	}
	return putout;
}

set<int> DFA::Edge(set<int> my_set, char input) {	//���ؼ���my_set�е�ÿ��״̬����input�ߵ����״̬�ļ���
	set<int> putout;
	for (auto i = my_set.begin();i != my_set.end();i++) {
		set<int> temp(Edge(*i, input));
		putout.insert(temp.begin(), temp.end());
	}
	return putout;
}

set<int> DFA::Closure(int start) {	//����״̬start�ıհ�
	set<int> putout;
	putout.insert(start);
	int last_len = -1;
	while (putout.size() != last_len) {
		last_len = putout.size();
		auto temp= Edge(putout, BLANK);
		putout.insert(temp.begin(), temp.end());
	}
	return putout;
}

set<int> DFA::Closure(set<int> & my_set) {	//����״̬����my_set�ıհ�
	set<int> putout = my_set;
	int last_len = -1;
	while (putout.size() != last_len) {
		last_len = putout.size();
		auto temp(Edge(putout, BLANK));
		putout.insert(temp.begin(), temp.end());
	}
	return putout;
}

set<int> DFA::Move(set<int>& my_set, char input) {	//����״̬����my_set��input��ת��
	set<int> temp= Edge(my_set, input);
	return Closure(temp);
}

void DFA::ToDfa() {
	queue<set<int>> Q;	//��������״̬����(open��
	vector<set<int>> close;	//close��(����open����ģ�
	vector<DFA_state> temp_state;	//�洢DFA��ʱ״̬
	//set<int> tt{1,2,4,5,6,7,9};
	//Move(tt, 'b');
	//����״̬ת����
	Q.push(Closure(0));
	close.push_back(Closure(0));
	while (!Q.empty()) {
		set<int> &temp = Q.front();
		for (const char& input : input_char) {
			set<int> temp2 = Move(temp, input);
			if (!temp2.empty()) {			//��Ԫ�طǿ��Ҳ���close����
				if (find(close.begin(), close.end(), temp2) == close.end()) {
					Q.push(temp2);
					close.push_back(temp2);
				}
				temp_state.push_back({temp,temp2,input});
			}
			
		}
		state_list.insert(pair<set<int>, int>(temp, state_list.size()));
		receive_DFA.push_back(0);
		for (const auto& i : receive_NFA) {		//�ж��Ƿ�Ϊ����״̬
			if (find(temp.begin(), temp.end(), i) != temp.end()) {
				receive_DFA[state_list.size() - 1] = 1;
				break;
			}
		}
		Q.pop();
	}

	for (int i = 0;i < input_char.size();i++)
		input_map.insert(pair<char,int>(input_char[i], i));

	My_DFA.resize(state_list.size());
	for (auto& i : My_DFA)
		i.resize(input_char.size(),-1);
	for (const auto& i : temp_state) {
		My_DFA[state_list[i.start]][input_map[i.receive]] = state_list[i.end];
	}

	//�ṹչʾ

	
	Show_Dfa();
}

void DFA::Show_Dfa() {

	cout << "�����Ӽ���Ӧ��״̬��" << endl;
	for (const auto& i : state_list) {
		auto x = i.first;
		auto y = i.second;
		for (const auto& j : x) {
			cout << j << " ";
		}
		cout << ": " << y;
		cout << endl;
	}


	cout << endl;
	int num = 0;
	cout << "DFA�Ľṹ��" << endl;
	cout << left << setw(5) << "״̬"<< setw(15) << "�Ƿ�Ϊ����״̬";
	for (const auto& i : input_char)
		cout << left << setw(5) << i;
	cout << endl;
	for (const auto& i : My_DFA) {
		cout << left << " "<<setw(5) << num++;
		cout << left << setw(5) << " ";
		if (receive_DFA[num - 1])
			cout << setw(9) << "Yes";
		else
			cout << setw(9) << "No";

		for (const auto& j : i)
			cout << left << setw(5) << j;
		cout << endl;
	}

}

//���������Ե�DFA
bool DFA::DFA_Test(string input) {
	int now_state = 0;
	int next_state = 0;
	for (auto i : input) {
		if (is_in(DIGIT, i)) {	//�������ַ���ת��Ϊ��Ƿ���
			i = DFA_DIGIT[0];
		}
		else if (is_in(WORD, i)) {
			i = DFA_WORD[0];
		}
		next_state = My_DFA[now_state][input_map[i]];
		if (next_state == -1)
			return 0;
		else
			now_state = next_state;
	}
	if (receive_DFA[now_state])
		return 1;
	else
		return 0;
}

//������ȫDFA
bool DFA::DFA_Test1(string input) {
	int now_state = 0;
	int next_state = 0;
	for (auto i : input) {
		next_state = My_DFA[now_state][input_map[i]];
		if (next_state == -1)
			return 0;
		else
			now_state = next_state;
	}
	if (receive_DFA[now_state])
		return 1;
	else
		return 0;
}


int DFA::DFA_Class(string input) {		//���ظ��ַ��������ͣ�0����1�����֣�2��������3ʮ��������4ʮ��������
	if (DFA_Test(input)) {
		if (is_in(keyword, input))
			return 1;
		else
			return 2;
	}
	else if(input.size()>2 && input[0]=='0' && (input[1]=='X' || input[1] == 'x')){		//�ж�16������
		for (auto i = input.begin() + 2;i != input.end();i++) {
			if ((*i >= '0' && *i <= '9')||(*i >= 'a' && *i <= 'f') || (*i >= 'A' && *i <= 'F'));
			else 
				return 0;
		}
		return 4;
	}
	else {
		for (const auto& i : input) {
			if (i >= '0' && i <= '9');
			else
				return 0;
		}
		return 3;
	}
}
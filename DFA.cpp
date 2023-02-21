#include "DFA.h"
#include <queue>

bool is_high(char a,char b) {	//符号优先级比较,a>b则return 1  *>+>|
	map<char, int> temp{ pair<char,int>('*',3),pair<char,int>('+',2),pair<char,int>('|',1) };
	if (temp[a] > temp[b])
		return 1;
	else
		return 0;
}

DFA::DFA(string data) {	//a+(a|b)*      
	string temp1;
	for (auto i = data.begin();i != data.end();i++) {	//向正则表达式中添加+
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
	string keywords{"|()*+"};	//正则表达式的保留字符
	vector<char> key_words;		//保留字符栈+  (|)*
	vector<char> rec_words;		//后缀式子
	for (const auto& i : data) {	//转化为后缀式
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
			else if(is_high(i,key_words.back()))	//优先级大于栈顶的符号
				key_words.push_back(i);
			else {
				while (!is_high(i, key_words.back())) {	 //优先级小于栈顶的符号
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
	cout << "后缀表达式:";
	show_data(rec_words);

	//将后缀表达式转换为NFA				aab|*+     BLANK为占位符
	vector<char> ana_stack;		//操作栈
	vector<pair<int,int>> state_stack;		//状态存储栈
	int now_state = 1;		//当前的状态
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

			//a b连接
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

set<int> DFA::Edge(int start, char input) {	//返回start经过input边到达的状态的集合
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

set<int> DFA::Edge(set<int> my_set, char input) {	//返回集合my_set中的每个状态经过input边到达的状态的集合
	set<int> putout;
	for (auto i = my_set.begin();i != my_set.end();i++) {
		set<int> temp(Edge(*i, input));
		putout.insert(temp.begin(), temp.end());
	}
	return putout;
}

set<int> DFA::Closure(int start) {	//返回状态start的闭包
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

set<int> DFA::Closure(set<int> & my_set) {	//返回状态集合my_set的闭包
	set<int> putout = my_set;
	int last_len = -1;
	while (putout.size() != last_len) {
		last_len = putout.size();
		auto temp(Edge(putout, BLANK));
		putout.insert(temp.begin(), temp.end());
	}
	return putout;
}

set<int> DFA::Move(set<int>& my_set, char input) {	//返回状态集合my_set的input边转换
	set<int> temp= Edge(my_set, input);
	return Closure(temp);
}

void DFA::ToDfa() {
	queue<set<int>> Q;	//待分析的状态队列(open表）
	vector<set<int>> close;	//close表(包括open表里的）
	vector<DFA_state> temp_state;	//存储DFA临时状态
	//set<int> tt{1,2,4,5,6,7,9};
	//Move(tt, 'b');
	//构造状态转换表
	Q.push(Closure(0));
	close.push_back(Closure(0));
	while (!Q.empty()) {
		set<int> &temp = Q.front();
		for (const char& input : input_char) {
			set<int> temp2 = Move(temp, input);
			if (!temp2.empty()) {			//新元素非空且不在close表内
				if (find(close.begin(), close.end(), temp2) == close.end()) {
					Q.push(temp2);
					close.push_back(temp2);
				}
				temp_state.push_back({temp,temp2,input});
			}
			
		}
		state_list.insert(pair<set<int>, int>(temp, state_list.size()));
		receive_DFA.push_back(0);
		for (const auto& i : receive_NFA) {		//判断是否为接收状态
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

	//结构展示

	
	Show_Dfa();
}

void DFA::Show_Dfa() {

	cout << "构造子集对应的状态：" << endl;
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
	cout << "DFA的结构：" << endl;
	cout << left << setw(5) << "状态"<< setw(15) << "是否为接收状态";
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

//用于有缩略的DFA
bool DFA::DFA_Test(string input) {
	int now_state = 0;
	int next_state = 0;
	for (auto i : input) {
		if (is_in(DIGIT, i)) {	//将具体字符串转化为标记符号
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

//用于完全DFA
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


int DFA::DFA_Class(string input) {		//返回该字符串的类型，0错误，1保留字，2变量名，3十进制数，4十六进制数
	if (DFA_Test(input)) {
		if (is_in(keyword, input))
			return 1;
		else
			return 2;
	}
	else if(input.size()>2 && input[0]=='0' && (input[1]=='X' || input[1] == 'x')){		//判断16进制数
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
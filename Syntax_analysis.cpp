#include "Syntax_analysis.h"

const production& syntax_ana::product(string input) {
	for (const auto& i : LL_1) {
		if (i.left == input)
			return i;
	}
}

set<string> syntax_ana::First(string input) {
	if (my_first.count(input))
		return my_first[input];
	set<string> putout;
	if (input == SYN_BLANK || is_in(terminator, input)) {	//input是终结符号
		putout.insert(input);
		return putout;
	}
	else {
		vector<vector<string>> temp = product(input).right;
		if (temp.size() > 1) {		//一条产生式有多个分支
			for (const auto& i : temp) {
				auto temp_putout = First(i.front());
				putout.insert(temp_putout.begin(),temp_putout.end());
			}
			return putout;
		}
		for (auto i = temp.front().begin();i != temp.front().end();i++) {	// *i是一个string
			set<string> temp2 = First(*i);
			if (is_in(temp2, SYN_BLANK)) {
				if(i!= temp.front().end()-1)
					temp2.erase(SYN_BLANK);
				putout.insert(temp2.begin(), temp2.end());
				return putout;
			}
			else {
				putout.insert(temp2.begin(), temp2.end());
				return putout;
			}
		}
	}
	return putout;
}

set<string> syntax_ana::Follow(string input) {
	if (my_follow.count(input))
		return my_follow[input];

	set<string> putout;

	if (input == start_state) {
		putout.insert("$");
	}
	for (const auto&a :LL_1) {	//每一条产生式
		for (const auto& i : a.right) {		//i为单独的产生式右部
			if (is_in(i, input)) {
				for (auto j = i.begin();j != i.end();j++) {
					if (*j==input) {		//非终结符
						if (j != i.end() - 1) {
							if (is_toBlank(*(j + 1)) == 0 || is_toBlank(*(j + 1)) == 2) {	//不能产生BLANK的部分
								set<string> temp_putout{ First(*(j + 1)) };
								if (temp_putout.find(SYN_BLANK) != temp_putout.end()) {
									temp_putout.erase(SYN_BLANK);
								}
								putout.insert(temp_putout.begin(), temp_putout.end());
							}

							if (is_toBlank(*(j + 1)) == 2) {	//可以产生BLANK的部分
								if (a.left != input) {
									set<string> temp_putout{ Follow(a.left) };
									putout.insert(temp_putout.begin(), temp_putout.end());
								}
							}
						}
						else if(j == i.end() - 1 ){
							if (a.left != input) {
								set<string> temp_putout{ Follow(a.left) };
								putout.insert(temp_putout.begin(), temp_putout.end());
							}		
						}			
					}
				}
			}
		}
	}
	return putout;
}

int syntax_ana::is_toBlank(string input) {		//return    0不会产生BLANK,1只会产生BLANK,2两个都会产生
	if (is_in(terminator, input))
		return 0;
	auto temp = product(input).right;
	//if (temp.size() == 1 && temp.front().size()==1 && temp.front().front() == SYN_BLANK)
	//	return 1;

	bool can_to = 0;
	bool cant_to = 0;
	for (const auto& i : temp) {	//i为唯一的右部
		
		//for (const auto& j : i) {	//右部中的每一个
		//	if (is_in(terminator, j)) {	//终结符
		//		flag = 0;
		//		break;
		//	}
		//	else if(is_toBlank(j)==0){	//只要有一个不为BLANK，则该产生式就不会是BLANK
		//		flag = 0;
		//		break;
		//	}
		//}
		if (i.size() == 1 && i.front() == SYN_BLANK)
			can_to = 1;
		else
			cant_to = 1;
	}
	if (cant_to && can_to)
		return 2;
	else if (cant_to)
		return 0;
	else
		return 1;
	
}


void syntax_ana::Select(production input) {
	for (const auto& j : input.right) {	//j为单条产生式右部
		if (j.size() == 1 && j.front() == SYN_BLANK) {
			set<string> putout(First(j.front()));
			set<string> temp(Follow(input.left));
			putout.insert(temp.begin(), temp.end());
			single_LL_1.push_back({input.left,j});
			select.push_back(putout);
			//show_data(putout);
		}
		else {
			set<string> putout(First(j.front()));
			single_LL_1.push_back({ input.left,j});
			select.push_back(putout);
			//show_data(putout);
		}
	}
	
}

bool syntax_ana::is_legal(vector<string> input) {
	string trans_input;
	for (const auto& i : input) {
		if (is_in(terminator, i))		//运算符号
			trans_input.push_back(i.front());
		else{		//数字
			trans_input.push_back('i');
		}
	}
	trans_input.push_back('$');
	//开始分析
	vector<string> my_stack{"$",start_state};
	int flag = 1;
	false_location.clear();
	int count = -1;	//位置计数
	cout << endl;
	cout << left << setw(20) << "已匹配";
	cout << left << setw(20) << "栈";
	cout << left << setw(20) << "输入串";
	cout << left << setw(20) << "动作"<<endl;
	string last_putout;
	for (const auto& i : trans_input) {
		count++;
		//cout <<endl<<endl<< "------" << i<<endl;
		//show_data(my_stack);
		//show_data(trans_input);
		for (;;) {
			if (my_stack.back() == "$" && string{ i } == "$") {	//匹配完成
				string string_temp;
				//cout << left << setw(20);
				for (int t = 0;t < count;t++)
					string_temp += trans_input[t];
				cout << left << setw(20) << string_temp;
				string_temp.clear();
				for (auto t = my_stack.rbegin();t != my_stack.rend();t++)
					string_temp += *t;
				cout << left << setw(20) << string_temp;
				string_temp.clear();
				for (int t = count;t < trans_input.size();t++)
					string_temp += trans_input[t];
				cout << left << setw(20) << string_temp;
				string_temp.clear();

				cout << last_putout << endl;
				last_putout = "匹配 " + string{ i };
				return flag;
			}
			auto top_stack = my_stack.back();	//栈顶元素
			if (top_stack == SYN_BLANK) {	//产生式右部为BLANK
				my_stack.pop_back();
			}
			else if (top_stack == string{ i }) {
				my_stack.pop_back();
				string string_temp;
				//cout << left << setw(20);
				for (int t = 0;t < count;t++)
					string_temp+= trans_input[t];
				cout << left << setw(20)<<string_temp;
				string_temp.clear();
				for (auto t = my_stack.rbegin();t != my_stack.rend();t++)
					string_temp+=*t;
				cout << left << setw(20) << string_temp;
				string_temp.clear();
				for (int t = count;t < trans_input.size();t++)
					string_temp+=trans_input[t];
				cout << left << setw(20) << string_temp;
				string_temp.clear();

				cout << last_putout << endl;
				last_putout = "匹配 " + string{ i };
				//cout << "匹配 " << i << endl;
				break;
			}
			else {		//无法匹配时
				single_production temp = ana_list[non_terminator_list[my_stack.back()]][terminator_list[string{ i }]];

				if (temp.left == "0") {	//表中为空白
					flag = 0;
					false_location.push_back(count);
					break;
					//return 0;
				}
				else {
					string string_temp;
					for (int t = 0;t < count;t++)
						string_temp+= trans_input[t];
					cout << left << setw(20) << string_temp;
					string_temp.clear();
					for (auto t = my_stack.rbegin();t != my_stack.rend();t++)
						string_temp+=*t;
					cout << left << setw(20) << string_temp;
					string_temp.clear();
					for (int t = count;t < trans_input.size();t++)
						string_temp+= trans_input[t];
					cout << left << setw(20) << string_temp;
					string_temp.clear();

					cout << last_putout<<endl;
					last_putout = "输出 " + temp.left + " -> ";
					//cout << "输出 " << temp.left << " -> ";
					for (const auto& t : temp.right)
						last_putout+= t;
					
				}
				//将产生式右部加入stack
				my_stack.pop_back();
				for (auto t = temp.right.rbegin();t != temp.right.rend();t++) {
					my_stack.push_back(*t);
				}
			}
		}
	}
	return 0;
}

void syntax_ana::syn_show() {
	for (const auto& i : non_terminator) {
		cout << "First " << i << " : ";
		auto temp = First(i);
		for (const auto& j : temp)
			cout << j;
		cout << endl;

		temp = Follow(i);
		cout << "Follow " << i << " : ";
		for (const auto& j : temp)
			cout << j;
		cout << endl<<endl;
	}

	for (int i = 0;i < single_LL_1.size();i++) {
		cout << "Select "<< single_LL_1[i].left <<" -> ";
		for (const auto& j : single_LL_1[i].right)
			cout << j;
		cout << " : ";
		for (auto j = select[i].begin();j != select[i].end();j++) {
			cout << *j;
		}
		cout << endl;
	}
}

//void syntax_ana::to_expression(vector<vector<string>> input) {
//	for (const auto& i : input) {
//		string temp;
//		for (const auto& j : i) {
//			temp += j;
//		}
//	}
//}

void syntax_ana::start_ana() {
	for (const auto& i : terminator)
		my_first.insert(pair<string,set<string>>(i,First(i)));
	for (const auto& i : terminator)
		my_first.insert(pair<string, set<string>>(i, First(i)));
	for (const auto& i : non_terminator)
		my_follow.insert(pair<string, set<string>>(i, Follow(i)));
	for (const auto& i : non_terminator)
		my_follow.insert(pair<string, set<string>>(i, Follow(i)));


	for (const auto& i : LL_1) {
		Select(i);
	}
	
	syn_show();

	//map填充
	for (auto i = terminator.begin();i != terminator.end();i++) {
		terminator_list.insert(pair<string,int>(*i,i-terminator.begin()));
	}
	for (auto i = non_terminator.begin();i != non_terminator.end();i++) {
		non_terminator_list.insert(pair<string, int>(*i, i - non_terminator.begin()));
	}

	//分析表空间分配
	ana_list.resize(non_terminator.size());
	for (auto& i : ana_list) {
		i.resize(terminator.size());
	}

	//分析表构建
	for (auto i = select.begin();i != select.end();i++) {
		auto temp = single_LL_1[i - select.begin()];
		for (const auto& j : *i) {
			if (j != SYN_BLANK) {
				ana_list[non_terminator_list[temp.left]][terminator_list[j]] = temp;
			}
		}
	}

	//cout << is_legal({ "(","(","(","(","1","+","1" ,")","*","999",")" ,"/","92299",")" ,"-","22319",")" });
	cout << endl << endl;
	for (const auto& i : expression) {
		cout << "表达式: ";
		for (const auto& j : i) {
			cout << j << " ";
		}
		bool flag = is_legal(i);
		cout << (flag ?"表达式合法":"表达式非法");
		if (flag == 0) {
			cout << "  错误的单词： ";
			for (const auto& t : false_location)
				cout <<t+1<<" "<< i[t] << ",";
		}
		cout << endl<< "-------------------------------------------------------------------------------";
		cout << endl;
	}
}


#include "SLR1.h"

int to_int(string input) {
	int t = 0;
	for (auto i = input.begin();i != input.end();i++) {
		t = t * 10 + (*i-'0');
	}
	return t;
}


SLR::SLR() {

}

SLR::~SLR() {

}

const production& SLR::product(string input) {
	for (const auto& i : SLR_1) {
		if (i.left == input)
			return i;
	}
}

set<string> SLR::First(string input) {
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
				if (i.front()==input) {		//防止死循环
					continue;
				}
				auto temp_putout = First(i.front());
				putout.insert(temp_putout.begin(), temp_putout.end());
			}
			return putout;
		}
		for (auto i = temp.front().begin();i != temp.front().end();i++) {	// *i是一个string
			set<string> temp2 = First(*i);
			if (is_in(temp2, SYN_BLANK)) {
				if (i != temp.front().end() - 1)
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

set<string> SLR::Follow(string input) {
	if (my_follow.count(input))
		return my_follow[input];

	set<string> putout;

	if (input == start_state) {
		putout.insert("$");
	}
	for (const auto& a : SLR_1) {	//每一条产生式
		for (const auto& i : a.right) {		//i为单独的产生式右部
			if (is_in(i, input)) {
				for (auto j = i.begin();j != i.end();j++) {
					if (*j == input) {		//非终结符
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
						else if (j == i.end() - 1) {
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

int SLR::is_toBlank(string input) {		//return    0不会产生BLANK,1只会产生BLANK,2两个都会产生
	if (is_in(terminator, input))
		return 0;
	auto temp = product(input).right;

	bool can_to = 0;
	bool cant_to = 0;
	for (const auto& i : temp) {	//i为唯一的右部
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


/************SLR_1*****************/
vector<SLR_production> SLR::SLR_Find(string input) {
	vector<SLR_production> output;
	for (const auto& i : SLR_Data) {
		if (i.left == input)
			output.push_back(i);
	}
	return output;
}


set<SLR_production> SLR::closure(SLR_production input) {
	set<SLR_production> output;
	vector<SLR_production> open, close;		//open表和close表
	open.push_back(input);
	while (!open.empty()) {
		auto temp = open.back();
		open.pop_back();
		if (is_in(close, temp))
			continue;
		output.insert(temp);
		if (temp.point + 1 >= temp.right.size() || is_in(terminator, temp.right[temp.point + 1]))	//是终结符或者已经移进到最后
			continue;
		else {
			close.push_back(temp);
			auto temp1{SLR_Find(temp.right[temp.point+1])};
			for (const auto& i : temp1) {
				open.push_back(i);
			}
		}
	}
	return output;
}

set<SLR_production> SLR::Go_to(set<SLR_production> start, string input) {
	set<SLR_production> putout;
	for (const auto& i : start) {
		if (i.point + 1 < i.right.size() && i.right[i.point + 1] == input) {
			SLR_production temp{ i };
			temp.point++;
			auto temp_putout{closure(temp)};
			putout.insert(temp_putout.begin(), temp_putout.end());
		}
	}
	return putout;
}

void SLR::create_family() {
	auto put1 = closure(SLR_Data.front());
	SLR_Family.push_back(put1);
	SLR_Family_Map.insert(pair<set<SLR_production>, int >(put1,0));
	for(int count = 0;;count++){
		if (count >= SLR_Family.size())
			break;
		auto now = SLR_Family[count];
		set<SLR_production> data_list(now) ;
		set<string> word;	//待移进的字符
		for (const auto& i : data_list) {
			if (i.point + 1 < i.right.size()) {
				word.insert(i.right[i.point + 1]);
			}
		}
		for (const auto& i : word) {
			//for (const auto& j : data_list) {
				//if (j.point + 1 >= j.right.size())
				//	continue;
				auto temp = Go_to(data_list, i);		//获得新的节点
				if (temp.empty())
					continue;
				if (SLR_Family_Map.count(temp)) {	//该结点在表中
					int start = SLR_Family_Map[data_list];
					int end = SLR_Family_Map[temp];
					SLR_Family_To[start].insert(pair<string, int>(i,end));
				}
				else {
					SLR_Family.push_back(temp);
					SLR_Family_To.resize(SLR_Family.size());
					SLR_Family_Map.insert(pair<set<SLR_production>, int >(temp, SLR_Family.size()-1));

					int start = SLR_Family_Map[data_list];
					int end = SLR_Family_Map[temp];
					SLR_Family_To[start].insert(pair<string, int>(i, end));
				}
			//}
		}
	}

}

void SLR::creat_ana_list() {
	for (const auto& i : terminator)
		symbol_map.insert(pair<string,int>(i,symbol_map.size()));
	for (const auto& i : non_terminator)
		if(i!="S'")
			symbol_map.insert(pair<string, int>(i, symbol_map.size()));

	ana_list.resize(SLR_Family.size());
	int my_size = terminator.size() + non_terminator.size() - 1;
	for (auto& i : ana_list)
		i.resize(my_size);
	for (auto& i : ana_list) {	//初始化
		for (auto& j : i) {
			j.first = 0;
			j.second = -1;
		}
	}


	for (auto i = SLR_Family_To.begin();i != SLR_Family_To.end();i++) {
		// *i为转换关系map  map<string,int>
		for(const auto& j : *i){
			
			if (is_in(terminator, j.first)) {	//action
				pair<char, int>& temp = ana_list[i-SLR_Family_To.begin()][symbol_map[j.first]];
				temp.first = 's';
				temp.second = j.second;
			}
			else if (is_in(non_terminator, j.first)) {
				pair<char, int>& temp = ana_list[i - SLR_Family_To.begin()][symbol_map[j.first]];
				temp.first =' ';
				temp.second = j.second;
			}
		}
	}

	//follow集的计算等
	for (const auto& i : non_terminator) {		//数据填充
		production temp;
		temp.left = i;
		for (const auto& j : SLR_Data) {
			if (j.left == i) {
				temp.right.push_back(j.right);
			}
		}
		SLR_1.push_back(temp);
	}

	//计算first和follow集
	for (const auto& i : terminator)
		my_first.insert(pair<string, set<string>>(i, First(i)));
	for (const auto& i : non_terminator)
		my_first.insert(pair<string, set<string>>(i, First(i)));
	for (const auto& i : terminator)
		my_follow.insert(pair<string, set<string>>(i, Follow(i)));
	for (const auto& i : non_terminator)
		my_follow.insert(pair<string, set<string>>(i, Follow(i)));

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
		cout << endl << endl;
	}

	//map填充
	for (auto i = terminator.begin();i != terminator.end();i++) {
		terminator_list.insert(pair<string, int>(*i, i - terminator.begin()));
	}
	for (auto i = non_terminator.begin();i != non_terminator.end();i++) {
		non_terminator_list.insert(pair<string, int>(*i, i - non_terminator.begin()));
	}

	


	for (auto i = SLR_Family.begin();i != SLR_Family.end();i++) {
		//test1
		if (i->count({ "S'",{"S"},0 })) {
			cout << endl;
			pair<char, int>& temp = ana_list[i - SLR_Family.begin()][symbol_map["$"]];
			temp.first = 'A';
			temp.second = 0;
		}
		//test2
		SLR_production temp;
		int num = 1;
		bool flag = 0;
		for (SLR_production t : SLR_Data) {
			t.point = t.right.size() - 1;
			//bool non_terminator_flag = 0;	//是否包含非终结符
			//for (const auto& j : t.right) {
			//	if (is_in(non_terminator, j)) {
			//		non_terminator_flag = 1;
			//		break;
			//	}

			//}
			//if (non_terminator_flag) {
			//	num++;
			//	continue;
			//}

			if (i->count(t)) {
				temp = t;
				flag = 1;
				break;
			}
			num++;
		}
		if (flag) {
			auto temp_list(Follow(SLR_Data[num-1].left));
			for (const auto&t:temp_list) {
				pair<char, int>& temp = ana_list[SLR_Family_Map[*i]][terminator_list[t]];
				if (temp.second != -1)
					continue;
				temp.first = 'r';
				temp.second = num;
			}
		}	
	}

	//打印分析表

	/*********第一行**********/
	cout << left << setw(5) << "";
	cout << "|";

	int all_len = terminator.size() * 6 - 1;
	cout << left << setw((all_len-1)/2-2) << "";
	cout << left << setw((all_len - 1) / 2+3) << "action";
	cout << "|";
	all_len = (non_terminator.size()-1) * 6 - 1;
	cout << left << setw((all_len-1)/2-2) << "";
	cout << left << setw((all_len - 1) / 2 +3 ) << "goto";
	cout << "|";
	cout << endl;
	for (int t = 0;t < 6 * (ana_list[0].size() + 1);t++) {
		if (t != 1 && (t + 1) % 6 == 0)
			cout << "+";
		else
			cout << "-";
	}
	cout << endl;
	/*************************/
	cout << left << setw(5) << "";
	cout << "|";
	for (const auto& i : terminator) {
		cout << left << setw(5) << "  "+i;
		cout << "|";
	}
	for (const auto& i : non_terminator) {
		if (i != "S'") {
			cout << left << setw(5) << "  " + i;
			cout << "|";
		}
	}
	cout << endl;
	for (int t = 0;t < 6 * (ana_list[0].size() + 1);t++) {
		if (t != 1 && (t + 1) % 6 == 0)
			cout << "+";
		else
			cout << "-";
	}
	cout << endl;
	for (auto i = ana_list.begin();i != ana_list.end();i++) {
		cout << left << setw(5) << i - ana_list.begin();
		cout << "|";
		for (const auto& j : *i) {
			if (j.second == -1)
				cout << left << setw(5) << "";
			else if (j.first == 'A')
				cout << left << setw(5) << "ACC";
			else
				cout << left << setw(5) << string{ j.first } + to_string( j.second );
			cout << "|";
		}
		cout << endl;
		for (int t = 0;t < 6 * (ana_list[0].size() + 1);t++) {
			if (t!=1 && (t+1) % 6 == 0)
				cout << "+";
			else
				cout << "-";
		}
			
		cout << endl;
	}

	//打印项目集
	int n = 0;
	int print_num = 0;
	int last_flag=0;
	vector<string> print_out;
	print_out.resize(15);
	cout << endl;
	for (const auto& j : SLR_Family) {
		int max = 10;
		last_flag++;
		print_out[0] += "Item " + to_string(n++) + " :";
		while (print_out[0].size() < (print_num + 1) * 25)
			print_out[0] += " ";
		//cout << "Item " << n++ << " :" << endl;
		int my_count = 1;
		for (const auto& i : j) {
			//cout << print_out[my_count].size() << endl;
			print_out[my_count] += i.left + " -> ";
			//cout << i.left;
			//cout << " -> ";
			for (auto j = i.right.begin();j != i.right.end();j++) {
				if (j - i.right.begin() == i.point + 1)
					print_out[my_count] += "·";
					//cout << "·";
				print_out[my_count] += *j;
				//cout << *j;
			}
			if (i.point + 1 == i.right.size())
				print_out[my_count] += "·";
				//cout << "·";
			//cout << endl;
			
			my_count++;
		}
		if (my_count > max)
			max = my_count;
		for (int tt = 1;tt < 15;tt++) {
			//cout << print_out[tt].size() <<"  ";

			while (print_out[tt].size() < (print_num + 1) * 25)
				print_out[tt] += " ";

			//cout << print_out[tt].size() << endl;
		}
		print_num++;
		if (print_num == 5 || last_flag== SLR_Family.size()) {
			print_num = 0;
			int count = 0;
			for ( auto& x : print_out) {
				if(count<max+2)
					cout << x << endl;
				x.clear();
				count++;
			}
				

		}
		
	}
	cout << endl << endl;
	for (int tt = 0;tt < 100;tt++) {
		cout << "-";
	}
	
}

bool SLR::is_legal(vector<string> input) {
	false_location.clear();
	cout << endl << "表达式 : ";
	string trans_input;		//输入串
	for (const auto& i : input) {
		cout << i;
		if (is_in(terminator, i))		//运算符号
			trans_input.push_back(i.front());
		else {		//数字
			trans_input.push_back('i');
		}
	}
	cout << endl;
	trans_input.push_back('$');
	//开始分析
	vector<string> my_stack{"0" };	//栈
	false_location.clear();
	cout << endl;
	cout << left << setw(20) << "步骤";
	cout << left << setw(20) << "栈";
	cout << left << setw(20) << "输入串";
	cout << left << setw(20) << "action";
	cout << left << setw(20) << "goto" << endl;
	bool flag = 0;	//是否错误
	int count = 1;
	for (auto i = trans_input.begin();i != trans_input.end();) {
		cout << left << setw(20) << count;
		string print_out;

		for (auto j = my_stack.begin();j != my_stack.end();j++)	//栈
			print_out += *j;
		cout << left << setw(20) << print_out;

		print_out.clear();
		for (auto j = i;j != trans_input.end();j++)	//输入串
			print_out += *j;
		cout << left << setw(20) << print_out;

		pair<char, int> temp{ ana_list[to_int(my_stack.back())][terminator_list[string{*i}]] };
		if (temp.first == 'A') {
			cout << left << setw(20) << "ACC" << endl<<endl;
			if (!flag) {	//分析正确
				cout <<"表达式 : "<< string(trans_input.begin(), trans_input.end() - 1) << "  合法" << endl;
			}
			else {
				cout << "表达式 : " << string(trans_input.begin(), trans_input.end() - 1) << "  非法  错误位置: ";
				for (const auto& t : false_location) {
					cout << "[" <<trans_input[t]<<" at "<< t+1 <<" ] ";
				}
				cout << endl;
			}
			for (int tt = 0;tt < 100;tt++) {
				cout << "-";
			}
			cout << endl << endl << endl;
			return 1;
		}
		else if (temp.first == 's') {
			print_out = "s" + to_string(temp.second);
			cout << left << setw(20) << print_out << endl;
			my_stack.push_back(string{ *i });
			my_stack.push_back(to_string(temp.second));
			count++;
			i++;
			continue;
		}
		else if (temp.first == 'r') {
			print_out = "r" + to_string(temp.second);
			cout << left << setw(20) << print_out;
			SLR_production my_production = SLR_Data[temp.second - 1];
			for (int t = 0;t < my_production.right.size();t++) {
				my_stack.pop_back();
				my_stack.pop_back();
			}
			string temp = my_stack.back();
			my_stack.push_back(my_production.left);
			pair<char, int> temp1 = ana_list[to_int(temp)][symbol_map[my_production.left]];
			my_stack.push_back(to_string(temp1.second));
			cout << left << setw(20) << temp1.second<<endl;
		}
		else {		//出错
			flag = 1;
			if (*i == '$')
				break;
			cout  << "符号 [ " << *i<< " ] 分析错误！"<<endl;
			false_location.push_back(i-trans_input.begin());
			i++;
		}
		count++;
	}
	cout <<endl <<endl<< "表达式 : " << string(trans_input.begin(),trans_input.end()-1) << "  非法  错误位置: ";
	for (const auto& t : false_location) {
		cout << "[" << trans_input[t] << " at " << t + 1 << " ] ";
	}
	cout << endl;

	for (int tt = 0;tt < 100;tt++) {
		cout << "-";
	}
	cout << endl << endl << endl;
}
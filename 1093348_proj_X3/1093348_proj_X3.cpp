#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include <queue>
#include <tuple>
#include <iomanip>
using namespace std;

struct Instruct {
	string opperator;
	vector<string> reg;
	int immediate;
	bool imm = 0;

};

struct Buffer {
	string opperator,rf;
	int v1, v2, v3;
	int source;
	int rescycle;
	bool empty = 1;
	int retrunrf;
};
struct RS {
	string opperator="";
	int v1,v2;
	bool bv1 = 0, bv2 = 0;
	int retrunrf;
	string q1, q2;
	bool busy = 0, disp = 0;
};
queue<Instruct> inst;
vector<string> strinst;
vector<int> RF = {0,0,2,4,6,8};
vector<string> RAT = {"","F1","F2", "F3", "F4", "F5"};
vector<RS> addRS;
vector<RS> multRS;
Buffer addBuffer;
Buffer multBuffer;
bool change = 0;
void read() {
	ifstream in("test.txt");
	if (!in.is_open()) {
		cout << "can't open the file.";
		exit(0);
	}
	string str;
	while (getline(in, str)) {

		Instruct tempinst;
		strinst.push_back(str);
		
		int tr = -1, i = 0;
		string tstr;
		while (i < str.size()) {
			while (i < str.size())
			{
				tstr.push_back(str[i]);
				i++;
				if (str[i] == ' ' || str[i] == ',')
					break;

			}
			if (tr == -1) {
				tempinst.opperator = tstr;
				tr++;
			}
			else {
				if(tstr[0] == 'F')
					tempinst.reg.push_back(tstr);
				else {
					tempinst.immediate = tstr[0] - '0';
					tempinst.imm = 1;
				}
					
			}
			tstr.clear();
			i++;
		}
		inst.push(tempinst);
	}
}

void issue() {
	if (inst.empty())
		return;
	Instruct t;
	t = inst.front();
	
	if(t.opperator == "ADDI" || t.opperator == "ADD"|| t.opperator == "SUB")
		for (int i = 0; i < 3; i++) {
			if (!addRS[i].busy)
			{
				change = 1;
				addRS[i].busy = 1;
				addRS[i].opperator = t.opperator;
				
				if (RAT[t.reg[1][1] - '0'][0] == 'F') {
					addRS[i].v1 = RF[t.reg[1][1] - '0'];
					addRS[i].bv1 = 1;
				}	
				else
					addRS[i].q1 = RAT[t.reg[1][1] - '0'];
				
				if (t.imm == 1) {
					addRS[i].v2 = t.immediate;
					addRS[i].bv2 = 1;
				}else {
					if (RAT[t.reg[2][1] - '0'][0] == 'F') {
						addRS[i].v2 = RF[t.reg[2][1] - '0'];
						addRS[i].bv2 = 1;
					}	
					else
						addRS[i].q2 = RAT[t.reg[2][1] - '0'];
				}
				char c = '1' + i;
				RAT[t.reg[0][1] - '0'] = "R";
				RAT[t.reg[0][1] - '0'].push_back(c);
				addRS[i].retrunrf = t.reg[0][1] - '0';
				inst.pop();
				break;
			}
		}
	if (t.opperator == "MUL" || t.opperator == "DIV")
		for (int i = 0; i < 2; i++) {
			if (!multRS[i].busy)
			{
				change = 1;
				multRS[i].busy = 1;
				multRS[i].opperator = t.opperator;
				
				if (RAT[t.reg[1][1] - '0'][0] == 'F') {
					multRS[i].v1 = RF[t.reg[1][1] - '0'];
					multRS[i].bv1 = 1;
				}
				else
					multRS[i].q1 = RAT[t.reg[1][1] - '0'];

				if (t.imm == 1) {
					multRS[i].v2 = t.immediate;
					multRS[i].bv2 = 1;
				}
				else {
					if (RAT[t.reg[2][1] - '0'][0] == 'F') {
						multRS[i].v2 = RF[t.reg[2][1] - '0'];
						multRS[i].bv2 = 1;
					}
					else
						multRS[i].q2 = RAT[t.reg[2][1] - '0'];
				}
				char c = '1' + i + 3;
				RAT[t.reg[0][1] - '0'] = "R";
				RAT[t.reg[0][1] - '0'].push_back(c);
				multRS[i].retrunrf = t.reg[0][1] - '0';
				inst.pop();
				break;
			}
		}
}

void dispatch() {
	if (addBuffer.empty == 1) {
		for (int i = 0; i < 3; i++) {
			if (addRS[i].bv1 == 1 && addRS[i].bv2 == 1) {
				change = 1;
				addBuffer.empty = 0;
				char c = '1' + i;
				addBuffer.rf = "R";
				addBuffer.rf.push_back(c);
				addBuffer.source = i;
				addBuffer.opperator = addRS[i].opperator;
				addBuffer.v2 = addRS[i].v1;
				addBuffer.v3 = addRS[i].v2;
				if (addBuffer.opperator == "ADDI" || addBuffer.opperator == "ADD") {
					addBuffer.v1 = addBuffer.v2 + addBuffer.v3;
					addBuffer.rescycle = 1;
				}else if (addBuffer.opperator == "SUB" ) {
					addBuffer.v1 = addBuffer.v2 - addBuffer.v3;
					addBuffer.rescycle = 1;
				}
				addBuffer.retrunrf = addRS[i].retrunrf;
				break;
			}
		}
	}

	if (multBuffer.empty == 1) {
		for (int i = 0; i < 2; i++) {
			if (multRS[i].bv1 == 1 && multRS[i].bv2 == 1) {
				change = 1;
				multBuffer.empty = 0;
				char c = '1' + i+3;
				multBuffer.rf = "R";
				multBuffer.rf.push_back(c);
				multBuffer.source = i;
				multBuffer.opperator = multRS[i].opperator;
				multBuffer.v2 = multRS[i].v1;
				multBuffer.v3 = multRS[i].v2;
				if (multBuffer.opperator == "MUL") {
					multBuffer.v1 = multBuffer.v2 * multBuffer.v3;
					multBuffer.rescycle = 9;
				}
				else if (multBuffer.opperator == "DIV") {
					multBuffer.v1 = multBuffer.v2 / multBuffer.v3;
					multBuffer.rescycle = 39;
				}
				multBuffer.retrunrf = multRS[i].retrunrf;
				break;
			}
		}
	}
}

void doBuffer() {
	if (!addBuffer.empty) {
		if (addBuffer.rescycle != 0)
			addBuffer.rescycle--;
		else
		{
			change = 1;
			for (int i = 1; i <= 5; i++) {
				if (RAT[i] == addBuffer.rf)
				{
					char c = '1' + i;
					RAT[i] = "F";
					RAT[i].push_back(c);					
					break;
				}
			}
			RF[addBuffer.retrunrf] = addBuffer.v1;
			for (int i = 0; i < 3; i++) {
				if (addRS[i].q1 == addBuffer.rf) {
					addRS[i].bv1 = 1;
					addRS[i].v1 = addBuffer.v1;
				}
				if (addRS[i].q2 == addBuffer.rf) {
					addRS[i].bv2 = 1;
					addRS[i].v2 = addBuffer.v1;
				}
			}
			for (int i = 0; i < 2; i++) {
				if (multRS[i].q1 == addBuffer.rf) {
					multRS[i].bv1 = 1;
					multRS[i].v1 = addBuffer.v1;
				}
				if (multRS[i].q2 == addBuffer.rf) {
					multRS[i].bv2 = 1;
					multRS[i].v2 = addBuffer.v1;
				}
			}
			RS t;
			addRS[addBuffer.source] = t;
			Buffer t1;
			addBuffer = t1;
			
		}
	}
	
	if (!multBuffer.empty) {
		if (multBuffer.rescycle != 0)
			multBuffer.rescycle--;
		else
		{
			change = 1;
			for (int i = 1; i <= 5; i++) {
				if (RAT[i] == multBuffer.rf)
				{
					char c = '1' + i;
					RAT[i] = "F";
					RAT[i].push_back(c);
					
					break;
				}
			}
			RF[multBuffer.retrunrf] = multBuffer.v1;
			for (int i = 0; i < 3; i++) {
				if (addRS[i].q1 == multBuffer.rf) {
					addRS[i].bv1 = 1;
					addRS[i].v1 = multBuffer.v1;
				}
				if (addRS[i].q2 == multBuffer.rf) {
					addRS[i].bv2 = 1;
					addRS[i].v2 = multBuffer.v1;
				}
			}
			for (int i = 0; i < 2; i++) {
				if (multRS[i].q1 == multBuffer.rf) {
					multRS[i].bv1 = 1;
					multRS[i].v1 = multBuffer.v1;
				}
				if (multRS[i].q2 == multBuffer.rf) {
					multRS[i].bv2 = 1;
					multRS[i].v2 = multBuffer.v1;
				}
			}
			RS t;
			multRS[multBuffer.source] = t;
			Buffer t1;
			multBuffer = t1;
		}
	}
	
}

int main() {
	read();

	//ªì©l¤ÆRS
	for (int i = 0; i < 3; i++) {
		RS t;
		addRS.push_back(t);
	}
	for (int i = 0; i < 2; i++) {
		RS t;
		multRS.push_back(t);
	}

	int cycle = 1;
	while (1) {
		change = 0;
		doBuffer();
		dispatch();
		issue();

		if (change == 1) {
			cout << "cycle: " << cycle << endl;
			cout << "\n    __RF__\n";
			for (int i = 1; i < 6; i++) {
				cout << "F" <<i <<" |" << setw(5) << RF[i] << " |" << endl;
			}
			cout << "   -------\n";
			cout << "\n   __RAT__\n";
			for (int i = 1; i < 6; i++) {
				if (RAT[i][0] == 'R')
					cout << "F"<< i << " |" << setw(3) << RAT[i][0] <<"S"<< RAT[i][1] << " |" << endl;
				else
					cout << "F" << i << " |      |" << endl;
			}
			cout << "   -------\n";
			cout << "\n\n    ___RS_______\n";
			for (int i = 0; i < 3; i++) {
				cout << "RS" << i + 1 << " |";
				if (addRS[i].busy == 1) {
					char c;
					if (addRS[i].opperator == "ADDI" || addRS[i].opperator == "ADD")
						c = '+';
					if (addRS[i].opperator == "SUB")
						c = '-';
					cout << c << " |";
					if (addRS[i].bv1 == 1)
						cout <<" " << setw(2) << addRS[i].v1 << " | ";
					else
						cout << " " << addRS[i].q1[0] << "S" << addRS[i].q1[1] << " |";

					if (addRS[i].bv2 == 1)
						cout << setw(2) << addRS[i].v2 << " ";
					else
						cout  << addRS[i].q2[0] << "S" << addRS[i].q2[1] ;
				}
				else
					cout << "  |    |    ";
				cout <<"|" << endl;
			}
			cout << "    -------------\n";
			cout << "buffer:";
			if (!addBuffer.empty) {
				char c;
				if (addBuffer.opperator == "ADDI" || addBuffer.opperator == "ADD")
					c = '+';
				if (addBuffer.opperator == "SUB")
					c = '-';

				cout << "(" << addBuffer.rf << ") " << addBuffer.v2 << " " << c << " " << addBuffer.v3 << endl << endl;
			}
			else
				cout << "empty\n\n";
			cout << "\n    ___RS_______\n";
			for (int i = 0; i < 2; i++) {
				cout << "RS" << i + 4 << " |";
				if (multRS[i].busy == 1) {
					char c;
					if (multRS[i].opperator == "MUL")
						c = '*';
					if (multRS[i].opperator == "DIV")
						c = '/';
					cout << c << " |";
					if (multRS[i].bv1 == 1)
						cout << " " <<setw(2)<< multRS[i].v1 << " |";
					else
						cout << " " << multRS[i].q1[0] << "S" << multRS[i].q1[1] << " |";
						

					if (multRS[i].bv2 == 1)
						cout << setw(2) << multRS[i].v2 << " ";
					else
						cout << multRS[i].q2[0] << "S" << multRS[i].q2[1];
						
				}else
					cout << "  |    |   ";
				cout << "|" << endl;
			}
			cout << "    -------------\n";
			cout << "buffer:";
			if (!multBuffer.empty) {
				char c;
				if (multBuffer.opperator == "MUL")
					c = '*';
				if (multBuffer.opperator == "DIV")
					c = '/';

				cout << "(" << multBuffer.rf << ") " << multBuffer.v2 << " " << c << " " << multBuffer.v3 << endl;

			}
			else
				cout << "empty\n";
			cout << "-----------------------------------------------------------------------------------\n";
		}
		
		cycle++;
		int exit = 5;
		for (int i = 0; i < 3; i++) {
			if (addRS[i].busy == 0)
				exit--;
		}

		for (int i = 0; i < 2; i++) {
			if (multRS[i].busy == 0)
				exit--;
		}
		if (exit == 0 && inst.size()== 0)
			return 0;
	}

}

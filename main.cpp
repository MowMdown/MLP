//Ryan Wallace
//Design Project
//CSC 215
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <sstream>
#include <Windows.h>
using namespace std;

const static int V_MEM = 16, H_MEM = 16;

void write_mem(istream &_input, string _mem[][V_MEM]);
void display_mem(ostream &_output, string _mem[][V_MEM], string _reg[]);
void run_ml_c0de(string _mem[][V_MEM]);
void op_code(string _mem[][V_MEM], string &_PC, string &_IR);
void instruction(string _mem[][V_MEM], string &_IR, string _reg[], string &_PC);
void convert(string &hex);
void intToHex(unsigned long long int _left, unsigned long long int _right, string &_final);

int main()
{
	char keystroke;
	static string MEMORY[H_MEM][V_MEM];

	cout << "Do you wish to load \"mlprog.txt\" ? Y / N : ";
	cin >> keystroke;
	system("cls");

	if (keystroke == 'y' || keystroke == 'Y')
	{
		try 
		{
			ifstream input; 
			input.open("mlprog.txt");
			if (!input) 
			{
				string e = "mlprog.txt Not found";
				throw e;
			}//if

			write_mem(input, MEMORY);

		}//try
		catch (string e) 
		{
			cout << "Exception Thrown: " << e << endl;
			system("pause");
			return 1;
		}//catch
	}//if
	else 
	{
		cout << "Enter your ML Program Using 2Byte Hex values with spaces in between\n"
			"When you are ready to end your code, use a '00' value!\n"
			"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --\n\r";
		write_mem(cin, MEMORY);
	}//else

	run_ml_c0de(MEMORY);

	return 0;
}//main

void write_mem(istream &_input, string _mem[][V_MEM])
{
	for (int h = 0; h < H_MEM; h++) 
	{
		for (int v = 0; v < V_MEM; v++)
		{
			_input >> _mem[h][v];

			if(_mem[h][v] == "00")
			{
				for (int x = h; x < H_MEM; x++)
				{
					for (int z = v ; z < V_MEM; z++)
					{
						_mem[x][z] = "00";
					}//for

					v = 0;
				}//for

				return;
			}//if
		}//for
	}//for

	return;
}

void display_mem(ostream &_output, string _mem[][V_MEM], string _reg[])
{
	cout << "0x   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n";
	for (int h = 0; h < H_MEM; h++)
	{
		cout << uppercase << hex << h << "|  ";

		for (int v = 0; v < V_MEM; v++)
		{	
			cout << fixed << hex << _mem[h][v] << "  ";
		}//for

		cout << setw(1) << right << "R" << h << "= " << left << _reg[h] << endl;
	}//for
}//read_mem

void run_ml_c0de(string _mem[][V_MEM])
{
	bool init_pc = false;
	bool step_pc = false;
	string PC = "";
	string IR = "";
	string sout;
	char step;
	long long val;
	string REGISTER[16];

	for (int i = 0; i < 16; i++)
	{
		REGISTER[i] == "00";
	}

	display_mem(cout, _mem, REGISTER);

	do {
		if (!init_pc)
		{
			cout << "\nTurn on Step Thru? Y / N : ";
			cin >> step;
			if (step == 'y' || step == 'Y')
				step_pc = true;
			cout << "\nSet PC= "; 
			cin >> PC;
			init_pc = true;
			step = ' ';
		}//if

		system("cls");

		op_code(_mem, PC, IR);
		instruction(_mem, IR, REGISTER, PC);
		display_mem(cout, _mem, REGISTER);	

		sout = REGISTER[15];
		val = strtoul(sout.c_str(), NULL, 16);
		step = val - 0x100;
		cout << "\nIR= " << IR << "  PC= " << PC  << "  Screen= " << step << endl << endl;
		
		if (step_pc)
		{
			system("pause");
		}
		else
		{
			Sleep(1000);
		}

	} while (IR != "0000" && IR != "Halt");

	if (IR == "Halt" || IR == "C000")
		cout << "\nProgram has reached HALT\n" << endl;
	else
		cout << "\nProgram has ran out of valid instructions\n" << endl;
	system("pause");
}

void op_code(string _mem[][V_MEM], string &_PC, string &_IR)
{
	string op_code_left, op_code_right, pc_left, pc_right;
	string sub1, sub2, sub3, sub4;
	string e = "Index [Range] Out of Bounds!";

	sub1 = _PC.substr(0,1);
	sub2 = _PC.substr(1,1);
	int HORIZONTAL = strtoul(sub1.c_str(), NULL, 16);
	int VERTICAL = strtoul(sub2.c_str(), NULL, 16);
	int PC_LEFT = strtoul(sub1.c_str(), NULL, 16);
	int PC_RIGHT = strtoul(sub2.c_str(), NULL, 16);

	try
	{
		if (HORIZONTAL == 15 && VERTICAL + 1 >= 16)
		{
			_IR = "ERR";
			throw e;
		}
		op_code_left = _mem[HORIZONTAL][VERTICAL];
		if (VERTICAL + 1 == 16)
		{
			VERTICAL = 0;
			HORIZONTAL += 1;
		}
		else
		{
			VERTICAL += 1;
		}
		op_code_right = _mem[HORIZONTAL][VERTICAL];
		_IR = op_code_left + op_code_right;

		/**	If the high bit of the high byte is 6 we step once
		  *	since our opcode only needs 1 byte for its instruction 
		  */
		sub1 = op_code_left.substr(0,1); 
		if (PC_LEFT == 16 && PC_RIGHT + 2 >= 16)
		{
			_IR = "ERR";
			throw e;
		}
		if (PC_RIGHT + 2 > 15 || PC_RIGHT + 1 > 16)
		{
			PC_LEFT += 1;
			if(sub1 == "6") {
				PC_RIGHT -= 15;
			} else {
				PC_RIGHT -= 14;
			}
		} else {
			if (sub1 == "6") {
				PC_RIGHT += 1;
			} else {
				PC_RIGHT += 2;
			}
		}

		pc_left = static_cast<ostringstream*>( &(ostringstream() 
			<< (PC_LEFT)) )->str();
		pc_right  =  static_cast<ostringstream*>( &(ostringstream() 
			<< (PC_RIGHT)) )->str();

		convert(pc_left); convert(pc_right);

		_PC = pc_left + pc_right;

	}
	catch (string e)
	{
		cout << "Exception Thrown: " << e << endl;
		system("pause");
		return;
	}
}

void instruction(string _mem[][V_MEM], string &_IR, string _reg[], string &_PC)
{
	string first, second, third, fourth, input;
	char cinput;
	unsigned int ror, mask;

	//1<-operand 2<-Register [3, 4]
	first  = _IR.substr(0,1); //High bit / High byte
	unsigned int oprnd = strtoul(first.c_str(), NULL, 16);

	second = _IR.substr(1,1); //Low bit / High byte
	unsigned int rgstr = strtoul(second.c_str(), NULL, 16);

	third  = _IR.substr(2,1); //High bit / Low byte
	unsigned int left = strtoul(third.c_str(), NULL, 16);

	fourth = _IR.substr(3,1); //Low bit / low byte
	unsigned int right = strtoul(fourth.c_str(), NULL, 16);

	/* These are left and right values for registers */
    unsigned long long int r_left, r_right, final;
	r_left = strtoul(_reg[left].c_str(), NULL, 16);
	r_right = strtoul(_reg[right].c_str(), NULL, 16);

	switch(oprnd)
	{
	case 1:
		_reg[rgstr] = _mem[left][right];
		break;
	case 2:
		_reg[rgstr] = third + fourth; 
		break;
	case 3:
		_mem[left][right] = "";
		_mem[left][right] = _reg[rgstr + 1];
		break;
	case 4:
		_reg[right] = _reg[left];
		break;
	case 5:
		_reg[rgstr] = _reg[left] + _reg[right];
		break;
	case 6:
		cout << "\nEnter ASCII: ";
		cin >> cinput;								//Read in our ASCII char and then
		final = cinput;								//separate the left bit and right bit
		intToHex(final, final, input);				//and prepare them for hex conversion
		_reg[14] = input;							//before we store it into Register E 
		break;
	case 7:
		final = (r_left | r_right);
		intToHex(final, final, input);
		_reg[rgstr] = input;
		break;
	case 8:
		final = (r_left & r_right);
		intToHex(final, final, input);
		_reg[rgstr] = input;
		break;
	case 9:
		final = (r_left ^ r_right);
		intToHex(final, final, input);
		_reg[rgstr] = input;
		break;
	case 0xA:
		ror = strtoul(_reg[rgstr].c_str(), NULL, 16);
		mask = right;
		final = ror;
		ror <<= mask;
		final = (ror << (32 - mask));
		final = (ror | final);
		intToHex(final, final, input);
		_reg[rgstr] = input;
		break;
	case 0xB:
		r_left = strtoul(_reg[rgstr].c_str(), NULL, 16);
		r_right = strtoul(_reg[0].c_str(), NULL, 16);
		if (rgstr != 0 && (r_left == r_right))
			_PC = third + fourth;
		break;
	case 0xC:
		_IR = "Halt";
		_PC = "zz";
		break;
	case 0xD:
		_reg[rgstr] = _mem[r_left][r_right];
		break;
	case 0xE:
		input = _reg[right];
		first = input.substr(0,1);
		second = input.substr(1,1);
		r_left = strtoul(first.c_str(), NULL, 16);
		r_right = strtoul(second.c_str(), NULL, 16);
		_mem[r_left][r_right] = _reg[left];
		break;
	case 0xF:
		r_left = strtoul(_reg[rgstr].c_str(), NULL, 16);
		r_right = strtoul(_reg[0].c_str(), NULL, 16);
		if (r_left <= r_right)
			_PC = third + fourth;
		break;

	}
}

void convert(string &hex)
{
	if (hex == "10") {
		hex = "A";
	} else if (hex == "11") {
		hex = "B";
	} else if (hex == "12") {
		hex = "C";
	} else if (hex == "13") { 
		hex = "D";
	} else if (hex == "14") {
		hex = "E";
	} else if (hex == "15") {
		hex = "F";
	} else {
		return;
	}
}

void intToHex(unsigned long long int _left, unsigned long long int _right, string &_final)
{
	string left, right;
	_left /= 16;
	_right %= 16;
	left = to_string(_left);
	right = to_string(_right);
	convert(left); convert(right);
	_final = (left + right);
}

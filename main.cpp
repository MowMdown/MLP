/* Ryan Wallace
 * Design Project
 * CSC 215
 */
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

/* Sets our main memory size: 16(vertical) x 16(horizontal) */
const static int V_MEM = 16, H_MEM = 16;

/* Writes the hex values to the main memory */
void write_mem(istream &_input, string _mem[][V_MEM]);

/* Writes the 16 x 16 memory grid to the screen */
void display_mem(ostream &_output, string _mem[][V_MEM], string _reg[]);

/* Executes the ML code */
void run_ml_c0de(string _mem[][V_MEM]);

/* Separates the 4-bits into (OpCode, Register, X, Y) for Execution */
void op_code(string _mem[][V_MEM], string &_PC, string &_IR);

/* Uses the OpCode to manipulate data */
void instruction(string _mem[][V_MEM], string &_IR, string _reg[], string &_PC);

/* Converts to hex digits */
void convert(string &hex);

/* Takes an integer and converts it into its hex equivalent */
void intToHex(int _left, int _right, string &_final);

int main()
{
	char keystroke;
	static string MEMORY[H_MEM][V_MEM];

	cout << "Do you want to load \"mlprog.txt\" ? Y / N : ";
	cin >> keystroke;
	system("cls");

	try {
		if (keystroke == 'y' || keystroke == 'Y') {
			ifstream input;
			input.open("mlprog.txt");

			if (!input) {
				string e = "mlprog.txt Not found";
				throw e;
			}//if

			write_mem(input, MEMORY);			//Writes to memory from mlprog.txt file

		} else {
			cout << "Enter your ML Program Using 2Byte Hex values with spaces in between\n"
				"When you are ready to end your code, use a '00' value!\n"
				"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --\n";

			write_mem(cin, MEMORY);				//Writes to memory from keyboard input

			system("cls");
		}//if-else

		/* Executes all of the instructions programmed into memory */
		run_ml_c0de(MEMORY);

		/* Catch and Output any exceptions that are thrown */
	} catch (string e) {
		system("cls");
		cout << "Exception Thrown: " << e << endl;
		system("pause");
		return 1;
	}//catch

	return 0;
}//main

void write_mem(istream &_input, string _mem[][V_MEM])
{
	bool term = true;

	for (int h = 0; h < H_MEM; h++) {
		for (int v = 0; v < V_MEM; v++) {

			_input >> _mem[h][v];				//Read in a byte into the memory location

			if ((_mem[h][v] == "C0" || _mem[h][v] == "c0")	//If the byte is the terminating byte 
				|| !_input) {				//	or we run out of bytes fill the rest with 00's
					if (!_input) {													
						term = false;
					}//if

					for (int x = h; x < H_MEM; x++) {
						for (int z = v ; z < V_MEM; z++) {
							if (term) {
								_mem[x][z] = "C0";
								term = false;
							} else {
								_mem[x][z] = "00";
							}//if-else
						}//for

						v = 0;
					}//for

					return;
			}//if
		}//for
	}//for

	return;
}//write_mem

void display_mem(ostream &_output, string _mem[][V_MEM], string _reg[])
{
	cout << "0x  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n"
		"   -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --\n";

	for (int h = 0; h < H_MEM; h++) {
		cout << uppercase << hex << h << "| ";

		for (int v = 0; v < V_MEM; v++) {
			cout << fixed << hex << _mem[h][v] << " ";
		}//for

		cout << " " << setw(1) << right << "R" << h << "= " << left << _reg[h] << endl;
	}//for

	cout << "   -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --\n";
}//read_mem

void run_ml_c0de(string _mem[][V_MEM])
{
	bool init_pc = false;
	bool step_pc = false;
	string PC = "00";
	string IR = "0000";
	char keystroke;
	string sout, step, screen;
	long long val;
	enum REG {A = 10, B, C, D, E, F };
	string REGISTER[F + 1] = 
	      { "00", "00", "00", "00",
		"00", "00", "00", "00",
		"00", "00", "00", "00",
		"00", "00", "00", "00" };

	display_mem(cout, _mem, REGISTER);

	do {
		if (!init_pc) {
			cout << "\nTurn on Step Thru? Y / N : ";
			cin >> keystroke;

			if (keystroke == 'y' || keystroke == 'Y'){
				step_pc = true;
			}//if

			cout << "\nSet PC= ";
			cin >> PC;
			init_pc = true;
		}//if

		system("cls");

		op_code(_mem, PC, IR);					//(1)Fetch Instruction
		instruction(_mem, IR, REGISTER, PC);			//(2)Decode, (3)Execute Instruction
		display_mem(cout, _mem, REGISTER);			//Update Display

		if (REGISTER[F] == "FF") {				//Clear the screen if data is "FF"
			screen = "";
		} else if (REGISTER[F] != "00") {
			sout = REGISTER[F];				//Convert hex value
			val = strtoul(sout.c_str(), NULL, 16);		//	into an ASCII
			step = (val - 0x100);				//	character to be displayed
			screen += step;					//      on-screen in a string
		}//if-else

		cout << "\nIR= " << IR << " PC= " << PC << " Screen= " << screen << endl << endl;

		if (step_pc) {
			system("pause");
		} else {
			Sleep(1000);
		}//if-else

	} while (IR != "0000" && IR != "C000");

	if (IR == "C000") {
		cout << "Program has reached HALT\n" << endl;
	} else {
		cout << "Program has ran out of valid instructions\n" << endl;
	}//if-else

	system("pause");
}//run_ml_c0de

void op_code(string _mem[][V_MEM], string &_PC, string &_IR)
{
	string op_code_left, op_code_right, pc_left, pc_right;
	string sub1, sub2, sub3, sub4;
	string e = "Index [Range] Out of Bounds!";

	sub1 = _PC.substr(0,1);						//Separate the _PC into two halves
	sub2 = _PC.substr(1,1);						//sub1 is horizontal(x), sub2 is vertical(y)

	/*  Convert values from string to int data type
	*  Keeping the original values
	*/
	int HORIZONTAL = strtoul(sub1.c_str(), NULL, 16);
	int VERTICAL = strtoul(sub2.c_str(), NULL, 16);
	int PC_LEFT = strtoul(sub1.c_str(), NULL, 16);
	int PC_RIGHT = strtoul(sub2.c_str(), NULL, 16);

	if (HORIZONTAL >= 15 && VERTICAL >= 15) {			//If the X is at the last value in the row
		_IR = "ERR";						//	then we must move the right value to the 
		throw "IR: " + e;					//	start of the new row, if we reach the last
	}//if								//	row and the last column, throw and exception

	op_code_left = _mem[HORIZONTAL][VERTICAL];			//Update the high-end (left) byte

	if (VERTICAL + 1 > 15) {
		VERTICAL = 0;
		HORIZONTAL += 1;
	} else {
		VERTICAL += 1;
	}//if-else

	op_code_right = _mem[HORIZONTAL][VERTICAL];			//Update the low-end (right) byte
	_IR = op_code_left + op_code_right;				//Add the two bytes to form the Instruction

	/*  If the high bit of the high byte is "6" we step once
	*  since our opcode only needs 1 byte for its instruction
	*/
	sub1 = op_code_left.substr(0,1);

	if (PC_LEFT >= 15 && PC_RIGHT >= 15) {				//Same as the _IR we increment it and if it goes 
		_IR = "ERR";						//	past the last byte FF(255) we throw and exception
		throw "PC: " + e;
	}//if

	if (PC_RIGHT + 2 > 15 || PC_RIGHT + 1 > 15) {
		PC_LEFT += 1;
		if (sub1 == "6") {
			PC_RIGHT -= 15;
		} else {
			PC_RIGHT -= 14;
		}//if-else
	} else {
		if (sub1 == "6") {
			PC_RIGHT += 1;
		} else {
			PC_RIGHT += 2;
		}//if-else
	}//if-else

	pc_left = static_cast<ostringstream*>( &(ostringstream()	//Convert the high-end bit from an int to string
		<< (PC_LEFT)) )->str();
	pc_right = static_cast<ostringstream*>( &(ostringstream()	//Convert the low-end bit from an int to string
		<< (PC_RIGHT)) )->str();

	convert(pc_left); convert(pc_right);				//Convert them into the hex form

	_PC = pc_left + pc_right;					//Concatenate the two strings, which become the
}//op_code								//	new program counter

void instruction(string _mem[][V_MEM], string &_IR, string _reg[], string &_PC)
{
	string first, second, third, fourth, input;
	char cinput;
	unsigned int ror, mask;

	/* 1<-operand 2<-Register, 3<-X, 4<-Y */
	first = _IR.substr(0,1);					//High bit / High byte
	unsigned int oprnd = strtoul(first.c_str(), NULL, 16);		//Convert from string to integer

	second = _IR.substr(1,1);					//Low bit / High byte
	unsigned int rgstr = strtoul(second.c_str(), NULL, 16);		//Convert from string to integer

	third = _IR.substr(2,1);					//High bit / Low byte
	unsigned int left = strtoul(third.c_str(), NULL, 16);		//Convert from string to integer

	fourth = _IR.substr(3,1);					//Low bit / low byte
	unsigned int right = strtoul(fourth.c_str(), NULL, 16);		//Convert from string to integer

	/* These are left and right values for registers */
	unsigned int r_left, r_right, final;
	r_left = strtoul(_reg[left].c_str(), NULL, 16);			//Convert the data in _reg[X] and _reg[Y]  
	r_right = strtoul(_reg[right].c_str(), NULL, 16);		//	from string type to and integer type 

	/* The oprnd is the highest bit which is the instruction to preform */
	switch(oprnd) {
	case 1:
		_reg[rgstr] = _mem[left][right];			//Move the data from _mem[X][Y]
		break;//0x1						//	into the _reg[register]
	case 2:
		_reg[rgstr] = third + fourth;				//Store raw value XY into _reg[register]
		break;//0x2
	case 3:
		_mem[left][right] = "";					//Clear memory before we move new data
		_mem[left][right] = _reg[rgstr];			//Move data from _reg[register] into _mem[X][Y]
		break;//0x3
	case 4:
		_reg[right] = _reg[left];				//Move data from _reg[X] into _reg[Y]
		break;//0x4
	case 5:
		r_left += r_right;					//Add integer values from _reg[X] and
		intToHex(r_left, r_left, input);			//	_reg[Y] and then convert that into its
		_reg[rgstr] = input;					//	hex form before we store it into _reg[register]
		break;//0x5
	case 6:
		cout << "\nEnter ASCII: ";				//Read in our ASCII char from cin
		cin >> cinput;						//	and then convert that from a char to an int 
		final = cinput;						//	then we convert it into its hex value
		intToHex(final, final, input);				//	before we store it into Register E
		_reg[14] = input;										
		break;//0x6
	case 7:
		final = (r_left | r_right);				//Bitwise OR data from _reg[X] and _reg[Y]
		intToHex(final, final, input);				//Convert to hex value and store it
		_reg[rgstr] = input;					//	in the _reg[register]
		break;//0x7
	case 8:
		final = (r_left & r_right);				//Bitwise AND data from _reg[X] and _reg[Y]
		intToHex(final, final, input);				//Convert to hex value and store it
		_reg[rgstr] = input;					//	in the _reg[register]
		break;//0x8
	case 9:
		final = (r_left ^ r_right);				//Bitwise XOR data from _reg[X] and _reg[Y]
		intToHex(final, final, input);				//Convert to hex value and store it
		_reg[rgstr] = input;					//	in the _reg[register]
		break;//0x9
	case 0xA:
		ror = strtoul(_reg[rgstr].c_str(), NULL, 16);		//Bitwise Right Rotate the bits of the
		mask = right;						// 	_reg[Y], first convert it to an int
		final = ror;						// 	and then rotate the bits before 
		ror <<= mask;						// 	converting it back into a hex string
		final = (ror << (32 - mask));				// 	for storage in the _reg[register]
		final = (ror | final);
		intToHex(final, final, input);
		_reg[rgstr] = input;
		break;//0xA
	case 0xB:
		r_left = strtoul(_reg[rgstr].c_str(), NULL, 16);	//Get the data from _reg[register] and
		r_right = strtoul(_reg[0].c_str(), NULL, 16);		//	then get the data from _reg[0] to 
									//	compare to see if it is EQUAL TO
		if (second == "0") {					//	each other and then jump to raw XY
			_PC = third + fourth;				//If _reg[register] is "0" we just
		} else if (rgstr != 0 && (r_left == r_right)) {		// 	Jump to raw location XY regardless
			_PC = third + fourth;				// 	of _reg[0]'s value
		}//if-else
		break;//0xB
	case 0xC:
		_IR = "C000";						//We halt all instruction and exit
		break;//0xC						//	the program	
	case 0xD:
		_reg[rgstr] = _mem[r_left][r_right];			//Take value from _mem[X][Y] and store it
		break;//0xD													//into _reg[register]
	case 0xE:
		input = _reg[right];					//Take the byte from _reg[Y] and convert 
		first = input.substr(0,1);				//	them to an int before we use them
		second = input.substr(1,1);				//	as a memory location "_mem[X][Y]
		r_left = strtoul(first.c_str(), NULL, 16);		//Then take the data from _reg[X] and
		r_right = strtoul(second.c_str(), NULL, 16);		//	store it into our new memory location
		_mem[r_left][r_right] = _reg[left];
		break;//0xE
	case 0xF:
		r_left = strtoul(_reg[rgstr].c_str(), NULL, 16);	//Compare the data from _reg[register] to
		r_right = strtoul(_reg[0].c_str(), NULL, 16);		//	_reg[0] and if the data is LESS THAN OR
									// 	EQUAL TO we jump to raw location XY
		if (r_left <= r_right) {
			_PC = third + fourth;
		}//if
		break;//0xF
	}
}//instruction

void convert(string &hex)
{
	/* Convert any number GREATER THAN 9 into its hex value */
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
	}//if-else
}//convert

void intToHex(int _left, int _right, string &_final)
{
	/* Convert High and Low bit into its hex value and then store it into a string */
	string left, right;
	_left /= 16;
	_right %= 16;
	left = static_cast<ostringstream*>( &(ostringstream() << (_left)) )->str();
	right = static_cast<ostringstream*>( &(ostringstream() << (_right)) )->str();
	convert(left); convert(right);
	_final = (left + right);
}//intToHex

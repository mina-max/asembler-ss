#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "RelocationTable.h"
using namespace std;

typedef char _byte;

class Section {
public:
	string name;
	int size;
	vector<_byte> bytes;
	RelocationTable* relTable;
	int id;

	Section(string n) {
		name = n;
		size = 0;
		bytes = vector<_byte>();
		relTable = new RelocationTable();
	}

	void addByte(_byte b) {
		bytes.push_back(b);
		size++;
	}

	void printBytes(ostream& cout) {
		
		for (unsigned int i = 0; i < bytes.size(); i++) {
			std::stringstream ss;
			int high = (bytes[i] >> 4) & 0x0F;
			int low = (bytes[i] & 0x0F);
			ss << std::hex << high << low ; // int decimal_value
			std::string res(ss.str());

			cout << res;
			cout << " ";
			if (i % 2 == 1) {
				cout << "  ";
			}
			if ((i+1) % 16 == 0) {
				cout << endl;
			}
		}
	}

	void changeByte(int offset, _byte value) {
		bytes[offset] += value;
	}

	void changeWord(int offset, int value) {
		signed int high = ((int)bytes[offset + 1] & 0xFF) << 8;
		signed int low = ((int)bytes[offset] & 0xFF);
		signed short old = high + low;
		value += old;

		bytes[offset] = value & 0xFF;
		bytes[offset+1] = (value & 0xFF00) >> 8;
	}




};


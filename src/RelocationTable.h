#pragma once
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
using namespace std;

enum RelocationTypes {
	R_386_16,
	R_386_8,
	R_386_PC16
};


struct Relocation {
	int offset;
	RelocationTypes type;
	int value;
};



class RelocationTable
{
public:

	vector<string> relTypes = { "R_386_16", "R_386_8", "R_386_PC16" };

	vector<Relocation*> table = vector<Relocation*>();

	void addRel(Relocation* r) {
		table.push_back(r);
	}

	void print(ostream& cout) {
		for (unsigned int i = 0; i < table.size(); i++) {
			cout << left << setw(15) << table[i]->offset;
			cout << left << setw(15) << relTypes[table[i]->type];
			cout << left << setw(15) << table[i]->value << endl;
		}
	}
};


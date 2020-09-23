#pragma once
#include "Section.h"
#include <vector>
#include <iomanip>
#include <iostream>
using namespace std;
class SectionTable
{
public:
	vector<Section*> table = vector<Section*>();


	bool existSymbol(string name);
	void addSection(Section* s);

	int getSize() {
		return table.size();
	}

	void printLine(ostream& cout, int i) {

		cout << left << setw(15) << table[i]->name;
		cout << left << setw(15) << table[i]->name;
		cout << left << setw(10) << "/";
		cout << left << setw(5) << "l" << setw(5) << " ";
		cout << left << setw(10) << table[i]->id << left << setw(5) << table[i]->size << endl;
	}

	void printBytes(ostream& cout, int i) {
		cout << "Section: " << table[i]->name << endl;

		table[i]->printBytes(cout);
	}

	Section* getByName(string name) {
		for (unsigned int i = 0; i < table.size();i++) {
			if (table[i]->name == name) {
				return table[i];
			}
		}
		return nullptr;
	}

	void printRelocation(ostream& cout, int i) {
		table[i]->relTable->print(cout);
	}
	
};


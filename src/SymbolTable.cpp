#include "SymbolTable.h"
#include <iomanip>
#include "SectionTable.h"

bool SymbolTable::existSymbol(string name)
{
	for (std::vector<Symbol*>::iterator i = table.begin(); i != table.end(); ++i)
	{
		if ((*i)->name.compare(name) == 0)
			return true;
	}
	return false;
}

Symbol* SymbolTable::getSymbol(string name)
{
	int j = 0;
	cout << name << endl;
	for (std::vector<Symbol*>::iterator i = table.begin(); i != table.end(); ++i)
	{
		
			if ((*i)->name.compare(name) == 0) {
				//cout << "super" << endl;

				return *i;
			}
			
	}
	return nullptr;
}

void SymbolTable::addSymbol(Symbol* s)
{
	table.push_back(s);
}

void SymbolTable::checkAll()
{
	for (std::vector<Symbol*>::iterator i = table.begin(); i != table.end(); ++i)
	{
		if (!(*i)->defined && !(*i)->ext) {
			cout << "Error: All symbols must be defined!";
			exit(8);
		}
	}
}

void SymbolTable::print(ostream& cout, SectionTable* sectionTable)
{
	cout << "\t\t\t ~SYMBOL TABLE~ \t\t" << endl;
	cout << left << setw(15) << "name";
	cout << left << setw(15) << "section";
	cout << left << setw(10) << "value";
	cout << left << setw(5) << "scope" << setw(5) << " ";
	cout << left << setw(10) << "r.br." << left << setw(5) << "size" << endl;
	cout << "---------------------------------------------------------------------" << endl;

	cout << left << setw(15) << "";
	cout << left << setw(15) << "UND";
	cout << left << setw(10) << "/";
	cout << left << setw(5) << "l" << setw(5) << " ";
	cout << left << setw(10) << 0 << left << setw(5) << "/" << endl;

	for (unsigned int i = 0; i < sectionTable->getSize(); i++) {
		sectionTable->printLine(cout, i);
	}

	for (unsigned int i = 0; i < table.size(); i++) {
		cout << left << setw(15) << table[i]->name;
		cout << left << setw(15) << table[i]->section;
		cout << left << setw(10) << table[i]->value;
		cout << left << setw(5) << table[i]->scope << setw(5) << " ";
		cout << left << setw(10) << table[i]->myId << left << setw(5) << "/" << endl;
	}
	cout << "=====================================================================" << endl << endl << endl;


	cout << "\t\t\t ~RELOCATION TABLES~ \t\t" << endl;
	
	for (unsigned int i = 0; i < sectionTable->getSize(); i++) {
		if ((*sectionTable->table[i]->relTable).table.empty()) continue;
		cout << sectionTable->table[i]->name << " section" << endl << endl;
		cout << left << setw(15) << "offset";
		cout << left << setw(15) << "type";
		cout << left << setw(15) << "value" << endl;
		cout << "---------------------------------------------------------------------" << endl;
		sectionTable->printRelocation(cout, i);
		cout << endl;
	}
	cout << "======================================================================" << endl << endl << endl;



	cout << "\t\t\t ~SECTION DATA~ \t\t" << endl;
	cout << "---------------------------------------------------------------------" << endl;
	for (unsigned int i = 0; i < sectionTable->getSize(); i++) {
		sectionTable->printBytes(cout, i);
		cout << endl;
	}
	cout << "======================================================================" << endl << endl << endl;


}

void SymbolTable::backpatch(SectionTable* sectionTable)
{
	for (unsigned int i = 0; i < sectionTable->getSize(); i++) {
		Section* section = sectionTable->table[i];
		for (unsigned int j = 0; j < section->relTable->table.size(); j++) {
			Relocation* rel = section->relTable->table[j];
			int id = rel->value;
			Symbol* s = getById(id);
				if (s->relocatable) {

					if (!s->ext && rel->type == R_386_PC16 && section->name == s->section) {
						section->changeWord(rel->offset, s->value - rel->offset);
						section->relTable->table.erase(section->relTable->table.begin() + j);
						j--;
						continue;
					}

					if (s->scope == 'l') {
						Section* sect = sectionTable->getByName(s->section);
						rel->value = sect->id; //id sekcije
						if (rel->type == R_386_8) {
							//change byte
							section->changeByte(rel->offset, s->value);
						}
						else {
							//change word
							section->changeWord(rel->offset, s->value);
						}
					}
					
				}
				else {
					if (rel->type == R_386_8) {
						//change byte
						section->changeByte(rel->offset, s->value);
					}
					else {
						//change word
						section->changeWord(rel->offset, s->value);
					}
					section->relTable->table.erase(section->relTable->table.begin() + j);
					j--;
				}
		}
	}
	

}

Symbol* SymbolTable::getById(int id) {
	for (unsigned int i = 0; i < table.size(); i++) {
		if (table[i]->myId == id)
			return table[i];
	}
	return nullptr;
}

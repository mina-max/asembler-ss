#pragma once
#include <vector>
#include "Symbol.h"
#include "SectionTable.h"
class SymbolTable
{
private:
	vector<Symbol*> table = vector<Symbol*>();
public:
	bool existSymbol(string name);
	Symbol* getSymbol(string name);
	void addSymbol(Symbol* s);
	void checkAll();
	void print(ostream&, SectionTable*);
	void backpatch(SectionTable* sectionTable);
	Symbol* getById(int id);
};


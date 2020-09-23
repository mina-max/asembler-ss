#pragma once
#include <vector>
#include <string>
#include "SymbolTable.h"
using namespace std;

struct num {
	string symbol;
	int factor;
	num(string s, int f) {
		symbol = s;
		factor = f;
	}
};

class Equ
{
	string name;
	vector<num*> expression;

public:
	Equ(string n, vector<num*> expr) {
		expression = expr;
		name = n;
	}

	void solve(SymbolTable* symbolTable);

	bool isDefined(SymbolTable* symbolTable) {
		for (unsigned int i = 0; i < expression.size(); i++) {
			Symbol* s = symbolTable->getSymbol(expression[i]->symbol);
			if(s!=NULL)
				if (!s->defined && !s->ext) return false;
		}
		return true;
	}

};


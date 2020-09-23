#include "Equ.h"
#include <map>

void Equ::solve(SymbolTable* symbolTable)
{
	if(symbolTable != NULL) {
	map<string, int> indClass = map<string, int>();
	bool ext = false;
	for (unsigned int i = 0; i < expression.size(); i++) {
		string symbol = expression[i]->symbol;
		int factor = expression[i]->factor; //samo znak ispred simbola
		Symbol* s = symbolTable->getSymbol(symbol); //simbol iz izraza (desno)
		
		if(s != NULL) {
			if (!s->defined && !s->ext) { //ne moze ako nije definisan - osim ako nije extern
				cout << "Error: Illegal EQU expression";
				exit(5);
			}
			if (s->ext) { //ako jeste extern moze samo jedan extern simbol da bude u izrazu
				if (ext) {
					cout << "Error: cannot have more than one extern symbol";
					exit(6);
				}
			else {
				ext = true;
			}
		}
		
		//PRAVIMO MAPU ZA ODREDJIVANJE INDEKSA KLASIFIKACIJE
		std::map<string, int>::iterator it;
		

		if (s->relocatable) {
			it = indClass.find(s->section);

			if (it != indClass.end()) {
				it->second += factor;
			}
			else {
				indClass.insert(pair<string, int>(s->section, factor));
			}
		}
		}
		
		
	}

	Symbol* mySymbol = symbolTable->getSymbol(name); //simbol kom se dodeljuje vrednost
	string section = "?";
	bool rel = false;
	
	for (std::map<string, int>::iterator i = indClass.begin(); i != indClass.end(); i++) {
		//Symbol* s = symbolTable->getSymbol(i->first);
		int factor = i->second;
		if (i->second > 1 || i->second < -1) {
			cout << "Error: Illegal EQU expression";
			exit(7);
		}
		if (i->second == 1 || i->second == -1) {
			if (rel) {
				cout << "Error: Illegal EQU expression";
				exit(8);
			}
			else {
				section = i->first;
				if (section == "?") rel = false;
				else rel = true;
			}
		}
		//mySymbol->value += s->value * factor;
	}
	if(mySymbol != NULL){
	if (rel) {
		mySymbol->relocatable = true;
	}
	else {
		mySymbol->relocatable = false;
	}
	//cout << "solve" << endl;

	for (unsigned int i = 0; i < expression.size(); i++) {
		//cout << "ovo je simbol" << endl;
		
		string symbol = expression[i]->symbol;
		int factor = expression[i]->factor;
		Symbol* s = symbolTable->getSymbol(symbol);
		
		//cout << symbol << endl;
		
		if(s!=NULL) {
			cout << s->value << endl;
			mySymbol->value += s->value * factor;
		}
		


	}
	mySymbol->section = section;
	mySymbol->defined = true;
	}
	}
}

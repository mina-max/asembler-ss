#include "EquTable.h"

void EquTable::add(string symbolName, string expr, SymbolTable* symbolTable, Section* currSection)
{

	
	std::size_t prev = 1, pos;
	int sign = 1;
	vector<num*> e = vector<num*>();
	int constant = 0;
	
	
	while ((pos = expr.find_first_of("+-", prev)) != std::string::npos) {

		

		string n = expr.substr(prev, pos - prev);
		if (n == "") {
			sign = -1;
			prev = pos + 1;
			continue;
		}

		

		if ((std::regex_match(n, std::regex("[0-9]+")))) {
			//broj
			constant += sign * stoi(n);

			if (expr[pos] == '-') {
				sign = -1;
			}
			else {
				sign = 1;
			}
			prev = pos + 1;
			
		}
		else {
			//simbol
			num* symb = new num(n, sign);

			e.push_back(symb);

			prev = pos + 1;

			if (expr[pos] == '-') {
				sign = -1;
			}
			else {
				sign = 1;
			}
		}
		
	}
	
	string end = expr.substr(prev, pos-1);//pisalo je (prev,pos)
	//cout << end.size() << endl;
	end.erase(std::remove_if(end.begin(), end.end(), ::isspace), end.end());
	//std::cout << end << endl;
	//cout << "mina";
	//std::cout << "miko";
	std::regex reg("[0-9]+");
	smatch matches;

	if ((std::regex_search(end, matches, reg))) {
		//number
		//cout << "broj" << endl;
		constant += sign * stoi(end);
		//cout << constant << endl;
	}
	else {
		//symbol
		//cout << "simbol" << endl;
		num* symb = new num(end, sign);
		e.push_back(symb);

	}

	if (symbolTable->existSymbol(symbolName)) { //simbol kom se dodeljuje vrednost
		Symbol* s = symbolTable->getSymbol(symbolName);
		s->value = constant;
		if (!e.empty()) {
			Equ* entry = new Equ(symbolName, e);
			table.push_back(entry);
		}
		else {
			if (currSection != nullptr) s->section = currSection->name; //ovo i ne mora
			else s->section = "?";

			s->defined = true; //simbol je apsolutan ako se sastoji samo iz literala
		}
	}
	else {
		Symbol* newSymbol = nullptr;
		if (!e.empty()) {
			Equ* entry = new Equ(symbolName, e);
			table.push_back(entry);
			newSymbol = new Symbol(symbolName, "?", constant, 'l'); //ubacujemo simbol u tabelu ali ne znamo jos uvek u kojoj je sekciji, 
																		//kao vrednost upisujemo samo konstantu iz izraza
		}
		else {
			if(currSection == nullptr) newSymbol = new Symbol(symbolName, "?", constant, 'l');
			else newSymbol = new Symbol(symbolName, currSection->name, constant, 'l'); //simbol ima apsolutnu vrednost, ne zavisi od sekcije
			newSymbol->defined = true;
			newSymbol->relocatable = false;
		}
		symbolTable->addSymbol(newSymbol);
	}
	

	


}

void EquTable::solveAll(SymbolTable* symbolTable)
{
	int i = 0;

	while(!table.empty()) {
		if(!table[i]->isDefined(symbolTable)) {
			i++;
			if (i == table.size()) {
			std::cout << "Error: Cant solve EQU!";
			exit(5);
			}
		} else {
			table[i]->solve(symbolTable);
			table.erase(table.begin() + i);
			i = 0;
		}
		
	}
		
	
}




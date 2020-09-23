#include "Assembler.h"
#include <string>

int Assembler::locationCounter = 0;
Section* Assembler::curr_section = nullptr;
vector<Instructions> Assembler::jumpInstructions = vector<Instructions>();
vector<Instructions> Assembler::notJumpInstructions = vector<Instructions>();
vector<int> Assembler::jumpCodes = vector<int>();
vector<int> Assembler::notJumpCodes = vector<int>();


void Assembler::init() {

	jumpInstructions.push_back(IRET);
	jumpInstructions.push_back(RET);
	jumpInstructions.push_back(INT);
	jumpInstructions.push_back(CALL);
	jumpInstructions.push_back(JMP);
	jumpInstructions.push_back(JEQ);
	jumpInstructions.push_back(JNE);
	jumpInstructions.push_back(JGT);
	
	for (int i = 1; i < 9; i++) {
		jumpCodes.push_back(i);
	}

	notJumpInstructions.push_back(PUSH);
	notJumpInstructions.push_back(POP);
	notJumpInstructions.push_back(XCHG);
	notJumpInstructions.push_back(MOV);
	notJumpInstructions.push_back(ADD);
	notJumpInstructions.push_back(SUB);
	notJumpInstructions.push_back(MUL);
	notJumpInstructions.push_back(DIV);
	notJumpInstructions.push_back(CMP);
	notJumpInstructions.push_back(NOT);
	notJumpInstructions.push_back(AND);
	notJumpInstructions.push_back(OR);
	notJumpInstructions.push_back(XOR);
	notJumpInstructions.push_back(TEST);
	notJumpInstructions.push_back(SHL);
	notJumpInstructions.push_back(SHR);

	for (int i = 9; i < 25; i++) {
		notJumpCodes.push_back(i);
	}


	
}

void Assembler::singlePass(string fname, string output)
{
	init();
	ifstream fin(fname);
	if (!fin.is_open()) {
		cout << "Unable to open file";
		exit(8);
	}


	ofstream fout;
	fout.open(output);

	string line;

	SymbolTable* symbolTable = new SymbolTable();
	SectionTable* sectionTable = new SectionTable();
	EquTable* equTable = new EquTable();
	

	while (getline(fin, line)) {

		istringstream iss(line);
		string word;

		iss >> word;

		if (word == "") continue;

		if (word[0] == ':') {
			cout << "Error: Line cannot start with ':'!";
			exit(8);
		}
						

		if (word.back() == ':') {
			//LABEL

			word.pop_back();
			string label = word;

			if (label[0] == '.') {
				cout << "Error: Wrong syntax!";
				exit(8);
			}

			if (curr_section == nullptr) {
				cout << "Error: Label must be defined inside a section!";
				exit(8);
			}
			//SYMBOL PROCESSING
			if (symbolTable->existSymbol(label)) {
				//postoji u tabeli simbola
				Symbol* symbol = symbolTable->getSymbol(label);
				if (symbol->defined) {
					cout << "Error: Symbol is already defined!";
					exit(8);
				}
				else {
					//dodefinisemo
					symbol->section = curr_section->name;
					symbol->value = curr_section->size;
					symbol->defined = true;
				}
			}
			else {
				//dodaj u tabelu simbola ako ne postoji u tabeli sekcija
				if (!sectionTable->existSymbol(label)) {
					Symbol* s = new Symbol(label, curr_section->name, curr_section->size, 'l');
					s->defined = true;
					symbolTable->addSymbol(s);
				}
				else {
					cout << "Error: Name already in use!";
					exit(8);
				}
			}


			if(!(iss>>word)) word = "";
			
		}

		//DIRECTIVES AND INSTRUCTIONS
		bool end = false;
		

		if (!word.empty()) {
			switch (hashDirective(word)) {
			case GLOBAL:
			{
				string p = iss.str();
				if (p.back() == ',') {
					cout << "Error: Bad declaration!";
					exit(8);
				}

				while (getline(iss, word, ',')) {
					istringstream help(word);


					if (!(help >> word)) {
						cout << "Bad declaration";
						exit(8);
					}
					if (word.empty() || word == "") {
						cout << "Error: Bad directive";
						exit(8);
					}


					if (symbolTable->existSymbol(word)) {

						Symbol* s = symbolTable->getSymbol(word);
						if (s->section == "") s->section = "?";
						

						if (s->ext) {
							cout << "Error: Symbol cannot be both extern and global";
							return;
						}

						s->scope = 'g';
					}
					else {
						Symbol* s = new Symbol(word, "?", -1, 'g');
						symbolTable->addSymbol(s);
					}

					if (help >> word) {
						cout << "Error: Bad directive";
						exit(8);
					}
				}


				break;
			}
			case EXTERN:
			{
				string p = iss.str();
				if (p.back() == ',') {
					cout << "Error: Bad declaration!";
					exit(8);
				}

				while (getline(iss, word, ',')) {
					istringstream help(word);


					if (!(help >> word)) {
						cout << "Bad declaration";
						exit(8);
					}
					if (word.empty() || word == " ") {
						cout << "Error: Bad directive";
						exit(8);
					}


					if (symbolTable->existSymbol(word)) {

						Symbol* s = symbolTable->getSymbol(word);

						if (s->scope == 'g') {
							cout << "Error: Symbol cannot be both extern and global";
							exit(8);
						}
						s->ext = true;
						s->defined = true;
					}
					else {
						Symbol* s = new Symbol(word, "UND", 0, 'g');
						s->ext = true;
						s->defined = true; //MOZDA
						symbolTable->addSymbol(s);
					}

					if (help >> word) {
						cout << "Error: Bad directive";
						exit(8);
					}
				}



				break;
			}
			case SECTION:
			{
				if (!(iss >> word)) {
					cout << "Error: Section must have a name!";
					exit(8);
				}
				word.pop_back();
				string name = word;
				

				if (sectionTable->existSymbol(name) || symbolTable->existSymbol(name)) {
					cout << "Error: Name already in use!";
					exit(8);
				}

				if (word[0] == '.') {
					cout << "Error: Wrong syntax";
					exit(8);
				}
				

				Section* s = new Section(name);
				sectionTable->addSection(s);
				s->id = Symbol::id++;


				if (curr_section != nullptr) {
					curr_section->size = locationCounter;
					locationCounter = 0;
				}

				curr_section = s;

				if (iss >> word) {
					cout << "Error: Bad directive/instruction";
					exit(8);
				}
				
				break;
			}
			case END:
			{

				if (curr_section) {
					curr_section->size = locationCounter;
					locationCounter = 0;
				}
				end = true;


				break;
			}
			case BYTE:
			{
				if (curr_section == nullptr) {
					cout << "Error: No section defined!";
					exit(8);
				}

				string p = iss.str();
				if (p.back() == ',') {
					cout << "Error: Bad declaration!";
					exit(8);
				}

				while (getline(iss, word, ',')) {
					istringstream help(word);

					if (!(help >> word)) {
						cout << "Bad declaration";
						exit(8);
					}
					if (word.empty() || word == " ") {
						cout << "Error: Bad directive";
						exit(8);
					}

					if (std::regex_match(word, std::regex("[-]*[0-9]+"))) {
						//number
						int num = stoi(word);
						char b = num & 0xFF;
						curr_section->addByte(b);
						locationCounter++;
					}
					else {
						//symbol
						symbolUsage(word, symbolTable, curr_section, 1, "abs");
						curr_section->addByte(0);
						locationCounter++;
						
					}


				}



				break;
			}
			case WORD:
			{
				if (curr_section == nullptr) {
					cout << "Error: No section defined!";
					exit(8);
				}

				string p = iss.str();
				if (p.back() == ',') {
					cout << "Error: Bad declaration!";
					exit(8);
				}

				while (getline(iss, word, ',')) {
					istringstream help(word);

					if (!(help >> word)) {
						cout << "Bad declaration";
						exit(8);
					}
					if (word.empty() || word == " ") {
						cout << "Error: Bad directive/instruction";
						exit(8);
					}

					if (std::regex_match(word, std::regex("[-]*[0-9]+"))) {
						//number
						int num = stoi(word);
						char low = num & 0xFF;
						curr_section->addByte(low);
						locationCounter++;
						char high = (0xFF00 & num) >> 8;
						curr_section->addByte(high);
						locationCounter++;
					}
					else {
						//symbol

						symbolUsage(word, symbolTable, curr_section, 2, "abs");
						curr_section->addByte(0);
						locationCounter++;
						curr_section->addByte(0);
						locationCounter++;

					}


				}
				break;
			}
			case SKIP:
			{
				if (curr_section == nullptr) {
					cout << "Error: No section defined!";
					exit(8);
				}

				if (!(iss >> word)) {
					cout << "Error: Bad directive";
					exit(8);
				}


				if (std::regex_match(word, std::regex("[0-9]+"))) {
					//number
					int num = stoi(word);

					while (num--) {
						curr_section->addByte('\0');
						locationCounter++;
					}
					}
				else {
					//symbol
					cout << "Error: Cannot use symbol here!";
					exit(8);
				}

				
				break;
			}
			case EQU:
			{
								
				if (!getline(iss, word, ',')) {
					cout << "Error: Not enough arguments!";
					exit(8);
				}

				istringstream is2(word);
				is2 >> word;

				if (word.empty() || word == " ") {
					cout << "Error: Bad directive!";
					exit(8);
				}
				if (word[0] == '.' || word[0] == '$' || isdigit(word[0])) {
					cout << "Error";
					exit(8);
				}

				if (sectionTable->existSymbol(word)) {
					cout << "Error";
					exit(8);
				}

				if (symbolTable->existSymbol(word)) {
					Symbol* s = symbolTable->getSymbol(word);
					if (s->defined || s->ext) {
						cout << "Error";
						exit(8);
					}
				}
				

				string expr;
				getline(iss, expr);

				equTable->add(word, expr, symbolTable, curr_section);


				break;
			}
			case NOTFOUND: 
			{
				processInstruction(word, iss, symbolTable, &end);
				
				break;
			}

			}
		}
		if (end) {
			//KOD ZA KRAJ
			
			equTable->solveAll(symbolTable);
			symbolTable->checkAll();
			symbolTable->backpatch(sectionTable);
			symbolTable->print(fout, sectionTable);
			cout << "USPEH";

			return;


		}


	}




}

Directives Assembler::hashDirective(string in) {
	if (in == ".equ") return EQU;
	if (in == ".section") return SECTION;
	if (in == ".extern") return EXTERN;
	if (in == ".byte") return BYTE;
	if (in == ".word")return WORD;
	if (in == ".skip") return SKIP;
	if (in == ".end") return END;
	if (in == ".global") return GLOBAL;
	return NOTFOUND;
}

Instructions Assembler::hashInstruction(string in)
{
	if (in == "halt") return HALT;
	if (in == "iret") return IRET;
	if (in == "ret") return RET;
	if (in == "call") return CALL;
	if (in == "jmp") return JMP;
	if (in == "jeq") return JEQ;
	if (in == "jne") return JNE;
	if (in == "jgt") return JGT;
	if (in == "int") return INT;
	if (in == "xchg" || in == "xchgw" || in == "xchgb") return XCHG;
	if (in == "push" || in == "pushw" || in == "pushb") return PUSH;
	if (in == "pop" || in == "popw" || in == "popb") return POP;
	if (in == "mov" || in == "movb" || in == "movw") return MOV;
	if (in == "add" || in == "addw" || in == "addb") return ADD;
	if (in == "sub" || in == "subw" || in == "subb") return SUB;
	if (in == "mul" || in == "mulw" || in == "mulb") return MUL;
	if (in == "div" || in == "divw" || in == "divb") return DIV;
	if (in == "cmp" || in == "cmpw" || in == "cmpb") return CMP;
	if (in == "not" || in == "notw" || in == "notb") return NOT;
	if (in == "and" || in == "andw" || in == "andb") return AND;
	if (in == "or" || in == "orw" || in == "orb") return OR;
	if (in == "xor" || in == "xorw" || in == "xorb") return XOR;
	if (in == "test" || in == "testw" || in == "testb") return TEST;
	if (in == "shl" || in == "shlw" || in == "shlb") return SHL;
	if (in == "shr" || in == "shrw" || in == "shrb") return SHR;
	return ILLEGALINSTRUCTION;
}

void Assembler::processInstruction(string word, istringstream& iss, SymbolTable* symbolTable, bool* end) {

	Instructions instr = hashInstruction(word);
	if (curr_section == nullptr) {
		cout << "Error: Instruction must be part of a section!";
		exit(6);
	}
	if (instr == ILLEGALINSTRUCTION) {
		cout << "Error: Illegal instruction!";
		exit(9);
	}

	if (instr == HALT) {
		if (curr_section) {
			curr_section->size = locationCounter;
			locationCounter = 0;
		}
		*end = true;

		curr_section->addByte(0);
		return;
		
	}

	if (isJump(instr)) {
		processJump(word, iss, symbolTable);
	}
	else {
		checkNumOfArgs(word, iss, symbolTable);
	}

}

bool Assembler::isJump(Instructions instr)
{
	for (unsigned int i = 0; i < jumpInstructions.size(); i++) {
		if (jumpInstructions[i] == instr) return true;
	}
	return false;

}

void Assembler::processJump(string word, istringstream& iss, SymbolTable* symbolTable)
{
	Instructions instr = hashInstruction(word);
	int ind = 0;
	for (unsigned int i = 0; i < jumpInstructions.size(); i++) {
		if (instr == jumpInstructions[i]) {
			ind = i;
			break;
		}
	}
	int code = jumpCodes[ind];
	int size = 1; //operand je uvek 2 bajta

	if (instr == IRET || instr == RET) {
		curr_section->addByte((_byte)((code << 3)));
		locationCounter++;
		return;
	}

	if (!(iss >> word)) {
		cout << "Error: Wrong parameters!";
		exit(6);
	}

	curr_section->addByte((_byte)((code << 3) + (size << 2)));
	locationCounter++;
	int addr = operandsJump(word);

	switch (addr) {
	case 1:
	{
		int arg = stoi(word);
		curr_section->addByte((_byte)0);
		curr_section->addByte((_byte)(arg & 0xFF));
		curr_section->addByte((_byte)((arg & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 2:
	{
		curr_section->addByte((_byte)0);
		symbolUsage(word, symbolTable, curr_section, 2, "abs");
		curr_section->addByte((_byte)(0 & 0xFF));
		curr_section->addByte((_byte)((0 & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 3:
	{
		int numOfRegister = word.back() - '0';
		curr_section->addByte((_byte)((1 << 5) + (numOfRegister << 1)));
		locationCounter++;
		break;
	}
	case 4:
	{
		int numOfRegister = word.at(word.size() - 2) - '0';
		curr_section->addByte((_byte)((2 << 5) + (numOfRegister << 1)));
		locationCounter++;
		break;
	}
	case 5:
	{
		int begin = 1;
		int end = word.find_first_of('(');
		int value = stoi(word.substr(begin, end-begin));
		int numOfRegister = word.at(word.size() - 2) - '0';
		curr_section->addByte((_byte)(3 << 5) + (numOfRegister << 1));
		curr_section->addByte((_byte)(value & 0xFF));
		curr_section->addByte((_byte)((value & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 6:
	{
		int begin = 1;
		int end = word.find_first_of('(');
		string symbol = word.substr(begin, end - begin);
		int numOfRegister = word.at(word.size() - 2) - '0';
		curr_section->addByte((_byte)((3 << 5) + (numOfRegister << 1)));
		symbolUsage(symbol, symbolTable, curr_section, 2, "abs");
		curr_section->addByte((_byte)(0 & 0xFF));
		curr_section->addByte((_byte)((0 & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 7:
	{
		int begin = 1;
		int end = word.find_first_of('(');
		string symbol = word.substr(begin, end - begin);
		curr_section->addByte((_byte)((3 << 5) + (7 << 1)));
		symbolUsage(symbol, symbolTable, curr_section, 2, "pcrel");
		curr_section->addByte((_byte)(-2 & 0xFF));
		curr_section->addByte((_byte)((-2 & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 8:
	{
		int value = stoi(word.substr(1));
		curr_section->addByte((_byte)(4 << 5));
		curr_section->addByte((_byte)(value & 0xFF));
		curr_section->addByte((_byte)((value & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 9:
	{
		string symbol = word.substr(1);
		curr_section->addByte((_byte)(4 << 5));
		symbolUsage(symbol, symbolTable, curr_section, 2, "abs");
		curr_section->addByte((_byte)(0 & 0xFF));
		curr_section->addByte((_byte)((0 & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 0:
	{
		cout << "Error in addresing";
		exit(5);
	}
	}



}

void Assembler::checkNumOfArgs(string word, istringstream& iss, SymbolTable* symbolTable)
{
	Instructions instr = hashInstruction(word);
	if (instr == POP || instr == PUSH) {
		//ONE ARG
		if (!(iss >> word)) {
			cout << "Error: not enough arguments!";
			exit(6);
		}
		int size = (word.back() == 'b' ? 0 : 1);


		int ind = 0;
		for (unsigned int i = 0; i < notJumpInstructions.size(); i++) {
			if (instr == notJumpInstructions[i]) {
				ind = i;
				break;
			}
		}
		int code = notJumpCodes[ind];

		curr_section->addByte((_byte)((code << 3) + (size << 2)));
		locationCounter++;
		int arg1 = operandsNotJump(word);
		if (instr == POP && (arg1 == 1 || arg1 == 2)) {
			cout << "Error: Cannot use imm for POP";
			exit(6);
		}
		processNotJump(arg1, word, iss, symbolTable, size+1);
		if (iss >> word) {
			cout << "Error: Too many arguments";
			exit(6);
		}
	}
	else {
		//OP CODE

		int size = (word.back() == 'b' ? 0 : 1);
		if (instr == SUB) size = 1;

		int ind = 0;
		for (unsigned int i = 0; i < notJumpInstructions.size(); i++) {
			if (instr == notJumpInstructions[i]) {
				ind = i;
				break;
			}
		}
		int code = notJumpCodes[ind];

		curr_section->addByte((_byte)((code << 3) + (size << 2)));
		locationCounter++;

		//CHECK BEGINNING

		string p = iss.str();
		if (p.back() == ',') {
			cout << "Error: Bad declaration";
			exit(6);
		}

		//FIRST OPERAND
		if (!getline(iss, word, ',')) {
			cout << "Error: First argument missing";
			exit(6);
		}

		istringstream issfirst(word);


		if (!(issfirst >> word)) {
			cout << "First argument missing";
			exit(6);
		}
		if (word.empty() || word == " ") {
			cout << "Argument empty";
			exit(6);
		}
		int arg1 = operandsNotJump(word);

		processNotJump(arg1, word, iss, symbolTable, size+1);

		if (issfirst >> word) {
			cout << "First argument is badly formated";
			exit(6);
		}
		//SECOND OPERAND

		if (!getline(iss, word, ',')) {
			cout << "Second argument missing";
			exit(6);
		}

		istringstream isssecond(word);


		if (!(isssecond >> word)) {
			cout << "Second argument missing";
			exit(6);
		}
		if (word.empty() || word == " ") {
			cout << "Argument empty";
			exit(6);
		}
		int arg2 = operandsNotJump(word);

		if ((instr == MOV || instr == ADD || instr == SUB || instr == MUL || instr == DIV || instr == NOT || instr == AND || instr == OR || instr == XOR || instr == SHL || instr == SHR) && (arg2 == 1 || arg2 == 2)) {
			cout << "Error: Cannot use imm for this instruction";
			exit(6);
		}

		processNotJump(arg2, word, iss, symbolTable, size+1);

		if (isssecond >> word) {
			cout << "Error";
			exit(6);
		}

		//CHECK END


		if (getline(iss, word, ',')) {
			cout << "Error";
			exit(6);
		}
		if (iss >> word) {
			cout << "Error Too many arguments";
			exit(6);
		}

	}

}

void Assembler::processNotJump(int arg, string word, istringstream& iss, SymbolTable* symbolTable, int numOfBytes) 
{
	switch (arg) {
	case 1:
	{
		int arg = stoi(word.substr(1));
		curr_section->addByte((_byte)0);
		locationCounter++;
		if(numOfBytes == 1) {
			curr_section->addByte((_byte)(arg & 0xFF));
			locationCounter++;
		}
		else {
			curr_section->addByte((_byte)(arg & 0xFF));
			curr_section->addByte((_byte)((arg & 0xFF00) >> 8));
			locationCounter+=2;
		}
		break;
	}
	case 2:
	{
		
		curr_section->addByte((_byte)0);
		locationCounter++;
		if (numOfBytes == 1) {
			symbolUsage(word.substr(1), symbolTable, curr_section, 1, "abs");
			curr_section->addByte((_byte)(0 & 0xFF));
			locationCounter++;
		}
		else {
			symbolUsage(word.substr(1), symbolTable, curr_section, 2, "abs");
			curr_section->addByte((_byte)(0 & 0xFF));
			curr_section->addByte((_byte)((0 & 0xFF00) >> 8));
			locationCounter += 2;
		}
		break;
	}
	case 3:
	{
		if (word.back() == 'h' || word.back() == 'l') {
			int numOfRegB = word.at(word.size() - 2) - '0';
			int hl = word.back() == 'h' ? 1 : 0;
			curr_section->addByte((_byte)((1 << 5)+(numOfRegB << 1) + hl));
			locationCounter++;
		}
		else {
			int numOfReg = word.back() - '0';
			curr_section->addByte((_byte)((1 << 5) + (numOfReg << 1)));
			locationCounter++;
		}
		
		break;
	}
	case 4:
	{

		if (word.at(word.size()-2) == 'h' || word.at(word.size()-2) == 'l') {
			int numOfRegB = word.at(word.size() - 3) - '0';
			int hl = word.at(word.size()-2) == 'h' ? 1 : 0;
			curr_section->addByte((_byte)((2 << 5) + (numOfRegB << 1) + hl));
			locationCounter++;
		}
		else {
			int numOfReg = word.at(word.size()-2)-'0';
			curr_section->addByte((_byte)((2 << 5) + (numOfReg << 1)));
			locationCounter++;
		}

		break;
	}
	case 5:
	{
		int begin = 0;
		int end = word.find_first_of('(');
		int value = stoi(word.substr(begin, end - begin));

		if (word.at(word.size() - 2) == 'h' || word.at(word.size() - 2) == 'l') {
			int numOfRegB = word.at(word.size() - 3) - '0';
			int hl = word.at(word.size() - 2) == 'h' ? 1 : 0;
			curr_section->addByte((_byte)((3 << 5) + (numOfRegB << 1) + hl));
			locationCounter++;
		}
		else {
			int numOfReg = word.at(word.size() - 2) - '0';
			curr_section->addByte((_byte)((3 << 5) + (numOfReg << 1)));
			locationCounter++;
		}

		if (numOfBytes == 1) {
			curr_section->addByte((_byte)(value & 0xFF));
			locationCounter++;
		}
		else {
			curr_section->addByte((_byte)(value & 0xFF));
			curr_section->addByte((_byte)((value & 0xFF00) >> 8));
			locationCounter += 2;
		}
					   		 	  
		
		
		break;
	}
	case 6:
	{
		int begin = 0;
		int end = word.find_first_of('(');
		string symbol = word.substr(begin, end - begin);

		if (word.at(word.size() - 2) == 'h' || word.at(word.size() - 2) == 'l') {
			int numOfRegB = word.at(word.size() - 3) - '0';
			int hl = word.at(word.size() - 2) == 'h' ? 1 : 0;
			curr_section->addByte((_byte)((3 << 5) + (numOfRegB << 1) + hl));
			locationCounter++;
		}
		else {
			int numOfReg = word.at(word.size() - 2) - '0';
			curr_section->addByte((_byte)((3 << 5) + (numOfReg << 1)));
			locationCounter++;
		}

		

		if (numOfBytes == 1) {
			symbolUsage(symbol, symbolTable, curr_section, 1, "abs");
			curr_section->addByte((_byte)(0 & 0xFF));
			locationCounter++;
		}
		else {
			symbolUsage(symbol, symbolTable, curr_section, 2, "abs");
			curr_section->addByte((_byte)(0 & 0xFF));
			curr_section->addByte((_byte)((0 & 0xFF00) >> 8));
			locationCounter += 2;
		}
		break;
	}
	case 7:
	{
		int begin = 0;
		int end = word.find_first_of('(');
		string symbol = word.substr(begin, end - begin);
		curr_section->addByte((_byte)((3 << 5) + (7 << 1)));
		symbolUsage(symbol, symbolTable, curr_section, 2, "pcrel");
		curr_section->addByte((_byte)(-2 & 0xFF));
		curr_section->addByte((_byte)((-2 & 0xFF00) >> 8));
		locationCounter += 3;
		break;
	}
	case 8:
	{
		int value = stoi(word);
		curr_section->addByte((_byte)(4 << 5));
		locationCounter++;

		if (numOfBytes == 1) {
			curr_section->addByte((_byte)(value & 0xFF));
			locationCounter++;
		}
		else {
			curr_section->addByte((_byte)(value & 0xFF));
			curr_section->addByte((_byte)((value & 0xFF00) >> 8));
			locationCounter += 2;
		}
		break;
	}
	case 9:
	{
		string symbol = word;
		curr_section->addByte((_byte)(4 << 5));
		locationCounter++;

		
		if (numOfBytes == 1) {
			symbolUsage(symbol, symbolTable, curr_section, 1, "abs");
			curr_section->addByte((_byte)(0 & 0xFF));
			locationCounter++;
		}
		else {
			symbolUsage(symbol, symbolTable, curr_section, 2, "abs");
			curr_section->addByte((_byte)(0 & 0xFF));
			curr_section->addByte((_byte)((0 & 0xFF00) >> 8));
			locationCounter += 2;
		}
		break;
	}
	case 0:
	{
		cout << "Error in addresing";
		exit(5);
	}

	}

}

int Assembler::operandsJump(string word) 
{
	//IMM literal
	string expr = "^[0-9]+$";
	if (std::regex_match(word, std::regex(expr))) {
		return 1;
	}
	
	//REGDIR
	expr = "^\\*%r[0-7][hl]?$";  
	if (std::regex_match(word, std::regex(expr))) {
		return 3;
	}
	//REGIND
	expr = "^\\*\\(%r[0-7][hl]?\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 4;
	}
	//REGINDPOM + literal
	expr = "^\\*[0-9]+\\(%r[0-7]\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 5;
	}
	//REGINDPOM + symbol
	expr = "^\\*[^$.0-9][^:]*\\(%r[0-6]\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 6;
	}
	//PCREL
	expr = "^\\*[^$.0-9][^:]*\\((%pc|%r[0-7]){1}\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 7;
	}
	//MEMDIR literal
	expr = "^\\*[-]?[0-9]+$";
	if (std::regex_match(word, std::regex(expr))) {
		return 8;
	}

	//MEMDIR symbol
	expr = "^\\*[^$.0-9][^:]*$";
	if (std::regex_match(word, std::regex(expr))) {
		return 9;
	}

	//IMM symbol
	expr = "^[^$.0-9][^:]*$";
	if (std::regex_match(word, std::regex(expr))) {
		return 2;
	}

	return 0;

}

int Assembler::operandsNotJump(string word) {
	//IMM literal
	string expr = "^\\$[0-9]+$";
	if (std::regex_match(word, std::regex(expr))) {
		return 1;
	}
	//IMM symbol
	expr = "^\\$[^$.0-9][^:]*$";
	if (std::regex_match(word, std::regex(expr))) {
		return 2;
	}
	//REGDIR
	expr = "^%r[0-7][hl]?$";
	if (std::regex_match(word, std::regex(expr))) {
		return 3;
	}
	//REGIND
	expr = "^\\(%r[0-7][hl]?\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 4;
	}
	//REGINDPOM + literal
	expr = "^[0-9]+\\(%r[0-7]\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 5;
	}
	//REGINDPOM + symbol
	expr = "^[^$.0-9][^:]*\\(%r[0-6]\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 6;
	}
	//PCREL
	expr = "^[^$.0-9][^:]*\\((%pc|%r[0-7]){1}\\)$";
	if (std::regex_match(word, std::regex(expr))) {
		return 7;
	}

	//MEMDIR literal
	expr = "^[-]?[0-9]+$";
	if (std::regex_match(word, std::regex(expr))) {
		return 8;
	}

	//MEMDIR symbol
	expr = "^[^$.0-9][^:]*$";
	if (std::regex_match(word, std::regex(expr))) {
		return 9;
	}

	return 0;
}

void Assembler::symbolUsage(string word, SymbolTable* symbolTable, Section* curr_section, int numOfBytes, string addr) {

	
	Relocation* rel = new Relocation();
	rel->offset = curr_section->size;
	
	if (addr == "abs") {
		if (numOfBytes == 1) 
			rel->type = R_386_8;
		else 
			rel->type = R_386_16;
	}
	else rel->type = R_386_PC16;
	

	if (symbolTable->existSymbol(word)) {
		Symbol* s = symbolTable->getSymbol(word);
		rel->value = s->myId;
		curr_section->relTable->addRel(rel);
	}
	else {
		Symbol* s = new Symbol(word, "?", 0, 'l');
		rel->value = s->myId;
		curr_section->relTable->addRel(rel);
		symbolTable->addSymbol(s);
	}


}

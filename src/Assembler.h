#pragma once
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Symbol.h"
#include "Section.h"
#include "SymbolTable.h"
#include "SectionTable.h"
#include "Enums.h"
#include "Equ.h"
#include "EquTable.h"
#include <regex>
#include <map>
using namespace std;





class Assembler
{
	
public:
	static int locationCounter;
	static Section* curr_section;

	static vector<Instructions> jumpInstructions;
	static vector<int> jumpCodes;

	static vector<Instructions> notJumpInstructions;
	static vector<int> notJumpCodes;

	static void singlePass(string inputFile, string outputFile);
	static void init();
	static Directives hashDirective(string in);
	static Instructions hashInstruction(string in);
	static void processInstruction(string word, istringstream& iss, SymbolTable* symbolTable, bool* end);
	static bool isJump(Instructions i);
	static void processJump(string instr, istringstream& iss, SymbolTable* symbolTable);
	static void checkNumOfArgs(string word, istringstream& iss, SymbolTable* symbolTable);
	static void processNotJump(int arg1, string word, istringstream& iss, SymbolTable* symbolTable, int numOfBytes);
	static int operandsJump(string word);
	static int operandsNotJump(string word);
	static void symbolUsage(string word, SymbolTable* symbolTable, Section* curr_section, int numOfBytes, string addr);
	
};


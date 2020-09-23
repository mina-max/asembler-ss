#pragma once
#include <vector>
#include "Equ.h"
#include "SymbolTable.h"
#include "Section.h"
#include <regex>

class EquTable
{
	vector<Equ*> table;
public:
	void add(string symbolName, string expr, SymbolTable* symbolTable, Section* currSection);
	void solveAll(SymbolTable* symbolTable);
};


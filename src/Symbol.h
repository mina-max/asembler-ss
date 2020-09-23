#pragma once
#include <string.h>
#include <iostream>
#include <vector>
using namespace std;


class Symbol {
public:
	static int id;
	int myId;
	string name;
	string section;
	int value;
	char scope;

	bool defined = false;
	bool relocatable = true;
	bool ext = false;

	Symbol(string n, string s, int v, char sc) {
		name = n; section = s; value = v; scope = sc;
		myId = id++;
	}


};
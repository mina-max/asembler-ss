#include <iostream>
#include "Assembler.h"
#include <string.h>
#include <regex>
#include <iomanip>
using namespace std;


int main(int argc, char** argv) {
	string inputFile;
	string outputFile;

	if (argc == 4 && strcmp(argv[1], "-o") == 0) {
		outputFile = argv[2];
		inputFile = argv[3];
	}
	else {
		cout << "Error: arguments\n";
		return 0;
	}

	Assembler::singlePass(inputFile, outputFile);

	

}
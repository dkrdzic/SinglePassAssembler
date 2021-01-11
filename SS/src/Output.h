#ifndef OUTPUT_H
#define OUTPUT_H


#include <string>
#include "Assembler.h"
using namespace std;

class Instruction;

class Output{


private:

Output();

string memory;
int locationCounter;
string section;

string adressing;//kod opdescr gde ide koje je adresiranje samo za ona kojima treba imDiAd
int pcRel;       
int PC;

friend class Assembler;


};



#endif
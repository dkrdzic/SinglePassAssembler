
#ifndef FORWARDREF_H
#define FORWARDREF_H

#include "Assembler.h"
#include <string>

using namespace std;

class Instruction;


class ForwardRef{



private:


ForwardRef(int LC,string section);
ForwardRef();

int LC;
string section;

int imDiAdSize;


friend class Assembler;

};

#endif
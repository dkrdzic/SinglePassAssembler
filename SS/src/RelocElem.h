#ifndef RELOCELEM_H
#define RELOCELEM_H

#include <string>

using namespace std;

class RelocElem
{

private:
    RelocElem();

    friend class Assembler;

    int locationCounter;

    string section;
    string type;
    int value;
};

#endif
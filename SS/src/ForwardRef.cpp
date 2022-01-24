#include "ForwardRef.h"
#include "Instruction.h"

ForwardRef::ForwardRef(int LC, string section)
{

    this->LC = LC;
    this->section = section;

    this->imDiAdSize = -1;
}

ForwardRef::ForwardRef()
{

    this->imDiAdSize = -1;
}
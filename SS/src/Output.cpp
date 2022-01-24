#include "Output.h"
#include "Instruction.h"

Output::Output()
{

    this->locationCounter = 0;
    this->memory = "??";
    this->section = "*UND*";

    this->pcRel = 0;
    this->PC = -1;
    this->adressing = "?";
}
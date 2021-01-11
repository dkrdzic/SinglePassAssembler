#ifndef INSTRUCTION_H
#define INSTRUCTION_H


#include <string>

using namespace std;


enum Instructions{XCHG,XCHGB,XCHGW,MOV,MOVB, MOVW,ADD,ADDB,ADDW,SUB,SUBB,SUBW,MUL,MULB,MULW,DIV,DIVB,DIVW,CMP,CMPB,CMPW,NOT,NOTB,NOTW,AND,ANDB,ANDW,OR,ORB,ORW,XOR,XORB,XORW,TEST,TESTB,TESTW,SHL,SHLB,SHLW,SHR,SHRB,SHRW,INT,CALL,JMP,JEQ,JNE,JGT,PUSH,POP};




class Instruction{

private:


Instruction();

Instructions instructions;

string instruction;
int operandSize;
string firstByte;






friend class Assembler;
friend class Symbol;
friend class Output;
friend class ForwardRef;


};


#endif

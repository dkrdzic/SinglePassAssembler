


#ifndef ASSEMBLER_H
#define ASSEMBLER_H


#include <list>
#include <iterator>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>





using namespace std;





class Symbol;
class Output;
class RelocElem;
class ForwardRef;
class Instruction;

class Assembler{


public:

Assembler(string inputFile, string outputFile); //inputFile je ime file-a sa sadrzajem nekog asemblerskog teskta
                                                //outputFile je ime file-a u koji cemo upisati izlaz koji nam se trazi  


~Assembler();



private:

list<string> tokensInLine;  



string currentSection;
int LC;
int PC;


list<list<Output*>> output;

list<RelocElem*> relocTable;


int tokenize(string line);

int fix(Symbol* symbol); //prepravlja simbol na samom kraju kad se preodje file
                        //u toku obrade file-a kad naidje na simbol on ne zna da li je globalan ili lokalan
                        //jer npr global direktiva moze biti posle tog simbola, takodje ne zna ni pravi id
                        // tog simbola u tos-u jer ce se tos sortirati jer simboli sekcija moraju biti prvi
                        //zato kad naidje popuni sve u reloc zapisu sto moze i u output-u
                        //a value u reloc i memory u output ostavi za fix, kad ce def znati da li je lokalan i
                        //globalan i sta treba da popuni

                        //sadrzaj fix->sustina

                        //if globalBool->globalan
                      //if !globalBool && externBool{
//                             if defined->lokaln (prklopi uvezen simbol iz extern direktive,nije isti simbol ko u .extern)
                         //    if !defined->globalan (simbol iz extern direktive)
                        //}
                        //if (!globalBool && !externBool){
                        //if defined->lokalan
                        //if !defined->greska,simbol nije definisan
                        //}



int singleRun(string inpuFile);


int currentLine;

//directives

int globalExtern(int externBool);
int equ();
int skip();
int byte();
int label(string token);
int section();
int word();



//instruction
int noOpInstructions(string token);
int twoOpInstructions(string instruction);
int oneOpInstructions(string token);

//adressing
int immed(Instruction* inst,string operand,int* dstSize,Output* outputSrcOperand);
int regdir(Instruction* inst,string operand,int isSrc,int* dstSize, Output* outputOpDescr);
int regind(Instruction* inst,string src,string dst, int isSrc,int* dstSize,Output* outputOpDescr);
int memdir(Instruction* inst,string src, string dst, int isSrc,int *dstSize,Output* outputOpDescr);
int regindpom(Instruction* inst,string src,string dst, int isSrc, int* dstSize,Output* outputOpDescr,string* pom);
int ImDiAd(string operand,int imDiAdSize, Output* outputImDiAd,string relocType);

//instructions

list<Instruction*> instructions;
void instructionsInsert();


//converters
string convertFromDecToHex(int digit);
int convertFromHexToDec(string value);
string convertFromDecToBin(int dec);
string findTwosComplement(string binary);
int convertFromBinToDec(int binary);


//tables
list<Symbol*> tableOfSymbols;
list<Symbol*> sectionSortTable;
list<Symbol*> symbolSortTable;
list<Symbol*> TNS;

//operation with symbols in TOS
Symbol* findSymbol(string label);
void pushSymbol(Symbol* symbol);

//edit table of Symbols 
void sortTableOfSymbols();
int isEverythingOkWithTable();

//print
void printRelocTable();
void printTableOfSymbols();
void printOutput();
void printTokensInLine();

void resolveExternSymbols();



int equResolve();


void writeToFile(string outputFile);


friend class Symbol;
friend class TableOfSymbols;



};


#endif


//1 output je radjen za operand (bilo 1 ili 2B) i za directive i njihove operande..
//za 1 instrukciju je moglo vise outputa koji su u jednom redu.. jedan za prvi bajt (OC),po jedan za op<num>descr, i po jedan za oba operanda (ma na koliko bajtova bili- 1 ili 2) 
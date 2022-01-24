#include "Assembler.h"
#include "Symbol.h"
#include "Output.h"
#include <iomanip>
#include "RelocElem.h"
#include "ForwardRef.h"
#include "Instruction.h"
#include <stdlib.h>
#include <regex>
#include <iostream>

using namespace std;

Assembler::Assembler(string inputFile, string outputFile)
{

    this->currentLine = 0;
    this->currentSection = "*UND*";
    this->LC = 0;
    this->PC = 0;

    this->instructionsInsert();

    if (!this->singleRun(inputFile))
    {
        cout << "Error line: " << currentLine << endl;
        exit(1);
    }

    if (!this->isEverythingOkWithTable())
    {
        cout << "Error line: " << currentLine << endl;
        exit(1);
    }

    this->resolveExternSymbols();

    if (!this->equResolve())
    {
        cout << "Error line: " << currentLine << endl;
        exit(1);
    }

    this->sortTableOfSymbols();

    list<Symbol *>::iterator it;

    for (it = this->tableOfSymbols.begin(); it != this->tableOfSymbols.end(); it++)
    {
        if (this->fix(*it))
            continue;
        else
        {
            cout << "Error line: " << currentLine << endl;
            exit(1);
        }
    }
    cout << endl;

    this->printTableOfSymbols();

    cout << endl;

    this->printRelocTable();

    cout << endl;
    cout << "----------------- MEMORY ---------------------" << endl;

    this->printOutput();

    this->writeToFile(outputFile);
}

void Assembler::instructionsInsert()
{

    Instruction *inst = new Instruction();

    inst->instructions = XCHG;
    inst->instruction = "xchg";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = XCHGB;
    inst->instruction = "xchgb";
    inst->operandSize = 1;
    inst->firstByte = "58";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = XCHGW;
    inst->instruction = "xchgw";
    inst->operandSize = 2;
    inst->firstByte = "5C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = MOV;
    inst->instruction = "mov";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = MOVB;
    inst->instruction = "movb";
    inst->operandSize = 1;
    inst->firstByte = "60";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = MOVW;
    inst->instruction = "movw";
    inst->operandSize = 2;
    inst->firstByte = "64";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = ADD;
    inst->instruction = "add";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = ADDB;
    inst->instruction = "addb";
    inst->operandSize = 1;
    inst->firstByte = "68";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = ADDW;
    inst->instruction = "addw";
    inst->operandSize = 2;
    inst->firstByte = "6C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SUB;
    inst->instruction = "sub";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SUBB;
    inst->instruction = "subb";
    inst->operandSize = 1;
    inst->firstByte = "70";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SUBW;
    inst->instruction = "subw";
    inst->operandSize = 2;
    inst->firstByte = "74";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = MUL;
    inst->instruction = "mul";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = MULB;
    inst->instruction = "mulb";
    inst->operandSize = 1;
    inst->firstByte = "78";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = MULW;
    inst->instruction = "mulw";
    inst->operandSize = 2;
    inst->firstByte = "7C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = DIV;
    inst->instruction = "div";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = DIVB;
    inst->instruction = "divb";
    inst->operandSize = 1;
    inst->firstByte = "80";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = DIVW;
    inst->instruction = "divw";
    inst->operandSize = 2;
    inst->firstByte = "84";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = CMP;
    inst->instruction = "cmp";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = CMPB;
    inst->instruction = "cmpb";
    inst->operandSize = 1;
    inst->firstByte = "88";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = CMPW;
    inst->instruction = "cmpw";
    inst->operandSize = 2;
    inst->firstByte = "8C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = NOT;
    inst->instruction = "not";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = NOTB;
    inst->instruction = "notb";
    inst->operandSize = 1;
    inst->firstByte = "90";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = NOTW;
    inst->instruction = "notw";
    inst->operandSize = 2;
    inst->firstByte = "94";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = AND;
    inst->instruction = "and";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = ANDB;
    inst->instruction = "andb";
    inst->operandSize = 1;
    inst->firstByte = "98";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = ANDW;
    inst->instruction = "andw";
    inst->operandSize = 2;
    inst->firstByte = "9C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = OR;
    inst->instruction = "or";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = ORB;
    inst->instruction = "orb";
    inst->operandSize = 1;
    inst->firstByte = "A0";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = ORW;
    inst->instruction = "orw";
    inst->operandSize = 2;
    inst->firstByte = "A4";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = XOR;
    inst->instruction = "xor";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = XORB;
    inst->instruction = "xorb";
    inst->operandSize = 1;
    inst->firstByte = "A8";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = XORW;
    inst->instruction = "xorw";
    inst->operandSize = 2;
    inst->firstByte = "AC";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = TEST;
    inst->instruction = "test";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = TESTB;
    inst->instruction = "testb";
    inst->operandSize = 1;
    inst->firstByte = "B0";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = TESTW;
    inst->instruction = "testw";
    inst->operandSize = 2;
    inst->firstByte = "B4";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SHL;
    inst->instruction = "shl";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SHLB;
    inst->instruction = "shlb";
    inst->operandSize = 1;
    inst->firstByte = "B8";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SHLW;
    inst->instruction = "shlw";
    inst->operandSize = 2;
    inst->firstByte = "BC";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SHR;
    inst->instruction = "shr";
    inst->operandSize = -1;
    inst->firstByte = "??";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SHRB;
    inst->instruction = "shrb";
    inst->operandSize = 1;
    inst->firstByte = "C0";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = SHRW;
    inst->instruction = "shrw";
    inst->operandSize = 2;
    inst->firstByte = "C4";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = INT;
    inst->instruction = "int";
    inst->operandSize = 2;
    inst->firstByte = "1C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = CALL;
    inst->instruction = "call";
    inst->operandSize = 2;
    inst->firstByte = "24";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = JMP;
    inst->instruction = "jmp";
    inst->operandSize = 2;
    inst->firstByte = "2C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = JEQ;
    inst->instruction = "jeq";
    inst->operandSize = 2;
    inst->firstByte = "34";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = JNE;
    inst->instruction = "jne";
    inst->operandSize = 2;
    inst->firstByte = "3C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = JGT;
    inst->instruction = "jgt";
    inst->operandSize = 2;
    inst->firstByte = "44";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = PUSH;
    inst->instruction = "push";
    inst->operandSize = 2;
    inst->firstByte = "4C";
    this->instructions.push_back(inst);

    inst = new Instruction();
    inst->instructions = POP;
    inst->instruction = "pop";
    inst->operandSize = 2;
    inst->firstByte = "54";
    this->instructions.push_back(inst);
}

int Assembler::tokenize(string line)
{

    regex globalExternRegex("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)?( *\\.(global|extern)) +(_|\\.)*[[:alpha:]]((_|\\.|\\$)|[[:alnum:]])*( *, *((_|\\.|\\$)*[[:alpha:]]((_|\\.|\\$)|[[:alnum:]])*))* *( #.*)?$");
    regex sectionRegex("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)?( *\\.section) +\\.?(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*:? *( #.*)?$");
    regex byteWordRegex("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)?( *\\.(byte|word)) +(((\\+)?(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-|\\+)?0x([[:digit:]]|[A-F])+)|((\\-|\\+)?[[:digit:]]+)|((\\-|\\+)?('.')))( *, *(((\\+)?(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-|\\+)?0x([[:digit:]]|[A-F])+)|((\\-|\\+)?[[:digit:]]+)|((\\-|\\+)?('.'))))* *( #.*)?$");
    regex labelRegex("^( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): *( #.*)?");
    regex skipRegex("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)?( *\\.skip) +([[:digit:]]+|0x([[:digit:]]|[A-F])+) *( #.*)?$");

    regex noOpInstRegex("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(halt|iret|ret) *(#.*)?$");

    regex twoOpInstRegex("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)? +((\\$?(((\\+)?(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-|\\+)?0x([[:digit:]]|[A-F])+)|((\\-|\\+)?[[:digit:]]+)|((\\-|\\+)?('.'))))|(%r[[:digit:]](h|l)?)|(\\(%r[[:digit:]]\\))|((((\\+)?(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-|\\-)?0x([[:digit:]]|[A-F])+)|((\\-|\\-)?[[:digit:]]+)|((\\-|\\-)?('.')))(\\(%r[0-6]\\)))) *, *((\\$?(((\\+)?(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-|\\-)?0x([[:digit:]]|[A-F])+)|((\\-|\\-)?[[:digit:]]+)|((\\-|\\-)?('.'))))|(%r[[:digit:]](h|l)?)|(\\(%r[[:digit:]]\\))|((((\\+)?(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-|\\-)?0x([[:digit:]]|[A-F])+)|((\\-|\\-)?[[:digit:]]+)|((\\-|\\-)?('.')))(\\(%r[0-6]\\)))) *(#.*)?$");

    /*  ^(( *(_|\.)*[[:alpha:]]((_|\.)|[[:alnum:]])*): +)? *(xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)? +((\$?(\-|\+)?(((_|\.)*[[:alpha:]]((_|\.)|[[:alnum:]])*)|(0x([[:digit:]]|[A-F])+)|[[:digit:]]+))|(%r[[:digit:]])|(\(%r[[:digit:]]\))|((\-|\+)?(((_|\.)*[[:alpha:]]((_|\.)|[[:alnum:]])*)|(0x([[:digit:]]|[A-F])+)|[[:digit:]]+)(\(%r[[:digit:]]\)))) *, *((\$?(\-|\+)?(((_|\.)*[[:alpha:]]((_|\.)|[[:alnum:]])*)|(0x([[:digit:]]|[A-F])+)|[[:digit:]]+))|(%r[[:digit:]])|(\(%r[[:digit:]]\))|((\-|\+)?(((_|\.)*[[:alpha:]]((_|\.)|[[:alnum:]])*)|(0x([[:digit:]]|[A-F])+)|[[:digit:]]+)(\(%r[[:digit:]]\))))$   */

    regex equRegex("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)?( *\\.equ) +(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])* *, *(\\+|\\-)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|(0x([[:digit:]]|[A-F])+|[[:digit:]]+|('.')))( *(\\+|\\-) *(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|(0x([[:digit:]]|[A-F])+|[[:digit:]]+|('.'))))* *( #.*)?$");

    regex oneOpInstJMPRegex1("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(int|call|jmp|jeq|jne|jgt) +\\*?((\\+)?((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?'.')) *(#.*)?$");
    regex oneOpInstJMPRegex2("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(int|call|jmp|jeq|jne|jgt) +((\\*((\\+)?((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?'.'))\\(%r[0-6]\\))|(\\*\\(%r[0-7]\\))|(\\*%r[0-7])) *(#.*)?$");

    regex oneOpInstPushPopRegex("^(( *(_|\\.)*[[:alpha:]](_|[[:alnum:]])*): +)? *(push|pop)(b|w)? +((\\$?(\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))|(%r[0-7](h|l)?)|(\\(%r[0-7]\\))|((\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.')))(\\(%(r[0-7]|pc)\\)))) *(#.*)?");

    regex pcRelRegexTwoOpsSrc("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)? +(((\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)(\\(%(r7|pc)\\))))) *, *((\\$?(\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))|(%r[[:digit:]](h|l)?)|(\\(%r[[:digit:]]\\))|((\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.')))(\\(%r[0-6]\\)))) *(#.*)?$");
    regex pcRelRegexTwoOpsDst("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)? +((\\$?(\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))|(%r[[:digit:]](h|l)?)|(\\(%r[[:digit:]]\\))|((\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.')))(\\(%r[0-6]\\)))) *, *((\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)(\\(%(r7|pc)\\)))) *(#.*)?$");
    regex pcRelRegexTwoOpsSrcDst("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)? +(((\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)(\\(%(r7|pc)\\))))) *, *((\\+)?(((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*)(\\(%(r7|pc)\\)))) *(#.*)?$");

    regex pcRelRegexOneOp("^(( *(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*): +)? *(int|call|jmp|jeq|jne|jgt) +((\\*((\\+)?((_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*))\\(%(r7|pc)\\))) *(#.*)?$");

    regex endRegex("^ *\\.end *(#.*)?$");

    regex blankLineRegex("^\\s*$");
    regex commentRegex("^ *#.*$");

    if (regex_match(line, blankLineRegex) || regex_match(line, commentRegex))
        return 2; //ako je cela linija komentar ili prazna ignorisi

    if (regex_match(line, globalExternRegex) || regex_match(line, sectionRegex) || regex_match(line, byteWordRegex) || regex_match(line, skipRegex) || regex_match(line, equRegex) || regex_match(line, labelRegex) || regex_match(line, noOpInstRegex) || regex_match(line, oneOpInstJMPRegex1) || regex_match(line, byteWordRegex) || regex_match(line, oneOpInstJMPRegex2) || regex_match(line, oneOpInstPushPopRegex) || regex_match(line, twoOpInstRegex) || regex_match(line, pcRelRegexOneOp) || regex_match(line, pcRelRegexTwoOpsDst) || regex_match(line, pcRelRegexTwoOpsSrc) || regex_match(line, pcRelRegexTwoOpsSrcDst) || regex_match(line, endRegex))
    {

        //regex token("[^ ,]+");
        //kupi 1 ili vise onoga sto nije blanko ili zarez ili..

        regex token("[^ ,]+");

        sregex_iterator currentMatch(line.begin(), line.end(), token);
        sregex_iterator lastMatch;

        while (currentMatch != lastMatch)
        {

            smatch match = *currentMatch;
            //cout<<match.str()<<endl;

            if (regex_match(match.str(), commentRegex)) //ako naidje # negde u samoj liniji
                break;

            currentMatch++;
            this->tokensInLine.push_back(match.str());
        }
    }
    else
    {
        cout << "INVALID SYNTAX\n"
             << endl;
        return 0;
    }
    return 1;
}

int Assembler::singleRun(string inputFile)
{

    ifstream inFile;

    string line;

    inFile.open(inputFile);

    if (!inFile.is_open())

    {
        cout << "ERROR!!! Input file cannot been opened." << endl;
        inFile.close();
        return 0;
    }

    else
    {
        while (!inFile.eof())
        {

            while (!this->tokensInLine.empty())
                this->tokensInLine.pop_front(); //ako je slucajno ostao neki token od prethodne linije ukloni ga

            getline(inFile, line);
            this->currentLine++;

            int retTokenize = this->tokenize(line); //popunjava tokensInLine tokenima iz tekuce linije

            if (retTokenize == 2)
                continue;

            else if (retTokenize == 0)
            {
                return 0;
            }

            else
            {

                regex twoOpInstRegex("(xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)(b|w)?");
                regex oneOpInstRegex("(int|call|jmp|jeq|jne|jgt|push|pop)");

            LABEL:
                string token = this->tokensInLine.front();

                this->tokensInLine.pop_front();

                if (token == ".global")
                {
                    if (this->globalExtern(0))
                        continue;
                    else
                        return 0;
                }
                else if (token == ".extern")
                {
                    if (this->globalExtern(1))
                        continue;
                    else
                        return 0;
                }
                else if (token == ".equ")
                {
                    if (this->equ())
                        continue;
                    else
                        return 0;
                }

                else if (token == ".skip")
                {
                    if (this->skip())
                        continue;
                    else
                        return 0;
                }

                else if (token == ".byte")
                {
                    if (this->byte())
                        continue;
                    else
                        return 0;
                }
                else if (token == ".section")
                {
                    if (this->section())
                        continue;
                    else
                        return 0;
                }
                else if (token[token.size() - 1] == ':')
                {
                    if (this->label(token))
                    {

                        if (this->tokensInLine.empty())
                            continue;
                        else
                            goto LABEL;
                    }
                    else
                        return 0;
                }
                else if (token == ".word")
                {

                    if (this->word())
                        continue;
                    else
                        return 0;
                }
                else if (token == "halt" || token == "iret" || token == "ret")
                {

                    if (this->noOpInstructions(token))
                        continue;
                    else
                        return 0;
                }
                else if (regex_match(token, twoOpInstRegex))
                {

                    if (this->twoOpInstructions(token))
                        continue;
                    else
                        return 0;
                }
                else if (regex_match(token, oneOpInstRegex))
                {

                    if (this->oneOpInstructions(token))
                        continue;
                    else
                        return 0;
                }
                else if (token == ".end")
                    return 1;
            }
        }

        cout << endl;
    }

    inFile.close();
    return 1;
}

int Assembler::globalExtern(int externBool)
{

    int size = this->tokensInLine.size();

    for (int i = 0; i < size; i++)
    {

        string tokenSymbol = this->tokensInLine.front();
        this->tokensInLine.pop_front();

        Symbol *symbol = this->findSymbol(tokenSymbol);

        if (symbol == 0)
        {
            symbol = new Symbol(tokenSymbol, "*UND*", -1, 'g', 0);

            if (externBool)
                symbol->externBool = 1;
            else
                symbol->globalBool = 1;

            this->pushSymbol(symbol);
        }
        else
        {

            if (symbol->localOrGlobal != 'g')
            {
                symbol->localOrGlobal = 'g';
                if (externBool)
                    symbol->externBool = 1;
                else
                    symbol->globalBool = 1;
            }
            else
            { //drugi put ulazi u .extern ili .global

                if (externBool)
                    symbol->externBool = 1;
                else
                    symbol->globalBool = 1;
            }
        }
    }

    return 1;
}

//directive

int Assembler::equ()
{

    regex equNoSymbols("^(\\+|\\-)?(0x([[:digit:]]|[A-F])+|[[:digit:]]+|('.'))( *(\\+|\\-) *(((0x([[:digit:]]|[A-F])+|[[:digit:]]+|('.')))))* *( #.*)?$");
    regex symbolRegex("^(\\+|\\-)(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*$");

    regex digitDec("^(\\+|\\-)[[:digit:]]+$");

    regex digitHex("^(\\+)0x([[:digit:]]|[A-F])+$");
    regex digitHexNeg("^(\\-)0x([[:digit:]]|[A-F])+$");
    regex character("^(\\+)('.')$");
    regex characterNeg("^(\\-)('.')$");

    string leftSymbol = this->tokensInLine.front();
    this->tokensInLine.pop_front();

    Symbol *symbol = this->findSymbol(leftSymbol);

    int newSymbol = 0;

    if (symbol == 0)
    {
        symbol = new Symbol();
        this->pushSymbol(symbol);
        newSymbol = 1;
    }

    if (symbol->defined)
    {
        cout << "Symbol is already defined" << endl;
        return 0;
    }

    symbol->isSectionSymbol = 0;
    symbol->defined = 1;
    symbol->section = "*ABS*";
    symbol->label = leftSymbol;

    string expression = this->tokensInLine.front();

    this->tokensInLine.pop_front();
    if (regex_match(expression, equNoSymbols))
    {

        string numString;
        int sum = 0;
        int i = 0;

        if (expression[0] != '-' && expression[0] != '+')
            expression.insert(0, 1, '+');

        while (i < expression.size())
        {

            int j = 0;
            if (expression[i] == '-')
                numString.insert(0, 1, '-');
            else

                numString.insert(0, 1, '+');

            i++;

            j++;

            while (expression[i] != '+' && expression[i] != '-' && i != expression.size())
            {

                numString.insert(j, 1, expression[i]);
                j++;
                i++;
            }

            if (regex_match(numString, digitDec))
            {

                int num = stoi(numString);
                sum += num;

                numString.clear();
            }
            else if (regex_match(numString, digitHex))
            {

                numString.erase(0, 3); //da obrise + i 0x

                int num = this->convertFromHexToDec(numString);

                sum += num;

                numString.clear();
            }
            else if (regex_match(numString, digitHexNeg))
            {

                numString.erase(0, 3); //da obrise - i 0x
                int num = this->convertFromHexToDec(numString);

                num = -num;
                sum += num;
                numString.clear();
            }
            else if (regex_match(numString, character))
            {

                numString.erase(0, 2);                    //da obrise + i '
                numString.erase(numString.size() - 1, 1); //da obrise ' na kraju

                char c = numString[0];
                int num = (int)c;

                sum += num;

                numString.clear();
            }
            else if (regex_match(numString, characterNeg))
            {

                numString.erase(0, 2);                    //da obrise - i '
                numString.erase(numString.size() - 1, 1); //da obrise ' na kraju

                char c = numString[0];
                int num = (int)c;
                num = -num;

                sum += num;

                numString.clear();
            }
        }

        symbol->offset = sum;

        this->symbolSortTable.push_back(symbol);
        symbol->isEquSymbol = 1;
    }
    else
    { //ima neki simbol u izrazu

        string numString;

        int i = 0;

        if (expression[0] != '-' && expression[0] != '+')
            expression.insert(0, 1, '+');

        while (i < expression.size())
        {

            int j = 0;
            if (expression[i] == '-')
                numString.insert(0, 1, '-');
            else

                numString.insert(0, 1, '+');

            i++;
            j++;

            while (expression[i] != '+' && expression[i] != '-' && i != expression.size())
            {

                numString.insert(j, 1, expression[i]);
                j++;
                i++;
            }

            symbol->equSymbols.push_back(numString);

            numString.clear();
        }

        symbol->offset = -10000000;
        this->TNS.push_back(symbol);
        this->tableOfSymbols.push_back(symbol);

        this->symbolSortTable.push_back(symbol);

        symbol->isEquSymbol = 1;
        symbol->unsolved = 1;
    }

    return 1;
}

int Assembler::skip()
{

    if (this->currentSection == "*UND*")
    {
        cout << "ERROR! No sectioon for memory content generation directive!" << endl;
        return 0;
    }

    string literal = this->tokensInLine.front();
    list<Output *> outputLine;
    int num = 0;
    int hexBool = 0;

    this->tokensInLine.pop_front();

    if (literal[0] == '0')
    {
        if (literal[1] == 'x')

        {
            literal.erase(0, 2);

            num = this->convertFromHexToDec(literal);

            hexBool = 1;
        }
    }

    if (hexBool == 0)
        num = stoi(literal);

    for (int i = 0; i < num; i++)
    {

        Output *output = new Output();
        output->locationCounter = LC;
        output->memory = "00";
        output->section = currentSection;
        outputLine.push_back(output);
        this->LC++;
    }

    this->PC += this->PC + num;

    this->output.push_back(outputLine);

    return 1;
}

int Assembler::byte()
{

    if (this->currentSection == "*UND*")
    {
        cout << "ERROR! Directive .byte must be in section!" << endl;
        return 0;
    }

    this->PC++;

    int size = this->tokensInLine.size();

    for (int i = 0; i < size; i++)
    {

        string token = tokensInLine.front();

        tokensInLine.pop_front();

        Output *output = new Output();
        list<Output *> outputLine;

        if (this->ImDiAd(token, 1, output, "R_X86_16_8"))
        {
            outputLine.push_back(output);
            this->output.push_back(outputLine);
        }
        else
            return 0;
    }
    return 1;
}

int Assembler::word()
{

    if (this->currentSection == "*UND*")
    {
        cout << "ERROR! Directive .word must be in section!" << endl;
        return 0;
    }

    this->PC++;
    this->PC++;

    int size = this->tokensInLine.size();

    for (int i = 0; i < size; i++)
    {

        string token = tokensInLine.front();

        tokensInLine.pop_front();

        Output *output = new Output();
        list<Output *> outputLine;

        if (this->ImDiAd(token, 2, output, "R_X86_16_16"))
        {
            outputLine.push_back(output);
            this->output.push_back(outputLine);
        }
        else
            return 0;
    }

    return 1;
}

int Assembler::label(string token)
{

    if (this->currentSection == "*UND*")
    {
        cout << "Label must be in section" << endl;
        return 0;
    }

    token.erase(token.size() - 1, 1);

    Symbol *symbol = this->findSymbol(token);

    if (symbol == 0)
    {

        Symbol *newSymbol = new Symbol();

        newSymbol->defined = 1;

        newSymbol->isSectionSymbol = 0;
        newSymbol->label = token;

        newSymbol->offset = this->LC;
        newSymbol->section = this->currentSection;

        this->symbolSortTable.push_back(newSymbol);

        this->tableOfSymbols.push_back(newSymbol);
    }
    else
    {

        if (symbol->defined == 1)
        {
            cout << "Symbol " << symbol->label << " is already defined";
            return 0;
        }

        symbol->defined = 1;

        symbol->isSectionSymbol = 0;
        symbol->label = token;

        symbol->offset = this->LC;
        symbol->section = this->currentSection;

        this->symbolSortTable.push_back(symbol);

        //cout<<symbol->label<<","<<symbol->idSymbol<<endl;
    }
    return 1;
}

int Assembler::section()
{

    if (currentSection != "*UND*")
    {
        Symbol *symbolCurrentSection = this->findSymbol(this->currentSection);
        symbolCurrentSection->lastLCInSection = this->LC;
        symbolCurrentSection->nextPCInSection = this->PC;
    } //sacuvaj lc koji treba da dodje sledeci u sekciji koju napustamo, u slucaju da ona opet dodje

    string token = this->tokensInLine.front();
    this->tokensInLine.pop_front();

    if (token[token.size() - 1] == ':')
        token.erase(token.size() - 1, 1);

    if (token == "*UND*")
    {
        cout << "*UND* is reserved for section name" << endl;
        return 0;
    }

    Symbol *symbol = this->findSymbol(token);

    if (symbol == 0)
    {

        this->LC = 0;
        this->PC = 0;

        this->currentSection = token;

        Symbol *newSymbol = new Symbol();

        newSymbol->defined = 1;

        newSymbol->isSectionSymbol = 1;
        newSymbol->label = token;

        newSymbol->offset = 0;
        newSymbol->section = this->currentSection;

        this->pushSymbol(newSymbol);

        this->sectionSortTable.push_back(newSymbol);
    }
    else
    {

        if (symbol->defined && !symbol->isSectionSymbol)
        {
            cout << "Symbol is already defined as label or equ symbol" << endl;
            return 0;
        }
        //ako je simbol vec definisan kao labela ne moze biti i kao sekcija

        if (symbol->defined)
        {
            this->LC = symbol->lastLCInSection;
            this->PC = symbol->nextPCInSection;
        } //ako je opet ista sekcija nastavi sa lc gde si stao
        else
            this->LC = 0;

        this->currentSection = token;

        if (symbol->defined == 0)
            this->sectionSortTable.push_back(symbol);

        symbol->defined = 1;

        symbol->isSectionSymbol = 1;
        symbol->label = token;

        symbol->offset = 0;
        symbol->section = this->currentSection;
    }

    return 1;
}

//instructions

int Assembler::noOpInstructions(string token)
{

    if (this->currentSection == "*UND*")
    {
        cout << "ERROR! Instruction must be in section!" << endl;
        return 0;
    }

    this->PC++;

    list<Output *> outputLine;
    Output *output = new Output();

    output->section = this->currentSection;
    output->locationCounter = this->LC;

    if (token == "halt")
        output->memory = "00";
    else if (token == "iret")
        output->memory = "08";
    else if (token == "ret")
        output->memory = "10";

    outputLine.push_back(output);
    this->output.push_back(outputLine);

    this->LC++;

    return 1;
}

int Assembler::twoOpInstructions(string instruction)
{

    if (this->currentSection == "*UND*")
    {
        cout << "ERROR! Instruction must be in section!" << endl;
        return 0;
    }

    list<Output *> outputLine;

    regex immedRegex("^(\\$(\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))$");

    regex regdirRegex("^%r[[:digit:]](h|l)?$");
    regex regindRegex("^\\(%r[[:digit:]]\\)$");
    regex memdirRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))$");
    regex regindpomRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))\\((%(r[[:digit:]]|pc))\\)$");

    regex pcRelRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)))(\\(%(r7|pc)\\))$");

    string src = this->tokensInLine.front();
    this->tokensInLine.pop_front();

    string dst = this->tokensInLine.front();
    this->tokensInLine.pop_front();

    //za svaku liniju sa twop instrukcijama dodaj instrukcije.. u toku obrada ce se instrukcije bez b ili w dobiti size koji ce biti iskoriscen za
    //dst operand

    if (regex_match(dst, immedRegex))
    {
        cout << "Imm operand cant't be destination" << endl;
        return 0;
    }

    if (regex_match(src, immedRegex) && (instruction == "xchg" || instruction == "xchgw" || instruction == "xchgb"))
    {
        cout << "In instruction xchg src operand can't be immed value!" << endl;
        return 0;
    }

    Output *outputFirstByte = new Output();

    list<Instruction *>::iterator it = this->instructions.begin();

    while ((*it)->instruction != instruction)
    {

        it++;
    }

    outputFirstByte->locationCounter = this->LC;
    outputFirstByte->section = this->currentSection;

    this->PC++; //za 1. bajt
    this->PC++; // za opDescr1
    this->PC++; //za opDescr2

    this->LC++;

    if ((regex_match(dst, memdirRegex) || regex_match(src, memdirRegex) || regex_match(src, regindRegex) || regex_match(dst, regindRegex) || regex_match(src, regindpomRegex) || regex_match(dst, regindpomRegex)) && (*it)->operandSize == -1 && (!regex_match(dst, regdirRegex) && !regex_match(src, regdirRegex)))
    {
        cout << "Error: ambiguous operand size for `" << (*it)->instruction << "'!" << endl;
        return 0;
    }

    //ako je jedan operand neko memorijsko adresiranje drugi mora biti regind ako hocemo da koristimo instrukciju bez b ili w

    int dstSize = -1;
    string pomSrc;
    string pomDst;

    Output *outputSrcOperand = new Output();
    Output *outputDstOperand = new Output();
    Output *imDiAdOutputSrc = new Output();
    Output *imDiAdOutputDst = new Output();
    ForwardRef *forwardRefSrc;
    ForwardRef *forwardRefDst;

    if (regex_match(dst, regdirRegex))
    {
        if (!this->regdir((*it), dst, 0, &dstSize, outputDstOperand))
            return 0;
    }
    else if (regex_match(dst, regindRegex))
    {
        if (!this->regind((*it), src, dst, 0, &dstSize, outputDstOperand))
            return 0;
    }
    else if (regex_match(dst, memdirRegex))
    {
        if (!this->memdir((*it), src, dst, 0, &dstSize, outputDstOperand))
            return 0;
        this->PC++;
        this->PC++;
    }
    else if (regex_match(dst, regindpomRegex))
    {
        if (!this->regindpom((*it), src, dst, 0, &dstSize, outputDstOperand, &pomDst))
            return 0;
        this->PC++;
        this->PC++;
    }
    else
    {
        cout << "Error: invalid adressing!" << endl;
        return 0;
    }

    //pc se inc za imDiAd deo dst operanda

    list<Instruction *>::iterator itHelp = it;

    if (dstSize == 1 && (*itHelp)->firstByte == "??")
    {
        itHelp++;
        outputFirstByte->memory = (*itHelp)->firstByte;
    }
    else if (dstSize == 2 && (*itHelp)->firstByte == "??")
    {
        itHelp++;
        itHelp++;
        outputFirstByte->memory = (*itHelp)->firstByte;
    }
    else
        outputFirstByte->memory = (*itHelp)->firstByte;

    if (regex_match(src, immedRegex))
    {
        if (!this->immed((*it), src, &dstSize, outputSrcOperand))
            return 0;
        this->PC += dstSize;
    }
    else if (regex_match(src, regdirRegex))
    {
        if (!this->regdir((*it), src, 1, &dstSize, outputSrcOperand))
            return 0;
    }
    else if (regex_match(src, regindRegex))
    {
        if (!this->regind((*it), src, dst, 1, &dstSize, outputSrcOperand))
            return 0;
    }
    else if (regex_match(src, memdirRegex))
    {
        if (!this->memdir((*it), src, dst, 1, &dstSize, outputSrcOperand))
            return 0;
        this->PC++;
        this->PC++;
    }
    else if (regex_match(src, regindpomRegex))
    {
        if (!this->regindpom((*it), src, dst, 1, &dstSize, outputSrcOperand, &pomSrc))
            return 0;
        this->PC++;
        this->PC++;
    }
    else
    {
        cout << "Error: invalid adressing!" << endl;
        return 0;
    }

    //pc se inc za imDiAd deo src operanda

    outputSrcOperand->locationCounter = this->LC;
    this->LC++;

    if (outputSrcOperand->adressing == "memdir")
        this->ImDiAd(src, 2, imDiAdOutputSrc, "R_X86_16_16");
    else if (outputSrcOperand->adressing == "immed")
    {
        src.erase(0, 1);
        string relocType;
        if (dstSize == 1)
            relocType = "R_X86_16_8";
        else
            relocType = "R_X86_16_16";
        this->ImDiAd(src, dstSize, imDiAdOutputSrc, relocType);
    }
    else if (outputSrcOperand->adressing == "regindpom")
    {
        if (regex_match(src, pcRelRegex))
        {
            imDiAdOutputSrc->pcRel = 1;
            imDiAdOutputSrc->PC = this->PC;
            this->ImDiAd(pomSrc, 2, imDiAdOutputSrc, "R_X86_16_PC16");
        }
        else
            this->ImDiAd(pomSrc, 2, imDiAdOutputSrc, "R_X86_16_16");
    }

    outputDstOperand->locationCounter = this->LC;
    this->LC++;

    if (outputDstOperand->adressing == "memdir")
        this->ImDiAd(dst, 2, imDiAdOutputDst, "R_X86_16_16");
    else if (outputDstOperand->adressing == "regindpom")
    {

        if (regex_match(dst, pcRelRegex))
        {
            imDiAdOutputDst->pcRel = 1;
            imDiAdOutputDst->PC = this->PC;
            this->ImDiAd(pomDst, 2, imDiAdOutputDst, "R_X86_16_PC16");
        }

        else
            this->ImDiAd(pomDst, 2, imDiAdOutputDst, "R_X86_16_16");
    }

    outputLine.push_back(outputFirstByte);
    outputLine.push_back(outputSrcOperand);
    if (outputSrcOperand->adressing != "?")
        outputLine.push_back(imDiAdOutputSrc);
    outputLine.push_back(outputDstOperand);
    if (outputDstOperand->adressing != "?")
        outputLine.push_back(imDiAdOutputDst);

    this->output.push_back(outputLine);

    //if (regex_match(src,pcRelRegex)){imDiAdOutputSrc->pcRel=1; imDiAdOutputSrc->PC=this->PC;}
    //if (regex_match(dst,pcRelRegex)){imDiAdOutputDst->pcRel=1; imDiAdOutputDst->PC=this->PC;}

    return 1;
}

int Assembler::oneOpInstructions(string instruction)
{

    if (this->currentSection == "*UND*")
    {
        cout << "ERROR! Instruction must bi in section!" << endl;
        return 0;
    }

    list<Output *> outputLine;

    regex immedJMPRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))$");

    regex regdirJMPRegex("^\\*%r[[:digit:]](h|l)?$");
    regex regindJMPRegex("^\\*\\(%r[[:digit:]]\\)$");
    regex memdirJMPRegex("^(\\*(\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))$");
    regex regindpomJMPRegex("^(\\*(\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))\\((%r[[:digit:]]|%pc)\\)$");

    regex immedRegex("^(\\$(\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))$");

    regex regdirRegex("^%r[[:digit:]](h|l)?$");
    regex regindRegex("^\\(%r[[:digit:]]\\)$");
    regex memdirRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))$");
    regex regindpomRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))\\((%r[[:digit:]]|%pc)\\)$");

    string operand = this->tokensInLine.front();
    this->tokensInLine.pop_front();

    Output *outputFirstByte = new Output();

    list<Instruction *>::iterator it = this->instructions.begin();

    while ((*it)->instruction != instruction)
    {

        it++;
    }

    outputFirstByte->section = this->currentSection;
    outputFirstByte->memory = (*it)->firstByte;
    outputFirstByte->locationCounter = this->LC;

    outputLine.push_back(outputFirstByte);
    this->LC++;

    this->PC++; //za 1. bajt
    this->PC++; // za op descr

    Output *operandDescrOutput = new Output();
    Output *imDiAdOutput = new Output();

    if ((*it)->instructions == PUSH || (*it)->instructions == POP)
    {

        if (regex_match(operand, immedRegex) && (*it)->instructions == POP)
        {
            cout << "POP instruction can't have immed adressing!" << endl;
            return 0;
        }

        if (regex_match(operand, immedRegex))
        {

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;
            operandDescrOutput->memory = "00";

            outputLine.push_back(operandDescrOutput);
            this->LC++;
            operand.erase(0, 1);

            this->ImDiAd(operand, 2, imDiAdOutput, "R_X86_16_16");

            this->PC++;
            this->PC++; //za imdiad

            outputLine.push_back(imDiAdOutput);
            this->output.push_back(outputLine);

            return 1;
        }
        else if (regex_match(operand, regdirRegex))
        {

            regex regdir("^%r[0-7](h|l)?$");

            if (!regex_match(operand, regdir))
            {
                cout << "There are only 8 registers from r0 to r7!" << endl;
                return 0;
            }

            int regNum = (int)operand[2] - 48;

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;

            if (operand[3] == 'h')
            {

                cout << "Value of operand is extended to 2B!" << endl;

                regNum = regNum << 1;

                regNum++;
                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                operandDescrOutput->memory = hex;
            }
            else
            {

                if (operand[3] == 'l')
                    cout << "Value of operand is extended to 2B!" << endl;

                regNum = regNum << 1;

                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                operandDescrOutput->memory = hex;
            }

            outputLine.push_back(operandDescrOutput);
            this->LC++;

            this->output.push_back(outputLine);

            return 1;
        }
        else if (regex_match(operand, regindpomRegex))
        {

            regex symbolRegex("^(\\+)?(_|\\.)*[[:alpha:]]((_|\\.|\\$)|[[:alnum:]])*$");
            regex regindpomRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))\\((%(r[0-7]|pc))\\)$");

            if (!regex_match(operand, regindpomRegex))
            {
                cout << "There is no such register!" << endl;
                return 0;
            }

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;

            int i = 0;
            string pom;

            while (operand[i] != '(')
            {

                pom.insert(i, 1, operand[i]);

                i++;
            }
            this->PC++;
            this->PC++; //za imdiad

            int j = i + 3; //num je na toj poziciji ako nije pc
            int regNum;

            if (operand[i + 2] == 'p' || operand[j] == '7')
            {
                if (!regex_match(pom, symbolRegex))
                {
                    cout << "Pcrel must have symbol as offset!" << endl;
                    return 0;
                }
                imDiAdOutput->pcRel = 1;
                regNum = 7;
                imDiAdOutput->PC = this->PC;
            }
            else
                regNum = (int)operand[j] - 48;

            regNum = regNum << 1;
            regNum += 96;

            string hex = this->convertFromDecToHex(regNum);

            operandDescrOutput->memory = hex;

            outputLine.push_back(operandDescrOutput);
            this->LC++;

            if (operand[i + 2] == 'p' || operand[j] == '7')
                this->ImDiAd(pom, 2, imDiAdOutput, "R_X86_16_PC16");
            else
                this->ImDiAd(pom, 2, imDiAdOutput, "R_X86_16_16");

            outputLine.push_back(imDiAdOutput);
            this->output.push_back(outputLine);

            return 1;
        }
        else if (regex_match(operand, memdirRegex))
        {

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;
            operandDescrOutput->memory = "80";
            outputLine.push_back(operandDescrOutput);
            this->LC++;

            this->ImDiAd(operand, 2, imDiAdOutput, "R_X86_16_16");

            outputLine.push_back(imDiAdOutput);
            this->output.push_back(outputLine);

            this->PC++;
            this->PC++; //za imdiad

            return 1;
        }
        else if (regex_match(operand, regindRegex))
        {

            regex regindRegex("^\\(%r[0-7]\\)$");

            if (!regex_match(operand, regindRegex))
            {
                cout << "Regsters are from 0-7!" << endl;
                return 0;
            }

            int regNum = (int)operand[3] - 48;

            regNum = regNum << 1;
            regNum += 64;

            string hex = this->convertFromDecToHex(regNum);

            operandDescrOutput->memory = hex;
            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;
            outputLine.push_back(operandDescrOutput);
            this->LC++;

            this->output.push_back(outputLine);

            return 1;
        }
    }
    else
    { // jmp instrukcije

        if (regex_match(operand, immedJMPRegex))
        {

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;
            operandDescrOutput->memory = "00";

            outputLine.push_back(operandDescrOutput);
            this->LC++;

            this->ImDiAd(operand, 2, imDiAdOutput, "R_X86_16_16");

            this->PC++;
            this->PC++; //za imdiad

            outputLine.push_back(imDiAdOutput);
            this->output.push_back(outputLine);

            return 1;
        }
        else if (regex_match(operand, regdirJMPRegex))
        {

            operand.erase(0, 1);

            regex regdir("^%r[0-7](h|l)?$");

            if (!regex_match(operand, regdir))
            {
                cout << "There are only 8 registers from r0 to r7!" << endl;
                return 0;
            }

            int regNum = (int)operand[2] - 48;

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;

            if (operand[3] == 'h')
            {

                cout << "Value of operand is extended to 2B!" << endl;

                regNum = regNum << 1;

                regNum++;
                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                operandDescrOutput->memory = hex;
            }
            else
            {

                if (operand[3] == 'l')
                    cout << "Value of operand is extended to 2B!" << endl;

                regNum = regNum << 1;

                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                operandDescrOutput->memory = hex;
            }

            outputLine.push_back(operandDescrOutput);
            this->LC++;

            this->output.push_back(outputLine);

            return 1;
        }
        else if (regex_match(operand, regindpomJMPRegex))
        {

            operand.erase(0, 1); // da obrise *

            regex regindpomRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))\\(%(r[0-7]|pc)\\)$");

            if (!regex_match(operand, regindpomRegex))
            {
                cout << "There is no such register!" << endl;
                return 0;
            }

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;

            int i = 0;
            string pom;

            while (operand[i] != '(')
            {

                pom.insert(i, 1, operand[i]);
                i++;
            }

            this->PC++;
            this->PC++; //za imdiad

            int j = i + 3; //num je na toj poziciji ako nije pc
            int regNum;

            if (operand[i + 2] == 'p' || operand[j] == '7')
            {
                imDiAdOutput->pcRel = 1;
                regNum = 7;
                imDiAdOutput->PC = this->PC;
            }
            else
                regNum = (int)operand[j] - 48;

            regNum = regNum << 1;
            regNum += 96;

            string hex = this->convertFromDecToHex(regNum);

            operandDescrOutput->memory = hex;

            outputLine.push_back(operandDescrOutput);
            this->LC++;

            if (operand[i + 2] == 'p' || operand[j] == '7')
                this->ImDiAd(pom, 2, imDiAdOutput, "R_X86_16_PC16");
            else
                this->ImDiAd(pom, 2, imDiAdOutput, "R_X86_16_16");

            outputLine.push_back(imDiAdOutput);
            this->output.push_back(outputLine);

            return 1;
        }
        else if (regex_match(operand, memdirJMPRegex))
        {

            operand.erase(0, 1); // da obrise *

            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;
            operandDescrOutput->memory = "80";
            outputLine.push_back(operandDescrOutput);
            this->LC++;

            this->ImDiAd(operand, 2, imDiAdOutput, "R_X86_16_16");

            outputLine.push_back(imDiAdOutput);
            this->output.push_back(outputLine);

            this->PC++;
            this->PC++; //za imdiad

            return 1;
        }
        else if (regex_match(operand, regindJMPRegex))
        {

            operand.erase(0, 1); // da obrise *

            regex regindRegex("^\\(%r[0-7]\\)$");

            if (!regex_match(operand, regindRegex))
            {
                cout << "Regsters are from 0-7!" << endl;
                return 0;
            }

            int regNum = (int)operand[3] - 48;

            regNum = regNum << 1;
            regNum += 64;

            string hex = this->convertFromDecToHex(regNum);

            operandDescrOutput->memory = hex;
            operandDescrOutput->locationCounter = this->LC;
            operandDescrOutput->section = this->currentSection;
            outputLine.push_back(operandDescrOutput);
            this->LC++;

            this->output.push_back(outputLine);

            return 1;
        }
    }

    return 1;
}

int Assembler::immed(Instruction *inst, string operand, int *dstSize, Output *outputOpDescr)
{

    outputOpDescr->memory = "00";
    outputOpDescr->section = this->currentSection;

    outputOpDescr->adressing = "immed";

    return 1;
}

int Assembler::regdir(Instruction *inst, string operand, int isSrc, int *dstSize, Output *outputOpDescr)
{

    regex regdir("^%r[0-7](h|l)?$");

    if (!regex_match(operand, regdir))
    {
        cout << "There are only 8 registers from r0 to r7!" << endl;
        return 0;
    }

    int regNum = (int)operand[2] - 48;

    if (isSrc)
    {

        if (*dstSize == 1)
        {

            if (operand[3] == 'h')
            {

                regNum = regNum << 1;

                regNum++;
                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
            }

            else
            {

                if (operand[3] != 'l')
                    cout << "Value of src operand is truncated to 1B!" << endl;

                regNum = regNum << 1;

                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
            }
        }

        else if (*dstSize == 2)
        {

            if (operand[3] == 'h')
            {

                cout << "Value of operand is extended to 2B!" << endl;

                regNum = regNum << 1;

                regNum++;
                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
            }
            else
            {

                if (operand[3] == 'l')
                    cout << "Value of operand is extended to 2B!" << endl;

                regNum = regNum << 1;

                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
            }
        }
    }

    else if (!isSrc)
    {

        if (inst->operandSize == 1)
        { //instrukcija sa b

            if (operand[3] == 'h')
            {

                regNum = regNum << 1;

                regNum++;

                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
                *dstSize = 1;
            }
            else
            {
                if (operand[3] != 'l')
                    cout << "Value is truncated to 1B!" << endl;

                regNum = regNum << 1;
                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;

                *dstSize = 1;
            }
        }

        else if (inst->operandSize == 2)
        { //instrukcija sa w

            if (operand[3] == 'h')
            {

                cout << "Value of src is truncated to 1B!" << endl;

                regNum = regNum << 1;

                regNum++;

                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
                *dstSize = 2;
            }
            else
            {

                if (operand[3] == 'l')
                    cout << "Value of src is truncated to 1B!" << endl;

                regNum = regNum << 1;
                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;

                *dstSize = 2;
            }
        }

        else if (inst->operandSize == -1)
        { //za instrukcije bez b i w

            if (operand[3] == 'h')
            {

                regNum = regNum << 1;

                regNum++;

                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
                *dstSize = 1;
            }
            else
            {

                if (operand[3] == 'l')
                    *dstSize = 1;
                else
                    *dstSize = 2;
                regNum = regNum << 1;
                regNum += 32;

                string hex = this->convertFromDecToHex(regNum);

                outputOpDescr->memory = hex;
            }
        }
    }

    outputOpDescr->section = this->currentSection;

    return 1;
}

int Assembler::regind(Instruction *inst, string src, string dst, int isSrc, int *dstSize, Output *outputOpDescr)
{

    regex regind("^\\(%r[0-7]\\)$");

    if (!isSrc)
    {

        if (!regex_match(dst, regind))
        {
            cout << "Regsters are from 0-7!" << endl;
            return 0;
        }

        if (inst->operandSize == 1)
        {

            *dstSize = 1;
        }
        else if (inst->operandSize == 2)
        {

            *dstSize = 2;
        }
        else if (inst->operandSize == -1)
        { //ako je instrukcija bez b ili w znaci da je src regdir

            if (src[3] != 'h' && src[3] != 'l')
                *dstSize = 2;
            else
                *dstSize = 1;
        }

        int regNum = (int)dst[3] - 48;

        regNum = regNum << 1;
        regNum += 64;

        string hex = this->convertFromDecToHex(regNum);

        outputOpDescr->memory = hex;
    }
    else if (isSrc)
    {

        if (!regex_match(src, regind))
        {
            cout << "Regsters are from 0-7!" << endl;
            return 0;
        }

        int regNum = (int)src[3] - 48;

        regNum = regNum << 1;
        regNum += 64;

        string hex = this->convertFromDecToHex(regNum);

        outputOpDescr->memory = hex;
    }

    outputOpDescr->section = this->currentSection;

    return 1;
}

int Assembler::regindpom(Instruction *inst, string src, string dst, int isSrc, int *dstSize, Output *outputOpDescr, string *pom)
{

    regex regindpomRegex("^((\\+)?(((_|\\.)*[[:alpha:]]((\\$|_|\\.)|[[:alnum:]])*)|((\\-)?0x([[:digit:]]|[A-F])+)|((\\-)?[[:digit:]]+)|((\\-)?('.'))))\\((%(r[0-7]|pc))\\)$");

    if (!isSrc)
    {

        if (!regex_match(dst, regindpomRegex))
        {
            cout << "There is no such register!" << endl;
            return 0;
        }

        if (inst->operandSize == 1)
            *dstSize = 1;
        else if (inst->operandSize == 2)
            *dstSize = 2;
        else if (inst->operandSize == -1)
        { //znaci da je src reg inace bi bila greska

            if (src[3] != 'h' && src[3] != 'l')
                *dstSize = 2;
            else
                *dstSize = 1;
        }

        int i = 0;

        while (dst[i] != '(')
        {

            (*pom).insert(i, 1, dst[i]);
            i++;
        }

        int j = i + 3; //num je na toj poziciji od prve zagrade

        int regNum;

        if (dst[i + 2] == 'p' || dst[j] == '7')
            regNum = 7;
        else
            regNum = (int)dst[j] - 48;

        regNum = regNum << 1;
        regNum += 96;

        string hex = this->convertFromDecToHex(regNum);

        outputOpDescr->memory = hex;
    }
    else if (isSrc)
    {

        if (!regex_match(src, regindpomRegex))
        {
            cout << "There is no such register!" << endl;
            return 0;
        }

        int i = 0;

        while (src[i] != '(')
        {

            (*pom).insert(i, 1, src[i]);
            i++;
        }

        int j = i + 3; //num je na toj poziciji od prve zagrade ako nije pc

        int regNum;

        if (src[i + 2] == 'p' || src[j] == '7')
            regNum = 7;
        else
            regNum = (int)src[j] - 48;

        regNum = regNum << 1;
        regNum += 96;

        string hex = this->convertFromDecToHex(regNum);

        outputOpDescr->memory = hex;
    }

    outputOpDescr->section = this->currentSection;
    outputOpDescr->adressing = "regindpom";

    return 1;
}

int Assembler::memdir(Instruction *inst, string src, string dst, int isSrc, int *dstSize, Output *outputOpDescr)
{

    if (!isSrc)
    {

        if (inst->operandSize == 1)
            *dstSize = 1;
        else if (inst->operandSize == 2)
            *dstSize = 2;
        else if (inst->operandSize == -1)
        { //znaci da je src reg inace bi bila greska

            if (src[3] != 'h' && src[3] != 'l')
                *dstSize = 2;
            else
                *dstSize = 1;
        }

        outputOpDescr->memory = "80";
    }
    else if (isSrc)
    {

        outputOpDescr->memory = "80";
    }

    outputOpDescr->section = this->currentSection;

    outputOpDescr->adressing = "memdir";

    return 1;
}

int Assembler::ImDiAd(string operand, int imDiAdSize, Output *outputOpDescr, string relocType)
{

    regex symbolRegex("^(\\+)?(_|\\.)*[[:alpha:]]((_|\\.|\\$)|[[:alnum:]])*$");

    regex digitDec("^(\\+)?[[:digit:]]+$");
    regex digitDecNeg("^(\\-)[[:digit:]]+$");
    regex digitHex("^(\\+)?0x([[:digit:]]|[A-F])+$");
    regex digitHexNeg("^(\\-)0x([[:digit:]]|[A-F])+$");
    regex character("^(\\+)?('.')$");
    regex characterNeg("^(\\-)('.')$");

    if (regex_match(operand, symbolRegex))
    {

        if (operand[0] == '+')
            operand.erase(0, 1);

        Symbol *symbol = this->findSymbol(operand);

        if (symbol == 0)
        {

            symbol = new Symbol(operand, "*UND*", -1, 'l', 0);

            this->pushSymbol(symbol);
        }

        RelocElem *reloc = new RelocElem();

        reloc->section = this->currentSection;
        reloc->type = relocType;
        reloc->value = -1; // ne znam da li ce biti lokalan ili globalan.. pa da li ce ici id sekcije iz def ili ce ici
                           //sam id tog simbola
        reloc->locationCounter = this->LC;
        this->relocTable.push_back(reloc);

        ForwardRef *fr = new ForwardRef();

        fr->LC = this->LC;
        fr->section = this->currentSection;

        symbol->forwardRefsTable.push_back(fr);

        outputOpDescr->locationCounter = this->LC;
        outputOpDescr->section = this->currentSection;

        if (imDiAdSize == 1)
        { // u slucaju kad je simbol immed adresiranja

            fr->imDiAdSize = 1; //sluzi posle kada radi fix da zna na koliko je bajta
            outputOpDescr->memory = "??";
            this->LC++;
        }

        else if (imDiAdSize == 2)
        {

            fr->imDiAdSize = 2;
            outputOpDescr->memory = "?? ??";

            this->LC++;
            this->LC++;
        }

    } //kraj za symbol

    else if (regex_match(operand, digitDec))
    {

        if (operand[0] == '+')
            operand.erase(0, 1);
        int num = stoi(operand); //gubi sve eventualne nule ispred
        string hex = convertFromDecToHex(num);

        string help = hex;

        if (imDiAdSize == 1)
        {

            if (num > 255)
            {
                while (hex.size() > 2)
                    hex.erase(0, 1);
                cout << "Value " << help << " truncated to " << hex << endl;
            }

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            if (num <= 15)
                hex.insert(0, 1, '0');
            outputOpDescr->memory = hex;

            this->LC++;
        }

        else if (imDiAdSize == 2)
        {

            if (num > 65535)
            {
                while (hex.size() > 4)
                    hex.erase(0, 1);
                cout << "Value " << help << " truncated to " << hex << endl;
            }

            outputOpDescr->locationCounter = this->LC;

            outputOpDescr->section = this->currentSection;

            if (num <= 15)
            {
                hex.insert(0, 3, '0');
            }
            else if (num <= 255)
            {
                hex.insert(0, 2, '0');
            }
            else if (num <= 4095)
            {
                hex.insert(0, 1, '0');
            }

            string help1 = "  ";
            string help2 = "  ";

            help1[0] = hex[2];
            help1[1] = hex[3];
            help2[0] = hex[0];
            help2[1] = hex[1];
            hex = help1 + help2;

            hex.insert(2, 1, ' ');

            outputOpDescr->memory = hex;

            this->LC++;
            this->LC++;
        }
    } //kraj digitDec

    else if (regex_match(operand, digitHex))
    {

        if (operand[0] == '+')
            operand.erase(0, 1);

        operand.erase(0, 2); //skini 0x

        int dec = this->convertFromHexToDec(operand); //da izgubi eventualne pocetne nule
        operand = this->convertFromDecToHex(dec);

        string help = operand;

        if (imDiAdSize == 1)
        {

            if (dec > 255)
            {
                while (operand.size() > 2)
                    operand.erase(0, 1);
                cout << "Value " << help << " truncated to " << operand << endl;
            }

            if (dec <= 15)
                operand.insert(0, 1, '0');
            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            outputOpDescr->memory = operand;

            this->LC++;
        }

        else if (imDiAdSize == 2)
        {

            if (dec > 65535)
            {
                while (operand.size() > 4)
                    operand.erase(0, 1);
                cout << "Value " << help << " truncated to " << operand << endl;
            }

            if (dec <= 15)
            {
                operand.insert(0, 3, '0');
            }
            else if (dec <= 255)
            {
                operand.insert(0, 2, '0');
            }
            else if (dec <= 4095)
            {
                operand.insert(0, 1, '0');
            }

            string help1 = "  ";
            string help2 = "  ";

            help1[0] = operand[2];
            help1[1] = operand[3];
            help2[0] = operand[0];
            help2[1] = operand[1];
            operand = help1 + help2;

            operand.insert(2, 1, ' ');

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            outputOpDescr->memory = operand;

            this->LC++;
            this->LC++;
        }

    } //kraj digitHex

    else if (regex_match(operand, digitDecNeg))
    {

        operand.erase(0, 1); //skine se -

        int digit = stoi(operand);

        if (imDiAdSize == 1)
        {

            if (digit == 0)
            {
                outputOpDescr->memory = "00";

                outputOpDescr->locationCounter = this->LC;
                outputOpDescr->section = currentSection;
                this->LC++;

                return 1;
            }

            unsigned char byte = ~digit + 1;
            int digitHelp = digit;

            digit = byte;

            string hex = convertFromDecToHex(digit);
            string help = hex;
            if (digitHelp > 128)
            {
                while (hex.size() > 2)
                    hex.erase(0, 1);
                cout << "Value " << help << " truncated to " << hex << endl;
            }

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            outputOpDescr->memory = hex;

            this->LC++;

            //brojkevi idu od -128 do 127.. Mi ovde gledamo samo -128 do -1.. Sve iznad -128 tj 128 se skracuje
        }
        else if (imDiAdSize == 2)
        {

            if (digit > 32768)
            {
                cout << "Value can't fit" << endl;
                return 0;
            }

            if (digit == 0)
            {
                outputOpDescr->memory = "00 00";
                this->LC++;
                this->LC++;

                outputOpDescr->locationCounter = this->LC;
                outputOpDescr->section = currentSection;

                return 1;
            }

            string binary = this->convertFromDecToBin(digit);

            string twosComplement = this->findTwosComplement(binary);

            int digitHelp = stoi(twosComplement, nullptr, 2);

            string hex = convertFromDecToHex(digitHelp);

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            string help1 = "  ";
            string help2 = "  ";

            help1[0] = hex[2];
            help1[1] = hex[3];
            help2[0] = hex[0];
            help2[1] = hex[1];
            hex = help1 + help2;

            hex.insert(2, 1, ' ');

            outputOpDescr->memory = hex;

            this->LC++;
            this->LC++;

            //brojkevi idu od -32768 do 32767.. Mi ovde gledamo samo -128 do -1.. Sve iznad -128 tj 128 se skracuje
        }

    } //kraj digitDecNeg

    else if (regex_match(operand, digitHexNeg))
    {

        operand.erase(0, 3);

        int dec = this->convertFromHexToDec(operand);

        if (imDiAdSize == 1)
        {

            if (dec == 0)
            {
                outputOpDescr->memory = "00";
                this->LC++;
                outputOpDescr->section = this->currentSection;

                return 1;
            }

            unsigned char byte = ~dec + 1;

            int digitHelp = dec;

            dec = byte;

            string hex = convertFromDecToHex(dec);
            string help = hex;
            if (digitHelp > 128)
            {
                while (hex.size() > 2)
                    hex.erase(0, 1);
                cout << "Value " << help << " truncated to " << hex << endl;
            }

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            outputOpDescr->memory = hex;

            this->LC++;
        }

        else if (imDiAdSize == 2)
        {

            operand = this->convertFromDecToHex(dec);

            if (dec > 32768)
            {
                cout << "Value can't fit" << endl;
                return 0;
            }

            if (dec == 0)
            {
                outputOpDescr->memory = "00 00";
                this->LC++;
                this->LC++;

                outputOpDescr->section = this->currentSection;
                return 1;
            } // zbog -0

            string binary = this->convertFromDecToBin(dec);

            string twosComplement = this->findTwosComplement(binary);

            int digitHelp = stoi(twosComplement, nullptr, 2);

            string hex = convertFromDecToHex(digitHelp);

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            string help1 = "  ";
            string help2 = "  ";

            help1[0] = hex[2];
            help1[1] = hex[3];
            help2[0] = hex[0];
            help2[1] = hex[1];
            hex = help1 + help2;

            hex.insert(2, 1, ' ');

            outputOpDescr->memory = hex;

            this->LC++;

            this->LC++;
        }
    } //kraj za digitHexNeg

    else if (regex_match(operand, character))
    {

        if (operand[0] == '+')
            operand.erase(0, 1);

        operand.erase(0, 1);
        operand.erase(operand.size() - 1, 1);

        int num = operand[0];

        string hex = convertFromDecToHex(num);
        string help = hex;

        if (imDiAdSize == 1)
        {

            if (num > 255)
            {
                while (hex.size() > 2)
                    hex.erase(0, 1);
                cout << "Value " << help << " truncated to " << hex << endl;
            }

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            if (num <= 15)
                hex.insert(0, 1, '0');
            outputOpDescr->memory = hex;

            this->LC++;
        }

        if (imDiAdSize == 2)
        {

            if (num > 255)
            {
                while (hex.size() > 2)
                    hex.erase(0, 1);
                cout << "Value " << help << " truncated to " << hex << endl;
            }

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            if (num <= 15)
                hex.insert(0, 1, '0');
            outputOpDescr->memory = hex;

            if (num <= 15)
            {
                hex.insert(0, 3, '0');
            }
            else if (num <= 255)
            {
                hex.insert(0, 2, '0');
            }
            else if (num <= 4095)
            {
                hex.insert(0, 1, '0');
            }

            string help1 = "  ";
            string help2 = "  ";

            help1[0] = hex[2];
            help1[1] = hex[3];
            help2[0] = hex[0];
            help2[1] = hex[1];
            hex = help1 + help2;

            hex.insert(2, 1, ' ');

            outputOpDescr->memory = hex;

            this->LC++;
            this->LC++;
        }

    } //kraj za character

    else if (regex_match(operand, characterNeg))
    {

        operand.erase(0, 2);
        operand.erase(operand.size() - 1, 1);

        int digit = operand[0];

        if (imDiAdSize == 1)
        {

            unsigned char byte = ~digit + 1;
            int digitHelp = digit;
            digit = byte;

            string hex = convertFromDecToHex(digit);
            string help = hex;
            if (digitHelp > 128)
            {
                while (hex.size() > 2)
                    hex.erase(0, 1);
                cout << "Value " << help << " truncated to " << hex << endl;
            }

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            outputOpDescr->memory = hex;

            this->LC++;
        }
        else if (imDiAdSize == 2)
        {

            if (digit > 32768)
            {
                cout << "Value can't fit" << endl;
                return 0;
            }

            string binary = this->convertFromDecToBin(digit);

            string twosComplement = this->findTwosComplement(binary);

            int digitHelp = stoi(twosComplement, nullptr, 2);

            string hex = convertFromDecToHex(digitHelp);

            outputOpDescr->locationCounter = this->LC;
            outputOpDescr->section = this->currentSection;

            string help1 = "  ";
            string help2 = "  ";

            help1[0] = hex[2];
            help1[1] = hex[3];
            help2[0] = hex[0];
            help2[1] = hex[1];
            hex = help1 + help2;

            hex.insert(2, 1, ' ');

            outputOpDescr->memory = hex;

            this->LC++;

            this->LC++;
        }

    } //kraj za charNeg

    return 1;
}

int Assembler::equResolve()
{

    list<Symbol *>::iterator it;
    struct ClassificationIndex
    {
        string section;
        int index;
        ClassificationIndex(string section, int index)
        {
            this->section = section;
            this->index = index;
        }
    };

    regex symbolRegex("^(\\+|\\-)(_|\\.)*[[:alpha:]]((_|\\.)|[[:alnum:]])*$");
    regex digitDec("^(\\+|\\-)[[:digit:]]+$");
    regex digitHex("^(\\+)0x([[:digit:]]|[A-F])+$");
    regex digitHexNeg("^(\\-)0x([[:digit:]]|[A-F])+$");
    regex character("^(\\+)('.')$");
    regex characterNeg("^(\\-)('.')$");

    int changed = 1;

    while (changed && !this->TNS.empty())
    {

        changed = 0;

        it = this->TNS.begin();

        int oneChange = 0;

        for (; it != this->TNS.end(); it++)
        {

            int equExtern = 0;
            string equExternSymbol;

            Symbol *symbol = (*it);

            list<string>::iterator itEqu;

            list<ClassificationIndex *> classificationTable;
            //proveri korektnost izraza

            list<Symbol *>::iterator sectionsIt;

            for (sectionsIt = this->sectionSortTable.begin(); sectionsIt != this->sectionSortTable.end(); sectionsIt++)
            {

                classificationTable.push_back(new ClassificationIndex((*sectionsIt)->section, 0));
            }

            classificationTable.push_back(new ClassificationIndex("*UND*", 0));

            int goToAnotherSymbol = 0;

            for (itEqu = symbol->equSymbols.begin(); itEqu != symbol->equSymbols.end(); itEqu++)
            {
                if (regex_match((*itEqu), symbolRegex))
                {

                    char sign = (*itEqu)[0];

                    string help = (*itEqu);

                    help.erase(0, 1); // da aobrise + ili -

                    Symbol *symbolExp = this->findSymbol(help);

                    if (symbolExp != 0 && (symbolExp->externBool || symbolExp->equExtern))
                    {
                        equExtern = 1;
                        if (!symbolExp->isEquSymbol)
                            equExternSymbol = symbolExp->label;
                        else
                            equExternSymbol = symbolExp->equExternSymbol;
                        ;
                        sign = '+';
                    }

                    if (symbolExp == 0)
                    {
                        cout << "Symbol " << help << " is undefined" << endl;
                        return 0;
                    }

                    else if (symbolExp->unsolved == 1)
                    {
                        cout << "Symbol is still unsolved " << endl;
                        goToAnotherSymbol = 1;
                        break;
                    }
                    else if (symbolExp->section != "*ABS*")
                    {

                        list<ClassificationIndex *>::iterator itIndex;

                        for (itIndex = classificationTable.begin(); itIndex != classificationTable.end(); itIndex++)
                        {

                            if ((*itIndex)->section == symbolExp->section)
                                break;
                            else
                                continue;
                        }

                        if (sign == '-')
                            (*itIndex)->index += -1;
                        else
                            (*itIndex)->index += 1;
                    }
                }
            }

            if (goToAnotherSymbol)
                continue;

            list<ClassificationIndex *>::iterator itIndex;
            int numOfOnes = 0;
            string symbolSection;

            for (itIndex = classificationTable.begin(); itIndex != classificationTable.end(); itIndex++)
            {

                if ((*itIndex)->index != 1 && (*itIndex)->index != 0)
                {
                    cout << "Not correct equ expression for symbol '" << symbol->label << "'!" << endl;
                    return 0;
                }

                if ((*itIndex)->index == 1)
                {
                    numOfOnes++;
                    symbol->section = (*itIndex)->section;
                }
            }

            if (numOfOnes > 1)
            {
                cout << "Not correct equ expression for symbol '" << symbol->label << "'!" << endl;
                return 0;
            }

            if (equExtern)
            {
                symbol->equExtern = 1;
                symbol->section = "*UND*";
                symbol->equExternSymbol = equExternSymbol;
            }

            //sabiranje za offset

            int sum = 0;
            int i = 0;

            for (itEqu = symbol->equSymbols.begin(); itEqu != symbol->equSymbols.end(); itEqu++)
            {

                if (regex_match((*itEqu), symbolRegex))
                {

                    char sign = (*itEqu)[0];
                    string help = (*itEqu);

                    help.erase(0, 1);

                    Symbol *symbolExp = findSymbol(help);

                    int num = symbolExp->offset;

                    if (sign == '-')
                    {

                        num = -num;
                    }
                    sum += num;
                }

                else if (regex_match((*itEqu), digitDec))
                {

                    int num = stoi(*itEqu);
                    sum += num;
                }
                else if (regex_match((*itEqu), digitHex))
                {

                    string help = (*itEqu);

                    help.erase(0, 3); //da obrise + i 0x

                    int num = this->convertFromHexToDec(help);

                    sum += num;
                }
                else if (regex_match(*itEqu, digitHexNeg))
                {
                    string help = (*itEqu);

                    help.erase(0, 3); //da obrise + i 0x

                    int num = this->convertFromHexToDec(help);
                    num = -num;
                    sum += num;
                }
                else if (regex_match((*itEqu), character))
                {

                    string help = (*itEqu);

                    help.erase(0, 2);               //da obrise + i '
                    help.erase(help.size() - 1, 1); //da obrise ' na kraju

                    char c = help[0];
                    int num = (int)c;

                    sum += num;
                }
                else if (regex_match((*itEqu), characterNeg))
                {

                    string help = (*itEqu);

                    help.erase(0, 2);               //da obrise + i '
                    help.erase(help.size() - 1, 1); //da obrise ' na kraju

                    char c = help[0];
                    int num = (int)c;
                    num = -num;

                    sum += num;
                }
            }

            symbol->offset = sum;

            it = this->TNS.erase(it);

            symbol->unsolved = 0;

            changed = 1;

            //sabiranje za offset
        }
    }

    if (!this->TNS.empty())
    {
        cout << "There are unsolved symbols!" << endl;
        return 0;
    }

    list<Symbol *>::iterator its;

    for (its = this->tableOfSymbols.begin(); its != this->tableOfSymbols.end(); its++)
    {

        if ((*its)->section == "*ABS*" && !(*its)->forwardRefsTable.empty())
        {

            list<ForwardRef *>::iterator itfr;

            for (itfr = (*its)->forwardRefsTable.begin(); itfr != (*its)->forwardRefsTable.end(); itfr++)
            {
                //razresavamo sva sporna mesta datog simbola.. sve je zabelezeno u forwardRef listi

                list<RelocElem *>::iterator it2;

                for (it2 = this->relocTable.begin(); it2 != this->relocTable.end(); it2++)
                {

                    if ((*it2)->locationCounter == (*itfr)->LC && (*it2)->section == (*itfr)->section)
                        it2 = this->relocTable.erase(it2);
                    else
                        continue;
                }
            }
        }
    }

    return 1;
}

int Assembler::fix(Symbol *symbol)
{

    list<ForwardRef *>::iterator it;

    for (it = symbol->forwardRefsTable.begin(); it != symbol->forwardRefsTable.end(); it++)
    {
        //razresavamo sva sporna mesta datog simbola.. sve je zabelezeno u forwardRef listi

        list<RelocElem *>::iterator it2;

        for (it2 = this->relocTable.begin(); it2 != this->relocTable.end(); it2++)
        {

            if ((*it2)->locationCounter == (*it)->LC && (*it2)->section == (*it)->section)
                break;
            else
                continue;
        }

        list<list<Output *>>::iterator it3;
        list<Output *>::iterator it4;
        int findOutput = 0;
        for (it3 = this->output.begin(); it3 != this->output.end(); it3++)
        {

            for (it4 = it3->begin(); it4 != it3->end(); it4++)
            {

                if ((*it4)->locationCounter == (*it)->LC && (*it4)->section == (*it)->section)
                {
                    findOutput = 1;
                    break;
                }
                else
                    continue;
            }

            if (findOutput == 1)
                break;
            else
                continue;
        }

        if ((*it4)->pcRel && (symbol->section == (*it4)->section || symbol->section == "*ABS*"))
        {

            string hex;

            if (it2 != this->relocTable.end())
            {
                this->relocTable.erase(it2);
                it2 = this->relocTable.end();
            }

            int imDiAd = symbol->offset - (*it4)->PC;

            if (imDiAd == 0)
                (*it4)->memory = "00 00";
            else if (imDiAd < 0)
            {

                imDiAd = -imDiAd;

                if (imDiAd > 32768)
                {
                    cout << "Value can't fit!" << endl;
                    return 0;
                }

                string binary = this->convertFromDecToBin(imDiAd);

                string twosComplement = this->findTwosComplement(binary);

                int digitHelp = stoi(twosComplement, nullptr, 2);

                hex = convertFromDecToHex(digitHelp);

                string help1 = "  ";
                string help2 = "  ";

                help1[0] = hex[2];
                help1[1] = hex[3];
                help2[0] = hex[0];
                help2[1] = hex[1];
                hex = help1 + help2;

                hex.insert(2, 1, ' ');
            }
            else
            {

                hex = this->convertFromDecToHex(imDiAd);
                if (imDiAd > 65535)
                {
                    cout << "Value can't fit!" << endl;
                    return 0;
                }

                if (imDiAd <= 15)
                {
                    hex.insert(0, 3, '0');
                }
                else if (imDiAd <= 255)
                {
                    hex.insert(0, 2, '0');
                }
                else if (imDiAd <= 4095)
                {
                    hex.insert(0, 1, '0');
                }

                string help1 = "  ";
                string help2 = "  ";

                help1[0] = hex[2];
                help1[1] = hex[3];
                help2[0] = hex[0];
                help2[1] = hex[1];
                hex = help1 + help2;

                hex.insert(2, 1, ' ');
            }

            (*it4)->memory = hex;

        } //kraj pcRel za simbol i instrikciju u istoj sekciji ili abs

        else
        { //nije pcrel sa const simbolom ili istom sekcijom intrsukcije i simbola ili aps adresiranje

            if (symbol->equExtern)
                (*it2)->value = this->findSymbol(symbol->equExternSymbol)->id;

            if ((*it)->imDiAdSize == 1)
            {

                if ((symbol->globalBool || symbol->externBool) && symbol->section != "*ABS*" && !symbol->equExtern)
                {

                    //jedino sta nisam mogao da popunim jeste u reloc id jer nisam znao konacan id simbola
                    //i output memoru jer nisam znao da li je gobalan ili lokalan

                    if (it2 != this->relocTable.end())
                        (*it2)->value = symbol->id;

                    (*it4)->memory = "00";
                }

                else if ((symbol->globalBool == 0 && symbol->externBool == 0) || symbol->section == "*ABS*" || symbol->equExtern)
                {

                    if (symbol->defined)
                    {

                        if (it2 != this->relocTable.end() && !symbol->equExtern)
                            (*it2)->value = this->findSymbol(symbol->section)->id;

                        if (symbol->offset < 256)
                        {
                            string hex;

                            if (symbol->offset < 0)
                            {

                                int digit = -symbol->offset;

                                unsigned char byte = ~digit + 1;
                                int digitHelp = digit;

                                digit = byte;

                                hex = convertFromDecToHex(digit);
                                string help = hex;
                                if (digitHelp > 128)
                                {
                                    while (hex.size() > 2)
                                        hex.erase(0, 1);
                                    cout << "Value " << help << " truncated to " << hex << endl;
                                }
                            }

                            else if (symbol->offset >= 0)
                            {
                                hex = this->convertFromDecToHex(symbol->offset);

                                if (symbol->offset <= 15)
                                {
                                    hex.insert(0, 1, '0');
                                }
                            }

                            (*it4)->memory = hex;
                        }
                        else
                        {
                            cout << "ERROR! Offset can't fit!";
                            return 0;
                        }
                    }
                    else
                    {
                        cout << "Symbol " << symbol->label << " is not defined!";
                        return 0;
                    }
                }
            }
            else if ((*it)->imDiAdSize == 2)
            {

                if ((symbol->globalBool || symbol->externBool) && symbol->section != "*ABS*" && !symbol->equExtern)
                {

                    //jedino sta nisam mogao da popunim jeste u reloc id jer nisam znao konacan id simbola
                    //i output memoru jer nisam znao da li je gobalan ili lokalan

                    if (it2 != this->relocTable.end())
                        (*it2)->value = symbol->id;

                    if ((*it4)->pcRel == 0)
                        (*it4)->memory = "00 00";
                    else
                    {
                        string hex;

                        int imDiAd = 0 - ((*it4)->PC - (*it4)->locationCounter);

                        if (imDiAd == 0)
                            (*it4)->memory = "00 00";
                        else if (imDiAd < 0)
                        {

                            imDiAd = -imDiAd;

                            if (imDiAd > 32768)
                            {
                                cout << "Value can't fit!" << endl;
                                return 0;
                            }

                            string binary = this->convertFromDecToBin(imDiAd);

                            string twosComplement = this->findTwosComplement(binary);

                            int digitHelp = stoi(twosComplement, nullptr, 2);

                            hex = convertFromDecToHex(digitHelp);

                            string help1 = "  ";
                            string help2 = "  ";

                            help1[0] = hex[2];
                            help1[1] = hex[3];
                            help2[0] = hex[0];
                            help2[1] = hex[1];
                            hex = help1 + help2;

                            hex.insert(2, 1, ' ');
                        }
                        else if (imDiAd > 0)
                        {

                            hex = this->convertFromDecToHex(imDiAd);
                            if (imDiAd > 65535)
                            {
                                cout << "Value can't fit!" << endl;
                                return 0;
                            }

                            if (imDiAd <= 15)
                            {
                                hex.insert(0, 3, '0');
                            }
                            else if (imDiAd <= 255)
                            {
                                hex.insert(0, 2, '0');
                            }
                            else if (imDiAd <= 4095)
                            {
                                hex.insert(0, 1, '0');
                            }

                            string help1 = "  ";
                            string help2 = "  ";

                            help1[0] = hex[2];
                            help1[1] = hex[3];
                            help2[0] = hex[0];
                            help2[1] = hex[1];
                            hex = help1 + help2;

                            hex.insert(2, 1, ' ');
                        }

                        (*it4)->memory = hex;
                    }
                }

                else if ((symbol->globalBool == 0 && symbol->externBool == 0) || symbol->section == "*ABS*" || symbol->equExtern)
                {

                    if (symbol->defined)
                    {

                        if (it2 != this->relocTable.end() && !symbol->equExtern)
                            (*it2)->value = this->findSymbol(symbol->section)->id;

                        if ((*it4)->pcRel == 0)
                        {

                            string hex;

                            if (symbol->offset < 0)
                            {
                                int digit = -symbol->offset;

                                if (digit > 32768)
                                {
                                    cout << "Value can't fit" << endl;
                                    return 0;
                                }

                                string binary = this->convertFromDecToBin(digit);

                                string twosComplement = this->findTwosComplement(binary);

                                int digitHelp = stoi(twosComplement, nullptr, 2);

                                hex = convertFromDecToHex(digitHelp);

                                string help1 = "  ";
                                string help2 = "  ";

                                help1[0] = hex[2];
                                help1[1] = hex[3];
                                help2[0] = hex[0];
                                help2[1] = hex[1];
                                hex = help1 + help2;

                                hex.insert(2, 1, ' ');
                            }

                            else

                                if (symbol->offset >= 0)
                            {

                                if (symbol->offset < 65536)
                                {
                                    hex = this->convertFromDecToHex(symbol->offset);

                                    if (symbol->offset <= 15)
                                    {
                                        hex.insert(0, 3, '0');
                                    }
                                    else if (symbol->offset <= 255)
                                    {
                                        hex.insert(0, 2, '0');
                                    }
                                    else if (symbol->offset <= 4095)
                                    {
                                        hex.insert(0, 1, '0');
                                    }

                                    string help1 = "  ";
                                    string help2 = "  ";

                                    help1[0] = hex[2];
                                    help1[1] = hex[3];
                                    help2[0] = hex[0];
                                    help2[1] = hex[1];
                                    hex = help1 + help2;

                                    hex.insert(2, 1, ' ');
                                }

                                else
                                {
                                    cout << "ERROR! Offset can't fit!";
                                    return 0;
                                }
                            }
                            (*it4)->memory = hex;
                        }
                        else
                        {

                            string hex;

                            int imDiAd = symbol->offset - ((*it4)->PC - (*it4)->locationCounter);

                            if (imDiAd == 0)
                                (*it4)->memory = "00 00";
                            else if (imDiAd < 0)
                            {

                                if (symbol->label == "z")
                                    cout << "HEJ5" << endl;

                                imDiAd = -imDiAd;

                                if (imDiAd > 32768)
                                {
                                    cout << "Value can't fit!" << endl;
                                    return 0;
                                }

                                string binary = this->convertFromDecToBin(imDiAd);

                                string twosComplement = this->findTwosComplement(binary);

                                int digitHelp = stoi(twosComplement, nullptr, 2);

                                string hex = convertFromDecToHex(digitHelp);

                                string help1 = "  ";
                                string help2 = "  ";

                                help1[0] = hex[2];
                                help1[1] = hex[3];
                                help2[0] = hex[0];
                                help2[1] = hex[1];
                                hex = help1 + help2;

                                hex.insert(2, 1, ' ');

                                (*it4)->memory = hex;
                            }
                            else if (imDiAd > 0)
                            {

                                hex = this->convertFromDecToHex(imDiAd);
                                if (imDiAd > 65535)
                                {
                                    cout << "Value can't fit!" << endl;
                                    return 0;
                                }

                                if (imDiAd <= 15)
                                {
                                    hex.insert(0, 3, '0');
                                }
                                else if (imDiAd <= 255)
                                {
                                    hex.insert(0, 2, '0');
                                }
                                else if (imDiAd <= 4095)
                                {
                                    hex.insert(0, 1, '0');
                                }

                                string help1 = "  ";
                                string help2 = "  ";

                                help1[0] = hex[2];
                                help1[1] = hex[3];
                                help2[0] = hex[0];
                                help2[1] = hex[1];
                                hex = help1 + help2;

                                hex.insert(2, 1, ' ');

                                (*it4)->memory = hex;
                            }
                        }
                    }
                    else
                    {
                        cout << "Symbol " << symbol->label << " is not defined!";
                        return 0;
                    }
                }

            } //kraj imdiAdSize==2
        }
    }

    return 1;
}

//tehnicke stvari

string Assembler::findTwosComplement(string binary)
{

    int n = binary.size();

    // Traverse the string to get first '1' from
    // the last of string
    int i;
    for (i = n - 1; i >= 0; i--)
        if (binary[i] == '1')
            break;

    // If there exists no '1' concatenate 1 at the
    // starting of string
    if (i == -1)
        return '1' + binary;

    // Continue traversal after the position of
    // first '1'
    for (int k = i - 1; k >= 0; k--)
    {
        //Just flip the values
        if (binary[k] == '1')
            binary[k] = '0';
        else
            binary[k] = '1';
    }

    int dif = 16 - binary.size();
    if (dif > 0)
    {
        binary.insert(0, dif, '1');
    }

    // return the modified string
    return binary;
}

void Assembler::printTokensInLine()
{

    if (this->tokensInLine.empty())
    {
        cout << "Nema tokena u redu!" << endl;
        return;
    }
    else
    {
        cout << "Tokeni iz reda:" << endl;

        list<string>::iterator it;

        for (it = this->tokensInLine.begin(); it != this->tokensInLine.end(); it++)
            cout << "Token:" << *it << endl;
    }
}

string Assembler::convertFromDecToHex(int digit) //prenosimo ili niz brojeva ili jedan karakter u obliku 'a' pa ascii racunamo
{

    int n = digit;
    char hexaDeciNum[1000];

    string retValue;

    int i = 0;

    while (n != 0)
    {

        int temp = 0;

        temp = n % 16;

        if (temp < 10)
        {
            hexaDeciNum[i] = temp + 48;
            i++;
        }
        else
        {
            hexaDeciNum[i] = temp + 55;
            i++;
        }

        n = n / 16;
    }

    int k;

    if (digit > 0)
        for (int j = i - 1, k = 0; j >= 0; j--, k++)
        {
            retValue.insert(k, 1, hexaDeciNum[j]);
        }
    else
        retValue = "0";

    return retValue;
}

string Assembler::convertFromDecToBin(int n)
{

    // array to store binary number

    int binaryNum[20];

    // counter for binary array
    int i = 0;
    while (n > 0)
    {

        // storing remainder in binary array
        binaryNum[i] = n % 2;

        n = n / 2;
        i++;
    }

    string binary;
    int k = i;
    int j = i - 1;
    while (k > 0)
    {
        binary.append(to_string(binaryNum[j]));
        k--;
        j--;
    }

    if (i == 0)
        binary.append("0");

    return binary;
}

int Assembler::convertFromHexToDec(string value)
{

    int len = value.size();

    int base = 1;

    int dec_val = 0;

    for (int i = len - 1; i >= 0; i--)
    {

        if (value[i] >= '0' && value[i] <= '9')
        {
            dec_val += (value[i] - 48) * base;

            base = base * 16;
        }

        else if (value[i] >= 'A' && value[i] <= 'F')
        {
            dec_val += (value[i] - 55) * base;

            base = base * 16;
        }
    }

    return dec_val;
}

void Assembler::printOutput()
{

    cout << endl;

    list<list<Output *>>::iterator it1;

    list<Output *>::iterator it2;

    string currentSection = "*UND*";

    for (it1 = this->output.begin(); it1 != this->output.end(); it1++)
    {

        for (it2 = it1->begin(); it2 != it1->end(); it2++)
        {
            if (currentSection == "*UND*")
            {
                currentSection = (*it2)->section;
                cout << endl;
                cout << "----------" << currentSection << "----------" << endl;
            }
            if (currentSection != (*it2)->section)
            {
                currentSection = (*it2)->section;
                cout << endl
                     << "----------" << currentSection << "----------" << endl;
            }

            cout << (*it2)->memory << " ";
        }
        cout << endl;
    }
}

Symbol *Assembler::findSymbol(string label)
{

    list<Symbol *>::iterator it;

    for (it = this->tableOfSymbols.begin(); it != this->tableOfSymbols.end(); it++)
    {

        if ((*it)->label == label)
            return *it;
    }
    return 0;
}

void Assembler::pushSymbol(Symbol *symbol)
{

    this->tableOfSymbols.push_back(symbol);
}

void Assembler::printTableOfSymbols()
{

    cout << "-------------------TABLE OF SYMBOLS-------------------" << endl
         << endl;

    cout << "|" << setw(10) << "LABEL"
         << "|" << setw(10) << "SECTION"
         << "|" << setw(10) << "OFFSET"
         << "|" << setw(10) << "LOCAL?"
         << "|" << setw(10) << "NUMBER"
         << "|" << endl;

    list<Symbol *>::iterator it;

    for (it = this->tableOfSymbols.begin(); it != this->tableOfSymbols.end(); it++)
    {

        if ((*it)->equExtern)
            (*it)->offset = 0;

        cout << "|" << setw(10) << (*it)->label << "|" << setw(10) << (*it)->section << "|" << setw(10) << (*it)->offset << "|" << setw(10) << (*it)->localOrGlobal << "|" << setw(10) << (*it)->id << "|" << endl;
    }
}

void Assembler::resolveExternSymbols()
{

    list<Symbol *>::iterator it;

    for (it = this->tableOfSymbols.begin(); it != this->tableOfSymbols.end(); it++)
    {

        if ((*it)->defined == 0 && (*it)->externBool == 1)
        {

            this->symbolSortTable.push_back(*it);

            (*it)->section = "*UND*";
            (*it)->offset = 0;
        }
    }
}

void Assembler::sortTableOfSymbols()
{

    list<Symbol *>::iterator it;

    for (it = this->symbolSortTable.begin(); it != this->symbolSortTable.end(); it++)
        sectionSortTable.push_back(*it);

    this->tableOfSymbols = this->sectionSortTable;

    for (it = this->tableOfSymbols.begin(); it != this->tableOfSymbols.end(); it++)
    {

        (*it)->id = Symbol::ID++;
    }
}

int Assembler::isEverythingOkWithTable()
{

    list<Symbol *>::iterator it;

    for (it = this->tableOfSymbols.begin(); it != this->tableOfSymbols.end(); it++)
    {

        if ((*it)->defined == 0 && (*it)->globalBool == 1)
        {
            cout << "Exporting symbol " << (*it)->label << " that is not defined!" << endl;
            return 0;
        }

        if ((*it)->defined == 0 && (*it)->externBool == 0)
        {
            cout << "Undefined symbol " << (*it)->label << endl;
            return 0;
        }

        if ((*it)->isSectionSymbol == 1 && (*it)->globalBool == 1)
        {
            cout << "Section symbols are local" << endl;
            return 0;
        }

        if ((*it)->defined == 1 && (*it)->externBool == 1)
        {

            if ((*it)->globalBool == 0)
                (*it)->localOrGlobal = 'l';

            (*it)->externBool = 0;
        }

        if ((*it)->globalBool == 1 && (*it)->externBool == 1)
        {
            cout << "There is already a global label with name '" << (*it)->label << "' defined in another file" << endl;
            return 0;
        }
    }

    return 1;
}

void Assembler::printRelocTable()
{

    cout << "-------------------RELOCATION TABLE-------------------" << endl
         << endl;

    cout << "|" << setw(10) << "SECTION"
         << "|" << setw(10) << "LC"
         << "|" << setw(15) << "TYPE"
         << "|" << setw(10) << "VALUE"
         << "|" << endl;

    list<RelocElem *>::iterator it;

    for (it = this->relocTable.begin(); it != this->relocTable.end(); it++)
    {

        cout << "|" << setw(10) << (*it)->section << "|" << setw(10) << (*it)->locationCounter << "|" << setw(15) << (*it)->type << "|" << setw(10) << (*it)->value << "|" << endl;
    }
}

Assembler::~Assembler()
{

    list<RelocElem *>::iterator it1;

    for (it1 = this->relocTable.begin(); it1 != this->relocTable.end(); it1++)
    {

        delete (*it1);
        (*it1) = nullptr;
    }

    list<Symbol *>::iterator it2;

    for (it2 = this->tableOfSymbols.begin(); it2 != this->tableOfSymbols.end(); it2++)
    {

        delete (*it2);
        (*it2) = nullptr;
    }

    list<list<Output *>>::iterator it3;

    list<Output *>::iterator it4;

    string currentSection = "*UND*";

    for (it3 = this->output.begin(); it3 != this->output.end(); it3++)
    {

        for (it4 = it3->begin(); it4 != it3->end(); it4++)
        {
            delete (*it4);
            (*it4) = nullptr;
        }
    }

    list<Instruction *>::iterator it5;

    for (it5 = this->instructions.begin(); it5 != this->instructions.end(); it5++)

    {
        delete (*it5);
        (*it5) = nullptr;
    }

    /*
list<Symbol*>::iterator it6;

for(it6=this->TNS.begin();it2!=this->TNS.end();it6++){


delete (*it6);
(*it6)=nullptr;

}
*/
}

void Assembler::writeToFile(string outputFile)
{

    ofstream outFile;

    outFile.open(outputFile);

    if (!outFile.is_open())
    {

        cout << "Outputfile can't be opened!" << endl;
        exit(1);
    }

    else
    {

        outFile << endl;
        //tabela simbola
        outFile << "-------------------TABLE OF SYMBOLS-------------------" << endl
                << endl;

        outFile << "|" << setw(10) << "LABEL"
                << "|" << setw(10) << "SECTION"
                << "|" << setw(10) << "OFFSET"
                << "|" << setw(10) << "LOCAL?"
                << "|" << setw(10) << "NUMBER"
                << "|" << endl;

        list<Symbol *>::iterator it;

        for (it = this->tableOfSymbols.begin(); it != this->tableOfSymbols.end(); it++)
        {

            outFile << "|" << setw(10) << (*it)->label << "|" << setw(10) << (*it)->section << "|" << setw(10) << (*it)->offset << "|" << setw(10) << (*it)->localOrGlobal << "|" << setw(10) << (*it)->id << "|" << endl;
        }

        outFile << endl;

        //relocTable

        outFile << "-------------------RELOCATION TABLE-------------------" << endl
                << endl;

        outFile << "|" << setw(10) << "SECTION"
                << "|" << setw(10) << "LC"
                << "|" << setw(15) << "TYPE"
                << "|" << setw(10) << "VALUE"
                << "|" << endl;

        list<RelocElem *>::iterator it3;

        for (it3 = this->relocTable.begin(); it3 != this->relocTable.end(); it3++)
        {

            outFile << "|" << setw(10) << (*it3)->section << "|" << setw(10) << (*it3)->locationCounter << "|" << setw(15) << (*it3)->type << "|" << setw(10) << (*it3)->value << "|" << endl;
        }

        outFile << endl;

        //outputMemory

        outFile << "----------------- MEMORY ---------------------" << endl;

        outFile << endl;

        list<list<Output *>>::iterator it1;

        list<Output *>::iterator it2;

        string currentSection = "*UND*";

        for (it1 = this->output.begin(); it1 != this->output.end(); it1++)
        {

            for (it2 = it1->begin(); it2 != it1->end(); it2++)
            {
                if (currentSection == "*UND*")
                {
                    currentSection = (*it2)->section;
                    outFile << endl;
                    outFile << "----------" << currentSection << "----------" << endl;
                }
                if (currentSection != (*it2)->section)
                {
                    currentSection = (*it2)->section;

                    outFile << endl
                            << "----------" << currentSection << "----------" << endl;
                }

                outFile << (*it2)->memory << " ";
            }
            outFile << endl;
        }
    }

    outFile.close();
}

int Assembler::convertFromBinToDec(int n)
{
    int decimalNumber = 0;
    int base = 1;
    int temp = n;
    while (temp)
    {
        int lastDigit = temp % 10;
        temp = temp / 10;
        decimalNumber += lastDigit * base;
        base = base * 2;
    }
    cout << "Decimal form of " << n << " is " << decimalNumber << endl;
    ;

    return decimalNumber;
}
#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <list>

using namespace std;

class ForwardRef;

class Symbol
{

private:
  Symbol(string label, string section, int offset, char localOrGlobal, int defined);
  ~Symbol();

  Symbol();

  void printSymbol();

  string label;
  string section;

  int offset;
  char localOrGlobal;

  int defined;
  static int ID;

  int id;

  list<ForwardRef *> forwardRefsTable;

  int externBool;
  int globalBool;

  int isSectionSymbol; //da li je sekcija ili obican simbol

  int lastLCInSection;
  int nextPCInSection;

  int isEquSymbol;

  list<string> equSymbols;

  int equExtern;
  string equExternSymbol;
  int unsolved;

  friend class Assembler;
  friend class TableOfSymbols;
};

#endif
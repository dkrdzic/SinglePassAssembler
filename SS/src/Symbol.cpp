
#include "Symbol.h"
#include "ForwardRef.h"
#include <iostream>
#include <iomanip>



int Symbol:: ID=0;




  Symbol::  Symbol(string label, string section, int offset, char localOrGlobal,int defined){

        this->label=label;
        this->section=section;
        this->offset=offset;
        this->localOrGlobal=localOrGlobal;
        this->defined=defined;
       
        this->externBool=0;
        this->globalBool=0;
        this->isSectionSymbol=0;
     
        this->id=-1;
        this->lastLCInSection=0;
        this->nextPCInSection=0;
        
        this->isEquSymbol=0;
       

        this->equExtern=0;

         this->unsolved=0;
    }


Symbol:: Symbol(){

  this->label="unknown";
        this->section="*UND*";
        this->offset=-1;
        this->localOrGlobal='l';
        this->defined=0;
      
        this->externBool=0;
        this->globalBool=0;
        this->isSectionSymbol=0;
       
        this->id=-1;
        this->lastLCInSection=0;
        this->nextPCInSection=0;
        
        this->isEquSymbol=0;
      

       this->equExtern=0;

       this->unsolved=0;
}


Symbol::~Symbol(){

list<ForwardRef*>::iterator frIt;

for (frIt=this->forwardRefsTable.begin();frIt!=this->forwardRefsTable.end();frIt++) delete (*frIt);

}




void Symbol:: printSymbol(){


  cout<<"Simbol:"<<endl;

cout<<"|"<<setw(10)<<"LABEL"<<"|"<<setw(10)<<"SECTION"<<"|"<<setw(10)<<"OFFSET"<<"|"<<setw(10)<<"DEFINED"<<"|"<<setw(10)<<"LOCAL?"<<"|"<<setw(10)<<"NUMBER"<<"|"<<endl;



  cout<<"|"<<setw(10)<<this->label<<"|"<<setw(10)<<this->section<<"|"<<setw(10)<<this->offset<<"|"<<setw(10)<<this->defined<<"|"<<setw(10)<<this->localOrGlobal<<"|"<<setw(10)<<this->id<<"|"<<endl;
}
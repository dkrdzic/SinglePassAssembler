
#include <cstdlib>
#include "Assembler.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{

    //pokretanje ./exe asembler -o ulaz.txt ilzaz.txt
    //argv[3] -> ulaz.txt argv[4]->izlaz.txt

    Assembler *assembler = new Assembler(argv[3], argv[4]);

    delete assembler;

    return 0;
}
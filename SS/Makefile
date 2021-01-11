all:  
	g++ ./src/main.cpp ./src/Assembler.cpp ./src/Symbol.cpp ./src/RelocElem.cpp ./src/Output.cpp ./src/ForwardRef.cpp ./src/Instruction.cpp -o ./bin/exe


exec: 
	./bin/exe asembler -o ./testovi/ulaz.txt ./testovi/izlaz.txt







test:
	gcc -no-pie -o exe test.s

table: compile
		objdump -t test.o


compile:
	 	gcc -c test.s

disassemble: compile
			objdump --disassemble-all test.o


reloc: compile
		objdump -r test.o








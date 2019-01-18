########################################
##
## Makefile
## LINUX compilation 
##
##############################################





#FLAGS
C++FLAG = -g -std=c++11

MATH_LIBS = -lm

EXEC_DIR=.


.cc.o:
	g++ $(C++FLAG) $(INCLUDES)  -c $< -o $@


#Including
INCLUDES=  -I. 

#-->All libraries (without LEDA)
LIBS_ALL =  -L/usr/lib -L/usr/local/lib 


#First Program (ListTest)

Cpp_OBJ=image.o image_demo.o

PROGRAM_NAME=image_demo

$(PROGRAM_NAME): $(Cpp_OBJ)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(Cpp_OBJ) $(INCLUDES) $(LIBS_ALL)


all: 
	make program1
	make program2
	make program3 
	make program4

program1: 
	g++ $(C++FLAG) -I.   -c image.cc -o image.o
	g++ $(C++FLAG) -I.   -c program1.cpp -o program1.o
	g++ $(C++FLAG) -o program1 program1.cpp image.cc

program2: 
	g++ $(C++FLAG) -I.   -c image.cc -o image.o
	g++ $(C++FLAG) -I.   -c program2.cpp -o program2.o
	g++ $(C++FLAG) -o program2 program2.cpp image.cc
	
program3:
	g++ $(C++FLAG) -I.   -c image.cc -o image.o
	g++ $(C++FLAG) -I.   -c program3.cpp -o program3.o
	g++ $(C++FLAG) -o program3 program3.cpp image.cc

program4:
	g++ $(C++FLAG) -I.   -c image.cc -o image.o
	g++ $(C++FLAG) -I.   -c program4.cpp -o program4.o
	g++ $(C++FLAG) -o program4 program4.cpp image.cc

clean:
	(rm -f *.o; rm program1)
	(rm -f *.o; rm program2)
	(rm -f *.o; rm program3)
	(rm -f *.o; rm program4)
(:

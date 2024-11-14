all: compile link

compile:
	g++ -c Rule.cpp RuleGroup.cpp NonTerminal.cpp State.cpp functions.cpp main.cpp -std=c++23

link:
	g++ -o parser.exe Rule.o RuleGroup.o NonTerminal.o State.o functions.o main.o

clean:
	del parser.exe automata.dot *.o

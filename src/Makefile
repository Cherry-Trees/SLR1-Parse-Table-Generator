all: compile link

compile:
	g++ -c Rule.cpp RuleGroup.cpp State.cpp functions.cpp main.cpp

link:
	g++ -o parser.exe Rule.o RuleGroup.o State.o functions.o main.o

clean:
	del parser.exe automata.dot *.o
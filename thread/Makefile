main : main.o thread.o
	g++ main.o thread.o -o main -levent -lpthread
main.o : main.cpp
	g++ main.cpp -c -g
thread.o : thread.cpp
	g++ thread.cpp -c -g

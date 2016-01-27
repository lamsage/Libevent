server : server.o pb.o
	g++ server.o pb.o -llua -lm -ldl -levent -o server -lprotobuf
server.o : server.cpp
	g++ server.cpp -c -g -I/include
pb.o : pb.cpp
	g++ pb.cpp -c -I/include -g 
clean :
	rm -f log pb.o server.o server

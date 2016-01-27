client : client.o pb.o
	g++ client.o pb.o -llua -lm -ldl -levent -o client -lprotobuf
client.o : client.cpp
	g++ client.cpp -c -g -I/include
pb.o : pb.cpp
	g++ pb.cpp -c -I/include -g 
clean :
	rm -f log pb.o client.o client

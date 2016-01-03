write : write.cpp message.pb.cc
	g++ write.cpp message.pb.cc -o write -lprotobuf
read : read.cpp message.pb.cc
	g++ read.cpp message.pb.cc -o read -lprotobuf
clean :
	rm -f write read log

#include "thread.h"
#include <iostream>
#include <string.h>
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>     
#include <netdb.h> 
#include <stdlib.h>
#include <pthread.h>  
using namespace std;

const int MAXN = 2048;
const int MAXNTHREAD = 10;
struct event_base* base; 
int fd[MAXNTHREAD][2];
Thread *t[MAXNTHREAD];
int iSvrFd;

// 连接请求事件回调函数 
void onAccept(int iSvrFd, short iEvent, void *arg) 
{ 
	int iCliFd; 
	struct sockaddr_in sCliAddr; 

	socklen_t iSinSize = sizeof(sCliAddr); 
	iCliFd = accept(iSvrFd, (struct sockaddr*)&sCliAddr, &iSinSize); 
	if(iCliFd < 0) {
		perror("accept");
	}
	else if(iCliFd > FD_SETSIZE) {
		close(iCliFd);
	}
	else {
		write(fd[iCliFd%MAXNTHREAD][1], &iCliFd, sizeof(iCliFd));
	}
}

int sock(const char *Ip, int port) {
	int sockFd;
	struct sockaddr_in sSvrAddr; 
	memset(&sSvrAddr, 0, sizeof(sSvrAddr));   
	sSvrAddr.sin_family = AF_INET;   
	sSvrAddr.sin_addr.s_addr = inet_addr(Ip);     
	sSvrAddr.sin_port = htons(port);    
	sockFd = socket(AF_INET, SOCK_STREAM, 0);   
	evutil_make_socket_nonblocking(sockFd);
	int one = 1;
	setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	bind(sockFd, (struct sockaddr*)&sSvrAddr, sizeof(sSvrAddr));   
	listen(sockFd, MAXN); 
	return sockFd;
}

// 添加Accept事件
struct event* event_accept(struct event_base* base) {
	struct event *evListen = event_new(base, iSvrFd, EV_READ|EV_PERSIST, onAccept, NULL); 
	event_base_set(base, evListen); 
	event_add(evListen, NULL); 
	return evListen;
}

int main() {
	iSvrFd = sock("127.0.0.1", 8888);
	base = event_base_new(); 
	struct event *evListen = event_accept(base);
	pthread_t tid[MAXNTHREAD];
	for(int i = 0; i < MAXNTHREAD; i++) {
		pipe(fd[i]);
		t[i] = new Thread(fd[i][0]);
		pthread_create(&tid[i], NULL, Thread::start, t[i]);
	}
	event_base_dispatch(base); 
	return 0;
}

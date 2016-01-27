#include "thread.h"
#include <unistd.h>

Thread::Thread(int fd): pipeFd(fd){
	this->base = event_base_new();
}

void *Thread::start(void *arg) {
	Thread *t = (Thread *)arg;
	//struct event *evListen = event_new(t->base, t->pipeFd, EV_READ|EV_PERSIST, Thread::onAccept, NULL); 
	struct event *evListen;
	event_base_set(t->base, evListen);
	event_add(evListen, NULL); 
	event_base_dispatch(t->base); 
}

void Thread::onAccept(int pipeFd, short iEvent, void *arg) {
	int iCliFd;
	read(pipeFd, &iCliFd, sizeof(iCliFd));
	struct bufferevent *bev;
	evutil_make_socket_nonblocking(iCliFd);
	bev = bufferevent_socket_new(this->base, iCliFd, BEV_OPT_CLOSE_ON_FREE);
	//bufferevent_setcb(bev, onRead, NULL, NULL, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}
void Thread::onRead(int iCliFd, short iEvent, void *arg) {
	char buf[1500];
	read(iCliFd, buf, sizeof(buf));
	printf("%s\n", buf);
}

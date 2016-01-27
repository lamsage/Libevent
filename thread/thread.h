#ifndef THREAD_H
#define THREAD_H
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <event2/event-config.h>
#include <stdio.h>
class Thread {
public:
	struct event_base* base; 
	int pipeFd;
public:
	Thread(int );
	static void *start(void *);
	void onAccept(int , short , void *);
	void onRead(int , short , void *);
};
#endif

#include <stdio.h> 
#include <string.h> 
#include <iostream> 
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>     
#include <netdb.h> 
#include <stdlib.h>
#include <lua.hpp>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <event2/event-config.h>
#include <event.h>

#include <signal.h>
#include <mcheck.h>
using namespace std; 
const int MAXLINE = 1500;

lua_State *L;
//事件base 
struct event_base* base; 
int iSvrFd;
struct bufferevent *bevs[FD_SETSIZE];

int call(lua_State *L) {
	int fd = lua_tointeger(L, 1);
	lua_remove(L, 1);
	int n = lua_gettop(L);
	char buf[MAXLINE];
	lua_getglobal(L, "serialize");
	lua_insert(L, 1);

	if(lua_pcall(L, n, 1, 0)!= 0) {
		cout<<"error running function 'serialize' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	const char *arg = lua_tostring(L, -1);

	lua_getglobal(L, "getCall");
	if(lua_pcall(L, 0, 1, 0)!= 0) {
		cout<<"error running function 'getCall' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	const char *pto = lua_tostring(L, -1);
	sprintf(buf, "%s %s", pto, arg);
	bufferevent_write(bevs[fd], buf, strlen(buf) +1);
	return 0;
}

// 读事件回调函数 
void onRead(struct bufferevent *bev, void *arg) 
{ 
	int iCliFd = bufferevent_getfd(bev);
	char buf[MAXLINE], Pto[MAXLINE], Arg[MAXLINE]; 
	size_t len;
	len = bufferevent_read(bev, buf, sizeof(buf));

	sscanf(buf, "%s %s", Pto, Arg);
	sprintf(buf, "return %s", Arg);
	if(luaL_loadbuffer(L,buf, strlen(buf), "line") || lua_pcall(L, 0, -1, 0)) {
		cout<<"call return failed"<<endl;
		exit(0);
	}
	lua_pushinteger(L,iCliFd);
	lua_insert(L, 1);
	int n = lua_gettop(L);
	lua_getglobal(L, "for_caller");
	lua_pushstring(L, Pto);
	lua_gettable(L, -2);
	lua_insert(L, 1);
	lua_pop(L, 1);
	if(lua_pcall(L, n, -1, 0)!= 0) {
		cout<<"error running function 'test' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}

} 

//出错回调函数
void errorcb(struct bufferevent *bev, short error, void *ctx)
{
	if (error & BEV_EVENT_EOF)
	{
		printf("connection closed\n");
	}
	else if (error & BEV_EVENT_ERROR)
	{
		printf("some other error\n");
	}
	else if (error & BEV_EVENT_TIMEOUT)
	{
		printf("Timed out\n");
	}
	bufferevent_free(bev);
}

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

		struct bufferevent *bev;
		evutil_make_socket_nonblocking(iCliFd);
		bev = bufferevent_socket_new(base, iCliFd, BEV_OPT_CLOSE_ON_FREE);
		bufferevent_setcb(bev, onRead, NULL, errorcb, NULL);
		bevs[iCliFd] = bev;
		bufferevent_enable(bev, EV_READ | EV_WRITE);
	}
} 

void signal_cb(int fd, short event, void *arg)
{
	cout << "got signal\n";
	event_base_loopbreak(base);
}

void loadLua(lua_State *L) {
	luaL_openlibs(L);
	lua_pushcfunction(L, call);
	lua_setglobal(L, "call");
	if(luaL_loadfile(L,"util.lua") || lua_pcall(L, 0, 0, 0)) {
		cout<<"call pto failed"<<endl;
		exit(0);
	}
	if(luaL_loadfile(L,"pto.lua") || lua_pcall(L, 0, 0, 0)) {
		cout<<"call pto failed"<<endl;
		exit(0);
	}
	if(luaL_loadfile(L,"test.lua") || lua_pcall(L, 0, 0, 0)) {
		cout<<"call lua failed"<<endl;
		exit(0);
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
	listen(sockFd, 10); 
	return sockFd;
}

// 添加Accept事件
struct event* event_accept(struct event_base* base) {
	struct event *evListen = event_new(base, iSvrFd, EV_READ|EV_PERSIST, onAccept, NULL); 
	event_base_set(base, evListen); 
	event_add(evListen, NULL); 
	return evListen;
}

//添加信号结束event_loop事件
struct event* event_signal(struct event_base* base) {
	struct event *evSignal = event_new(base, SIGINT, EV_SIGNAL|EV_PERSIST, signal_cb, NULL);
	event_base_set(base, evSignal);
	event_add(evSignal, NULL);
	return evSignal;
}

int main() 
{ 
	setenv("MALLOC_TRACE","memory.log",1);
	mtrace();
	L = luaL_newstate();
	loadLua(L);

	iSvrFd = sock("127.0.0.1", 8888);

	// 初始化base 
	base = event_base_new(); 
	
	// 添加事件
	//struct event *evListen = event_accept(base);
	event_accept(base);
	struct event *evSignal = event_signal(base);

	// 事件循环 
	event_base_dispatch(base); 
	//event_free(evListen);
	event_free(evSignal);
	event_base_free(base);
	lua_close(L);
	muntrace();
	return 0; 
}

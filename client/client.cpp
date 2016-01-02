#include <stdio.h> 
#include <string.h> 
#include <iostream> 
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>     
#include <netdb.h> 
#include <stdlib.h>
#include <lua.hpp>
#include <unistd.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <event2/event-config.h>
#include <signal.h>
using namespace std; 
const int MAXLINE = 1500;

lua_State *L;
int iCliFd;   
//事件base 
struct event_base* base; 

int call(lua_State *L) {
	int n = lua_gettop(L);
	char buf[1500];
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
	int iLen;
	iLen = send(iCliFd, buf, strlen(buf)+1, 0);
	if(iLen <= 0) {
		cout << " send error\n";
	}
	return 0;
}

// 读事件回调函数 
void onRead(struct bufferevent *bev, void *arg) 
{ 
	char buf[MAXLINE], Pto[MAXLINE], Arg[MAXLINE]; 
	size_t len;
	len = bufferevent_read(bev, buf, sizeof(buf));

	sscanf(buf, "%s %s", Pto, Arg);
	sprintf(buf, "return %s", Arg);
	if(luaL_loadbuffer(L,buf, strlen(buf), "line") || lua_pcall(L, 0, -1, 0)) {
		cout<<"call return failed"<<endl;
		exit(0);
	}
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

void onInput(int fd, short iEvent, void *arg)
{
	int iLen;
	char buf[MAXLINE];
	cin >> buf;

	if(luaL_loadbuffer(L,buf, strlen(buf) , "line") || lua_pcall(L, 0, 0, 0)) {
		cout<<"call lua failed"<<endl;
		exit(0);
	}
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

	// 创建tcpSocket（iCliFd），监听本机8888端口   
	sockFd = socket(AF_INET, SOCK_STREAM, 0);   
	evutil_make_socket_nonblocking(sockFd);
	connect(sockFd, (struct sockaddr*)&sSvrAddr, sizeof(sSvrAddr));
	return sockFd;
}

void signal_cb(int fd, short event, void *arg)
{
	cout << "got signal\n";
	event_base_loopexit(base, NULL);
}

//添加信号结束event_loop事件
struct event* event_signal(struct event_base* base) {
	struct event *evSignal = event_new(base, SIGINT, EV_SIGNAL|EV_PERSIST, signal_cb, NULL);
	event_base_set(base, evSignal);
	event_add(evSignal, NULL);
	return evSignal;
}

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

void eventRead(event_base *base) {
	struct bufferevent *bev;
	bev = bufferevent_socket_new(base, iCliFd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, onRead, NULL, errorcb, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}

struct event * eventInput(event_base *base) {
	struct event *evInput = event_new(base, STDIN_FILENO, EV_READ|EV_PERSIST, onInput, NULL);
	event_base_set(base, evInput);
	event_add(evInput, NULL);
	return evInput;
}

int main() 
{ 

	L = luaL_newstate();
	loadLua(L);

	iCliFd = sock("127.0.0.1", 8888);

	// 初始化base 
	base = event_base_new(); 

	eventRead(base); 
	struct event *evInput = eventInput(base); 
	struct event *evSignal = event_signal(base);

	// 事件循环 
	event_base_dispatch(base); 
	event_free(evInput);
	event_free(evSignal);
	event_base_free(base);
	lua_close(L);

	return 0; 
}

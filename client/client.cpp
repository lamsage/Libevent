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
#include <fstream>
#include <map>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <event2/event-config.h>
#include <signal.h>

#include "pb.h"
using namespace std; 
const int MAXLINE = 1500;

map<int, string> ptoName;
map<string, int> ptoNum;
lua_State *L;
int iCliFd;   
struct bufferevent *bev;
//事件base 
struct event_base* base; 

int Call(lua_State *L) {
	SetMessage(L);
	lua_getglobal(L, "getPto");
	if(lua_pcall(L, 0, 1, 0)!= 0) {
		cout<<"error running function 'getPto' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	char buf[MAXLINE];
	string data = SerializeToString();
	const char *pto = lua_tostring(L, -1);
	int seq = ptoNum[string(pto)];
	sprintf(buf, "%d %s",seq, data.c_str());
	cout << "length : "<< data.length() <<endl;
	bufferevent_write(bev, buf, strlen(buf) +1);
	return 0;
}

int offset = 0;
char buf[MAXLINE];
// 读事件回调函数 
void onRead(struct bufferevent *bev, void *arg) 
{ 
	char Pto[MAXLINE], Arg[MAXLINE]; 
	bool flag = true;
	while(flag) {
		int len = bufferevent_read(bev, buf + offset, MAXLINE - offset);
		if(len != MAXLINE - offset)
			flag = false;
		len +=offset;
		offset = 0;
		int seq = 0;
		while(1) {
			sscanf(buf + offset, "%d", &seq);
			int j = 0;
			while(buf[offset + j] != ' ') ++j;
			++j ;
			strcpy(Arg, buf + offset + j);
			if(offset + strlen(buf + offset) + 1 > len) {
				for(int i = 0; i < len - offset; i++)
					buf[i] = buf[i+offset];
				offset = len - offset;
				break;
			}
			offset += strlen(buf + offset) + 1;
			strcpy(Pto, ptoName[seq].c_str());
			string data = string(Arg);
			GenMessage(Pto);
			ParseFromString(data);
			GetMessage(Pto);
			int n = lua_gettop(L);
			lua_getglobal(L, "caller");
			lua_pushstring(L, Pto);
			lua_gettable(L, -2);
			lua_insert(L, 1);
			lua_pop(L, 1);
			if(lua_pcall(L, n, -1, 0)!= 0) {
				cout<<"error running function 'caller' :"<<lua_tostring(L, -1)<<endl;
				exit(0);
			}
		}
	}
} 

void onInput(int fd, short iEvent, void *arg)
{
	int iLen;
	char buf[MAXLINE];
	cin >> buf;
	//for(int i = 0; i < 20000; ++i) {
		if(luaL_loadfile(L,"test.lua") || lua_pcall(L, 0, 0, 0)) {
			cout<<"call test failed"<<endl;
			exit(0);
		}
	//}
	cout << "finish\n";
}


void loadLua(lua_State *L) {
	LoadFile();
	ifstream in("protocol", ios::in);
	char name[MAXLINE];
	int sum = 0;
	while(in>>name) {
		BuildProto(name);
		ptoName[++sum] = string(name);
		ptoNum[string(name)] = sum;
	}
	//BuildProto("s_test");
	//BuildProto("s_sum");
	//BuildProto("c_print");
	BuildFile();
	lua_pushcfunction(L, Call);
	lua_setglobal(L, "Call");
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

#include <stdio.h> 
#include <string.h> 
#include <iostream> 
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>     
#include <netdb.h> 
#include <stdlib.h>
#include <lua.hpp>

#include <event.h> 
using namespace std; 
const int MAXLINE = 1500;

lua_State *L;
//事件base 
struct event_base* base; 

//.写时间回调函数
void onWrite(int iCliFd, short iEvent, void *arg)
{
	int iLen;
	char *msg = (char *)arg;
	iLen = send(iCliFd, msg, strlen(msg)+1, 0);
	if(iLen <= 0) {
		cout << " send error\n";
	}
}

static int call(lua_State *L) {
	int fd = lua_tointeger(L, 1);
	lua_remove(L, 1);
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
	struct event *pEvWrite = new event;
	event_set(pEvWrite, fd, EV_WRITE, onWrite, buf);
	event_base_set(base, pEvWrite);
	event_add(pEvWrite, NULL);
	/*
	int iLen = send(fd, buf, strlen(buf) + 1, 0);
	if(iLen <= 0) {
		printf("send error\n");
		exit(0);
	}
	*/
	return 0;
}

// 读事件回调函数 
void onRead(int iCliFd, short iEvent, void *arg) 
{ 
	int iLen; 
	char buf[MAXLINE], Pto[MAXLINE], Arg[MAXLINE]; 

	iLen = recv(iCliFd, buf, MAXLINE, 0); 

	if (iLen <= 0) { 
		cout << "Client Close" << endl; 

		// 连接结束(=0)或连接错误(<0)，将事件删除并释放内存空间 
		struct event *pEvRead = (struct event*)arg; 
		event_del(pEvRead); 
		delete pEvRead; 

		close(iCliFd); 
		return; 
	}
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

// 连接请求事件回调函数 
void onAccept(int iSvrFd, short iEvent, void *arg) 
{ 
	int iCliFd; 
	struct sockaddr_in sCliAddr; 

	socklen_t iSinSize = sizeof(sCliAddr); 
	iCliFd = accept(iSvrFd, (struct sockaddr*)&sCliAddr, &iSinSize); 

	// 连接注册为新事件 (EV_PERSIST为事件触发后不默认删除) 
	struct event *pEvRead = new event; 
	event_set(pEvRead, iCliFd, EV_READ|EV_PERSIST, onRead, pEvRead); 
	event_base_set(base, pEvRead); 
	event_add(pEvRead, NULL); 
} 

int main() 
{ 
	L = luaL_newstate();
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

	int iSvrFd;   
	struct sockaddr_in sSvrAddr; 

	memset(&sSvrAddr, 0, sizeof(sSvrAddr));   
	sSvrAddr.sin_family = AF_INET;   
	sSvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");     
	sSvrAddr.sin_port = htons(8888);    

	// 创建tcpSocket（iSvrFd），监听本机8888端口   
	iSvrFd = socket(AF_INET, SOCK_STREAM, 0);   
	bind(iSvrFd, (struct sockaddr*)&sSvrAddr, sizeof(sSvrAddr));   
	listen(iSvrFd, 10); 

	// 初始化base 
	base = event_base_new(); 

	struct event evListen; 
	// 设置事件 
	event_set(&evListen, iSvrFd, EV_READ|EV_PERSIST, onAccept, NULL); 
	// 设置为base事件 
	event_base_set(base, &evListen); 
	// 添加事件 
	event_add(&evListen, NULL); 

	// 事件循环 
	event_base_dispatch(base); 

	return 0; 
}

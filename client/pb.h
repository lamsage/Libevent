#ifndef PB_H
#define PB_H

#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <lua.hpp>

using namespace std;
using namespace google::protobuf;
extern lua_State *L;

string SerializeToString();
void ParseFromString(string &data);
void LoadFile();
void BuildProto(const char *name);
void GetMessage(const char*name);
int SetMessage(lua_State *L);
void BuildFile();
void GenMessage(const char *);
#endif

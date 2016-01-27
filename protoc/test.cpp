#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <lua.hpp>

using namespace std;
using namespace google::protobuf;

FileDescriptorProto file_proto;
DescriptorProto *message_proto;
DescriptorPool pool;
const FileDescriptor *file_descriptor;
const Descriptor *descriptor;
DynamicMessageFactory *factory;
const Message *message;
Message *msg;
const Reflection *reflection;
const FieldDescriptor *field;
lua_State *L;
int number;

FieldDescriptorProto* AddField(DescriptorProto* parent,
		const string& name, int number,
		FieldDescriptorProto::Label label,
		FieldDescriptorProto::Type type) {
	FieldDescriptorProto* result = parent->add_field();
	result->set_name(name);
	result->set_number(number);
	result->set_label(label);
	result->set_type(type);
	return result;
}

int BuildField(lua_State *L) {
	const char *name = lua_tostring(L, 1);
	const char *ptr = lua_tostring(L, 2);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_UINT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 2) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	AddField(message_proto, name, number, label, type);
	++number;
	return 0;
}

int GetField(lua_State *L) {
	const char *name = lua_tostring(L, 2);
	const char *ptr = lua_tostring(L, 3);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_UINT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 3) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	field = descriptor->FindFieldByName(name);
	if(label == FieldDescriptorProto::LABEL_REQUIRED) {
		if(type == FieldDescriptorProto::TYPE_UINT32) {
			int v = lua_tonumber(L, 1);
			reflection->SetUInt32(msg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			reflection->SetString(msg, field, v);
		}
		else {
			Message* tbl = MutableMessage(msg,field);
		}
	}
	else {
		if(type == FieldDescriptorProto::TYPE_UINT32) {
			int v = lua_tonumber(L, 1);
			reflection->AddUInt32(msg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			reflection->AddString(msg, field, v);
		}
		else {
		}
	}
	return 0;
}

int GetTable(lua_State *L) {
	const char *name = lua_tostring(L, 2);
	const char *ptr = lua_tostring(L, 3);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_UINT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 3) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	field = descriptor->FindFieldByName(name);
	if(label == FieldDescriptorProto::LABEL_REQUIRED) {
		if(type == FieldDescriptorProto::TYPE_UINT32) {
			int v = lua_tonumber(L, 1);
			reflection->SetUInt32(msg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			reflection->SetString(msg, field, v);
		}
		else {
			Message* tbl = MutableMessage(msg,field);
		}
	}
	else {
		if(type == FieldDescriptorProto::TYPE_UINT32) {
			int v = lua_tonumber(L, 1);
			reflection->AddUInt32(msg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			reflection->AddString(msg, field, v);
		}
		else {
		}
	}
	return 0;
}

int PostField(lua_State *L) {
	const char *name = lua_tostring(L, 1);
	const char *ptr = lua_tostring(L, 2);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_UINT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 2) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	field = descriptor->FindFieldByName(name);
	if(label == FieldDescriptorProto::LABEL_REQUIRED) {
		if(type == FieldDescriptorProto::TYPE_UINT32) {
			int v = reflection->GetUInt32(*msg, field);
			lua_pushinteger(L, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			string v = reflection->GetString(*msg, field);
			lua_pushstring(L, v.c_str());
		}
		else {
		}
	}
	else {
		if(type == FieldDescriptorProto::TYPE_UINT32) {
			const RepeatedField<uint32> my_doubs = reflection->GetRepeatedField<uint32>(*msg, field);
			int n = my_doubs.size();
			for(int i = 0; i < n; i++) {
				lua_pushinteger(L, i+1);
				lua_pushinteger(L, my_doubs.Get(i));
				lua_settable(L, -3);
			}
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const RepeatedPtrField<string> my_doubs = reflection->GetRepeatedPtrField<string>(*msg, field);
			int n = my_doubs.size();
			for(int i = 0; i < n; i++) {
				lua_pushnumber(L, i+1);
				lua_pushstring(L, my_doubs.Get(i).c_str());
				lua_settable(L, -3);
			}
		}
		else {
		}
	}
	return 1;
}

int Call(lua_State *L) {
	lua_getglobal(L, "getCall");
	if(lua_pcall(L, 0, 1, 0)!= 0) {
		cout<<"error running function 'getCall' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	const char *pto = lua_tostring(L, -1);
	int n = lua_gettop(L);
	//DynamicMessageFactory factory(&pool);
	//message = factory.GetPrototype(descriptor);
	factory = new DynamicMessageFactory(&pool);
	message = factory->GetPrototype(descriptor);
	msg = message->New();
	reflection = msg->GetReflection();
	lua_getglobal(L, "getMessage");
	lua_insert(L, 1);
	if(lua_pcall(L, n, 0, 0)!= 0) {
		cout<<"error running function 'getCall' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	return 0;
}

void BuildProto(const char *name) {
	number = 1;
	message_proto = file_proto.add_message_type();
	message_proto->set_name(name);
	luaL_openlibs(L);
	lua_pushcfunction(L, PostField);
	lua_setglobal(L, "PostField");
	lua_pushcfunction(L, BuildField);
	lua_setglobal(L, "BuildField");
	lua_pushcfunction(L, GetField);
	lua_setglobal(L, "GetField");
	lua_pushcfunction(L, Call);
	lua_setglobal(L, "Call");
	if(luaL_loadfile(L,"util.lua") || lua_pcall(L, 0, 0, 0)) {
		cout<<"call util failed"<<endl;
		exit(0);
	}
	if(luaL_loadfile(L,name) || lua_pcall(L, 0, 1, 0)) {
		cout<<"call name failed"<<endl;
		exit(0);
	}
	lua_getglobal(L, "buildProto");
	lua_insert(L, 1);
	if(lua_pcall(L, 1, 0, 0)!= 0) {
		cout<<"error running function 'buildProto' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
}

int main() {
	L = luaL_newstate();
	file_proto.set_name("proto");
	BuildProto("myProto");
	file_descriptor = pool.BuildFile(file_proto);
	descriptor = file_descriptor->FindMessageTypeByName("myProto");
	cout << descriptor->DebugString();
	if(luaL_loadfile(L,"test.lua") || lua_pcall(L, 0, 0, 0)) {
		cout<<"call test failed"<<endl;
		exit(0);
	}
	cout << msg->DebugString();

	if(luaL_loadfile(L,"myProto") || lua_pcall(L, 0, 1, 0)) {
		cout<<"call myProto failed"<<endl;
		exit(0);
	}
	lua_getglobal(L, "postMessage");
	lua_insert(L, 1);
	if(lua_pcall(L, 1, 0, 0)!= 0) {
		cout<<"error running function 'buildProto' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	/*
	   DescriptorProto *message_proto = file_proto.add_message_type();
	   message_proto->set_name("Pair");

	   FieldDescriptorProto *field_proto = NULL;

	   Field *f1,*f2;
	   f1->name = "key" , f1->type = "string", f1->label = "";
	   f2->name = "value" , f2->type = "string", f2->label = "";
	   field_proto = message_proto->add_field();
	   add_field(message_proto, field_proto, f1, 1);
	   field_proto = message_proto->add_field();
	   add_field(message_proto, field_proto, f2, 2);

	   DescriptorPool pool;
	   const FileDescriptor *file_descriptor = pool.BuildFile(file_proto);
	   const Descriptor *descriptor = file_descriptor->FindMessageTypeByName("Pair");
	   cout << descriptor->DebugString();

	   DynamicMessageFactory factory(&pool);
	   const Message *message = factory.GetPrototype(descriptor);
	   Message *pair = message->New();

	   const Reflection *reflection = pair->GetReflection();
	   const FieldDescriptor *field = NULL;
	   field = descriptor->FindFieldByName("key");
	   reflection->SetString(pair, field, "my key");
	   field = descriptor->FindFieldByName("value");
	   reflection->SetUInt32(pair, field, 1234);

	   cout << pair->DebugString();
	   string data;
	   pair->SerializeToString(&data);
	   cout << data << endl;
	   cout << data.length()<<endl;
	   delete pair;
	   */
	return 0;
}

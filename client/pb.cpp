#include "pb.h"


FileDescriptorProto file_proto;
DescriptorProto *message_proto;
DescriptorPool pool;
const FileDescriptor *file_descriptor;
const Descriptor *descriptor;
DynamicMessageFactory *factory;
const Message *message;
Message *msg;
const Descriptor *innerDescriptor;
Message *innerMsg;
const Reflection *reflection;
const Reflection *innerReflection;
const FieldDescriptor *field;
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
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_INT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 2) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	FieldDescriptorProto* FDP = AddField(message_proto, name, number, label, type);
	if(type == FieldDescriptorProto::TYPE_MESSAGE)
		FDP->set_type_name(ptr);
	++number;
	return 0;
}

int SetField(lua_State *L) {
	const char *name = lua_tostring(L, 2);
	const char *ptr = lua_tostring(L, 3);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_INT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 3) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	field = descriptor->FindFieldByName(name);
	if(label == FieldDescriptorProto::LABEL_REQUIRED) {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			int v = lua_tonumber(L, 1);
			reflection->SetInt32(msg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			reflection->SetString(msg, field, v);
		}
		else {
			innerMsg = reflection->MutableMessage(msg,field);
			innerReflection = innerMsg->GetReflection();
			innerDescriptor = file_descriptor->FindMessageTypeByName(ptr);
			lua_pushstring(L, ptr);
			int n = lua_gettop(L);
			lua_getglobal(L, "setInnerMessage");
			lua_insert(L, 1);
			if(lua_pcall(L, n, 0, 0)!= 0) {
				cout<<"error running function 'setInnerMessage' :"<<lua_tostring(L, -1)<<endl;
				exit(0);
			}
		}
	}
	else {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			int v = lua_tonumber(L, 1);
			reflection->AddInt32(msg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			reflection->AddString(msg, field, v);
		}
		else {
			innerMsg = reflection->AddMessage(msg,field);
			innerReflection = innerMsg->GetReflection();
			innerDescriptor = file_descriptor->FindMessageTypeByName(ptr);
			lua_pushstring(L, ptr);
			int n = lua_gettop(L);
			lua_getglobal(L, "setInnerMessage");
			lua_insert(L, 1);
			if(lua_pcall(L, n, 0, 0)!= 0) {
				cout<<"error running function 'setInnerMessage' :"<<lua_tostring(L, -1)<<endl;
				exit(0);
			}
		}
	}
	return 0;
}

int SetInnerField(lua_State *L) {
	const char *name = lua_tostring(L, 2);
	const char *ptr = lua_tostring(L, 3);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_INT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 3) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	field = innerDescriptor->FindFieldByName(name);
	if(label == FieldDescriptorProto::LABEL_REQUIRED) {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			int v = lua_tonumber(L, 1);
			innerReflection->SetInt32(innerMsg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			innerReflection->SetString(innerMsg, field, v);
		}
		else {
		}
	}
	else {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			int v = lua_tonumber(L, 1);
			innerReflection->AddInt32(innerMsg, field, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const char *v = lua_tostring(L, 1);
			innerReflection->AddString(innerMsg, field, v);
		}
		else {
		}
	}
	return 0;
}

int GetField(lua_State *L) {
	const char *name = lua_tostring(L, 1);
	const char *ptr = lua_tostring(L, 2);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_INT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) <= 3) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	field = descriptor->FindFieldByName(name);
	if(label == FieldDescriptorProto::LABEL_REQUIRED) {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			int v = reflection->GetInt32(*msg, field);
			lua_pushinteger(L, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			string v = reflection->GetString(*msg, field);
			lua_pushstring(L, v.c_str());
		}
		else {
			innerMsg = reflection->MutableMessage(msg,field);
			innerReflection = innerMsg->GetReflection();
			innerDescriptor = file_descriptor->FindMessageTypeByName(ptr);
			lua_pushstring(L, ptr);
			int n = lua_gettop(L);
			lua_getglobal(L, "getInnerMessage");
			lua_insert(L, 1);
			if(lua_pcall(L, n, 0, 0)!= 0) {
				cout<<"error running function 'getInnerMessage' :"<<lua_tostring(L, -1)<<endl;
				exit(0);
			}
		}
	}
	else {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			const RepeatedField<int32> my_doubs = reflection->GetRepeatedField<int32>(*msg, field);
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
			const RepeatedPtrField<Message> *my_doubs = reflection->MutableRepeatedPtrField<Message>(msg, field);
			int n = my_doubs->size();
			for(int i = 0; i < n; i++) {
				innerMsg = (Message*)&my_doubs->Get(i);
				innerReflection = innerMsg->GetReflection();
				innerDescriptor = file_descriptor->FindMessageTypeByName(ptr);
				lua_pushstring(L, ptr);
				int n = lua_gettop(L);
				lua_getglobal(L, "getInnerMessage");
				lua_insert(L, 1);
				if(lua_pcall(L, n, -1, 0)!= 0) {
					cout<<"error running function 'getInnerMessage' :"<<lua_tostring(L, -1)<<endl;
					exit(0);
				}
			}
		}
	}
	return 1;
}

int GetInnerField(lua_State *L) {
	const char *name = lua_tostring(L, 1);
	const char *ptr = lua_tostring(L, 2);
	FieldDescriptorProto::Type type;
	if(strcmp(ptr, "number") == 0)	type = FieldDescriptorProto::TYPE_INT32;
	else if(strcmp(ptr, "string") == 0)	type = FieldDescriptorProto::TYPE_STRING;
	else				type = FieldDescriptorProto::TYPE_MESSAGE;
	FieldDescriptorProto::Label label;
	label = (lua_gettop(L) == 2) ? FieldDescriptorProto::LABEL_REQUIRED : FieldDescriptorProto::LABEL_REPEATED ;
	field = innerDescriptor->FindFieldByName(name);
	if(label == FieldDescriptorProto::LABEL_REQUIRED) {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			int v = innerReflection->GetInt32(*innerMsg, field);
			lua_pushinteger(L, v);
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			string v = innerReflection->GetString(*innerMsg, field);
			lua_pushstring(L, v.c_str());
		}
		else {
		}
	}
	else {
		if(type == FieldDescriptorProto::TYPE_INT32) {
			const RepeatedField<int32> my_doubs = innerReflection->GetRepeatedField<int32>(*innerMsg, field);
			int n = my_doubs.size();
			for(int i = 0; i < n; i++) {
				lua_pushinteger(L, i+1);
				lua_pushinteger(L, my_doubs.Get(i));
				lua_settable(L, -3);
			}
		}
		else if(type == FieldDescriptorProto::TYPE_STRING) {
			const RepeatedPtrField<string> my_doubs =innerReflection->GetRepeatedPtrField<string>(*innerMsg, field);
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

int SetMessage(lua_State *L) {
	lua_getglobal(L, "getPto");
	if(lua_pcall(L, 0, 1, 0)!= 0) {
		cout<<"error running function 'getPto' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	const char *pto = lua_tostring(L, -1);
	int n = lua_gettop(L);
	factory = new DynamicMessageFactory(&pool);
	descriptor = file_descriptor->FindMessageTypeByName(pto);
	message = factory->GetPrototype(descriptor);
	msg = message->New();
	reflection = msg->GetReflection();
	lua_getglobal(L, "setMessage");
	lua_insert(L, 1);
	if(lua_pcall(L, n, 0, 0)!= 0) {
		cout<<"error running function 'setMessage' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
	return 0;
}

void GenMessage(const char *pto) {
	factory = new DynamicMessageFactory(&pool);
	descriptor = file_descriptor->FindMessageTypeByName(pto);
	message = factory->GetPrototype(descriptor);
	msg = message->New();
	reflection = msg->GetReflection();
}

string SerializeToString() {
	string data;
	msg->SerializeToString(&data);
	return data;
}

void ParseFromString(string &data) {
	msg->ParseFromString(data);
}

void LoadFile() {
	file_proto.set_name("proto");
	luaL_openlibs(L);
	lua_pushcfunction(L, GetField);
	lua_setglobal(L, "GetField");
	lua_pushcfunction(L, GetInnerField);
	lua_setglobal(L, "GetInnerField");
	lua_pushcfunction(L, BuildField);
	lua_setglobal(L, "BuildField");
	lua_pushcfunction(L, SetField);
	lua_setglobal(L, "SetField");
	lua_pushcfunction(L, SetInnerField);
	lua_setglobal(L, "SetInnerField");
	lua_pushcfunction(L, SetMessage);
	lua_setglobal(L, "SetMessage");
	if(luaL_loadfile(L,"util.lua") || lua_pcall(L, 0, 0, 0)) {
		cout<<"call util failed"<<endl;
		exit(0);
	}
}

void BuildProto(const char *name) {
	number = 1;
	message_proto = file_proto.add_message_type();
	message_proto->set_name(name);
	if(luaL_loadfile(L,name) || lua_pcall(L, 0, 1, 0)) {
		cout<<"call name failed"<<endl;
		exit(0);
	}
	lua_pushstring(L, name);
	lua_insert(L, 1);
	lua_getglobal(L, "Pto");
	lua_insert(L, 1);
	lua_settable(L, 1);
	lua_getfield(L, -1, name);
	lua_remove(L, 1);
	lua_getglobal(L, "buildProto");
	lua_insert(L, 1);
	if(lua_pcall(L, 1, 0, 0)!= 0) {
		cout<<"error running function 'buildProto' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
}

void GetMessage(const char*name) {
	lua_getglobal(L, "Pto");
	lua_getfield(L, -1, name);
	lua_remove(L, 1);
	lua_getglobal(L, "getMessage");
	lua_insert(L, 1);
	if(lua_pcall(L, 1, -1, 0)!= 0) {
		cout<<"error running function 'getMessage' :"<<lua_tostring(L, -1)<<endl;
		exit(0);
	}
}

void BuildFile() {
	file_descriptor = pool.BuildFile(file_proto);
	descriptor = file_descriptor->FindMessageTypeByName("myProto");
}

//int main() {
//	L = luaL_newstate();
//	LoadFile();
//	BuildProto("hisProto");
//	BuildProto("yourProto");
//	BuildProto("myProto");
//	file_descriptor = pool.BuildFile(file_proto);
//	if(luaL_loadfile(L,"test.lua") || lua_pcall(L, 0, 0, 0)) {
//		cout<<"call test failed"<<endl;
//		exit(0);
//	}
//	cout << msg->DebugString();
//	GetMessage("myProto");
//
//	string data = SerializeToString();
//	ParseFromString(data);
//	cout << msg->DebugString();
//	
//
//
//	   FileDescriptorProto file_proto;
//	   file_proto.set_name("proto");
//	   DescriptorProto *message_proto = file_proto.add_message_type();
//	   message_proto->set_name("Pair");
//
//	   AddField(message_proto, "key", 1, FieldDescriptorProto::LABEL_REQUIRED, FieldDescriptorProto::TYPE_STRING);
//	   AddField(message_proto, "value", 2, FieldDescriptorProto::LABEL_REQUIRED, FieldDescriptorProto::TYPE_INT32);
//
//	   message_proto = file_proto.add_message_type();
//	   message_proto->set_name("Pair2");
//	   FieldDescriptorProto* result;
//	   result = AddField(message_proto, "key", 1, FieldDescriptorProto::LABEL_REQUIRED, FieldDescriptorProto::TYPE_MESSAGE);
//	   result->set_type_name("Pair");
//	   DescriptorPool pool;
//	   const FileDescriptor *file_descriptor = pool.BuildFile(file_proto);
//	   const Descriptor *descriptor = file_descriptor->FindMessageTypeByName("Pair");
//	   cout << descriptor->DebugString();
//
//	   DynamicMessageFactory factory(&pool);
//	   const Message *message = factory.GetPrototype(descriptor);
//	   Message *pair = message->New();
//
//	   const Reflection *reflection = pair->GetReflection();
//	   const FieldDescriptor *field = NULL;
//	   field = descriptor->FindFieldByName("key");
//	   reflection->SetString(pair, field, "my key");
//	   field = descriptor->FindFieldByName("value");
//	   reflection->SetInt32(pair, field, 1234);
//
//	   cout << pair->DebugString();
//	   string data;
//	   pair->SerializeToString(&data);
//	   cout << data << endl;
//	   cout << data.length()<<endl;
//	   delete pair;
//	return 0;
//}

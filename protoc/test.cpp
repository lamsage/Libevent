#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>

using namespace std;
using namespace google::protobuf;

struct Field {
	char *name;
	char *type;
	char *label;
};
void add_field(DescriptorProto *message_proto, FieldDescriptorProto *field_proto, Field *field, int n) {
	//field_proto = message_proto->add_field();
	field_proto->set_name(field->name);
	if(strcmp(field->type,"number") == 0)  field_proto->set_type(FieldDescriptorProto::TYPE_UINT32);
	if(strcmp(field->type,"string") == 0)  field_proto->set_type(FieldDescriptorProto::TYPE_STRING);
	field_proto->set_number(n);
	if(field->label[0] != '0' ) {
		field_proto->set_label(FieldDescriptorProto::LABEL_REQUIRED);
	}
	else {
		field_proto->set_label(FieldDescriptorProto::LABEL_REPEATED);
	}
}
int main() {
	FileDescriptorProto file_proto;
	file_proto.set_name("foo.proto");

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
	/*
	field_proto = message_proto->add_field();
	field_proto->set_name("key");
	field_proto->set_type(FieldDescriptorProto::TYPE_STRING);
	field_proto->set_number(1);
	field_proto->set_label(FieldDescriptorProto::LABEL_REQUIRED);

	field_proto = message_proto->add_field();
	field_proto->set_name("value");
	field_proto->set_type(FieldDescriptorProto::TYPE_UINT32);
	field_proto->set_number(2);
	field_proto->set_label(FieldDescriptorProto::LABEL_REQUIRED);
	*/

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
	return 0;
}

#include <iostream>
#include <ReinterpretCast.hxx>
#include <stdio.h>
using namespace std;

class base {
public :
virtual void func(){
cout << "hello " <<endl;};
};
int main() {
	base b;
	//cout << base::func<<endl;
	base * p = &b;
	cout << &b <<endl;
	void *f = ReinterpretCast<void *>(&base::func);
	cout << f <<endl;
	//printf("%X\n", &base::func);
	(*(void *(*)())p)();
	//cout << (int)&b.func<<endl;
	b.func();
}

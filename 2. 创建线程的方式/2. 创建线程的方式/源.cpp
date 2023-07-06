#include <iostream>
#include <thread>

// 1. 传递无参void函数
void print() {
	std::cout << "子线程: " << "传递无参void函数\n";
}
void create1() {
	std::thread t1(print);
	t1.join(); //加入主线程
}

// 2. 传递lambda表达式
void create2() {
	std::thread t1([]() {
		std::cout << "lambda表达式\n";
		});

	if (t1.joinable()) {
		t1.join();
	}

}

// 3. 传递有参函数
// 3.1  普通参数
void print2(int num, const std::string& name, const int age, const int& yina, const int* cp) {
	std::cout << "num: " << num << " name: " << name << " age: " << age << " yina: "
		<< yina << " cp: " << *cp << '\n';
}
void create3() {
	int num = 10, age = 20, yina = 999, cpnum = 50;
	std::thread t1(print2, num, "你好", age, yina, &cpnum);
	if (t1.joinable()) {
		t1.join();
	}
}

// 3.2 带引用的参数
void print3(int& num) {
	std::cout << "引用 num: " << num << '\n';
}
void create4() {
	int num = 10;
	std::thread t1(print3, std::ref(num));
	if (t1.joinable()) {
		t1.join();
	}
}

// 3.3 带指针的参数
void print4(int* num) {
	std::cout << "指针 num: " << *num << '\n';
}
void create5() {
	int num = 10;
	std::thread t1(print4, &num);
	if (t1.joinable()) {
		t1.join();
	}
}

// 3.4 智能指针
void print5(std::unique_ptr<int> ptr) {
	std::cout << "智能指针: " << *ptr.get() << '\n';
}
void create6() {
	std::unique_ptr<int> pointer(new int{ 100 });
	std::thread t1(print5, std::move(pointer));
	if (t1.joinable()) {
		t1.join();
	}
	std::cout << "移动之后,智能指针: " << pointer.get() << '\n';
}

class Foo {
public:
	Foo() {}
	void operator()() {
		std::cout << "仿函数\n";
	}
};
// 4. 类成员函数
// 4.1 仿函数
void create7() {
	Foo f = Foo();
	std::thread t1(f);
	if (t1.joinable()) {
		t1.join();
	}

	//匿名对象
	std::thread t2((Foo()));
	if (t2.joinable()) {
		t2.join();
	}
}
// 4.2 普通成员函数
class Aoo {
public:
	Aoo() {}
	void test() {
		std::cout << "普通成员函数\n";
	}
};
void create8() {
	Aoo a = Aoo();
	std::thread t1(&Aoo::test,a);
	if (t1.joinable()) {
		t1.join();
	}
}
int main() {
	create1();
	create2();
	create3();
	create4();
	create5();
	create6();
	create7();
	create8();
	return 0;
}
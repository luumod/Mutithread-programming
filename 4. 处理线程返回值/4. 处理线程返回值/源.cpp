#include <iostream>
#include <chrono>
#include <future> 
using namespace std::this_thread;

int value1(int num) {
	return num; 
}

/*
1. 使用async创建线程处理函数
2. future对象的.get获取
*/

//类对象
class Foo {
public:
	Foo() {}
	int getValue(int num) {
		std::chrono::milliseconds duration(2000);
		std::this_thread::sleep_for(duration);
		return num * 2;
	}
};


void testAsync() {
	// launch::async:		创建线程，执行线程处理函数（默认）
	// launch::deferred：	当调用wait或者get的时候才会创建并执行子线程
	std::future<int> res1 = std::async(value1, 100);
	std::cout << res1.get() << '\n';

	Foo m{};
	std::future<int> res2 = std::async(&Foo::getValue, &m, 200);
	std::cout << res2.get() << '\n';

	auto res3 = std::async(std::launch::deferred, &Foo::getValue, m, 400);
	//调用get，执行线程
	std::cout << res3.get() << '\n';
}

/*
1. 通过packaged_task来包装线程处理函数
2. 使用thread加入线程，执行线程处理函数
3. 调用 .get_future() 来获取future对象，接着调用 .get() 获取返回值
*/
void testPackaged_task() {
	//1. 普通函数的包装
	std::packaged_task<int(int)> pack1(value1);
	std::thread t1(std::ref(pack1),100);  //转换为&&
	t1.join();
	std::cout << pack1.get_future().get() << '\n';

	//2. 类中成员函数的包装
	Foo m{};
	std::packaged_task<int(int)> pack2(std::bind(&Foo::getValue, &m, std::placeholders::_1));
	std::thread t2(std::ref(pack2), 200);
	t2.join();
	std::cout << pack2.get_future().get() << '\n';
	
	//3. lambda表达式
	std::packaged_task<int(int)> pack3([](int num) {
		std::cout << "id: " << std::this_thread::get_id() << '\n';
		return num * 2;
	});
	std::thread t3(std::ref(pack3),300);
	t3.join();
	std::cout << pack3.get_future().get() << '\n';
}

/*
1. 创建Promise对象，使用其set_value来设置返回值
2. 对这个函数Promise对象的函数创建线程，并且执行
3. .get_future()获取future，然后.get()获取
*/
void testThread(std::promise<int>& pms, int num) {
	std::cout << get_id() << '\n';
	pms.set_value(num * 3);
}
void testGetValueThread(std::promise<int>& pms) {
	std::cout << "获取值: " << pms.get_future().get() << '\n';
}
void testGetValueThread(int num) {
	std::cout << "获取值: " << num << '\n';
}
void testPromise() {	
	std::promise<int> pms;
	std::thread t1(testThread, std::ref(pms), 100);
	t1.join();
	auto num = pms.get_future().get();
	std::cout << num << '\n';

	std::promise<int> pms2;
	pms2.set_value(99);
	//值传递到其他线程中
	//通过static_cast消除重载函数的歧义
	std::thread t2(static_cast<void(*)(std::promise<int>&)>(testGetValueThread), std::ref(pms2));
	t2.join();

	std::thread t3(static_cast<void(*)(int)>(testGetValueThread), num);
	t3.join();
}
int main() {
	testPromise();
	return 0;
}
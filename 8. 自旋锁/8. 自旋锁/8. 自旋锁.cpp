#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

class TestFlag {
public:
	TestFlag() {}

	void init_window() {
		while (flag.test_and_set()) { 
			//自旋过程
			std::cout << "窗口创建中...\n";
		}
		//自旋锁获取到了资源，等待结束
		std::cout << "窗口创建完毕...\n";
	}
	void init_info() {
		std::cout << "加载信息...\n";
		//等待加载资源信息
		std::this_thread::sleep_for(std::chrono::microseconds(10));
		//加载资源完成后，进行自旋锁的释放
		flag.clear(); //false
		std::cout << "信息加载完毕...\n";
	}

	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

void test1() {
	TestFlag f1;
	f1.flag.test_and_set(); //设置自旋
	std::thread window_thread(&TestFlag::init_window, &f1);
	std::thread info_thread(&TestFlag::init_info, &f1);
	window_thread.join();
	info_thread.join();
}
int main() {
	test1();

	return 0;
}

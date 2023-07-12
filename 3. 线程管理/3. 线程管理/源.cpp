#include <iostream>
#include <ctime>
#include <thread>
#include <chrono> //时间 
#include <iomanip>

//获取线程Id
void threadID() {
	std::cout << "Id: " << std::this_thread::get_id() << '\n';
}

//sleep_for延迟函数
void testSleep_for() {
	std::cout << "子线程sleep_for: \n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << "子线程sleep_for结束: \n";
}

//yeild
void testYield(int id) {
	for (int i = 1; i <= 10000; i++) {
		//std::cout << "id: " <<id << ": " << i << '\n';
		std::this_thread::yield();
	}
}

void testSleep_until() {
	//sleep_until： 阻塞线程直到某个时间点
#if 0 
	time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm* ptm = new tm;
	localtime_s(ptm, &t);
	std::cout << std::put_time(ptm, "%X") << '\n';
	if (ptm != nullptr) {
		std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(mktime(ptm)));
		}
	std::cout << std::put_time(ptm, "%X") << '\n';
#else 1
	std::cout << "子线程Sleep_untild: \n";
	for (int i = 1; i <= 10; i++) {
		auto startTime = std::chrono::steady_clock::now();
		std::this_thread::sleep_until(startTime+std::chrono::duration<long long>(2));
		std::cout << "i= " << i << '\n';
	}
	std::cout << "子线程Sleep_untild结束: \n";
#endif

}
int main() {
	std::thread t1(threadID);
	t1.join();	
	std::thread t2(testSleep_for);
	t2.join();
	std::thread t4(testSleep_until);
	t4.join();

	std::thread t5(testYield, 1);
	std::thread t6(testYield, 2);
	t5.join();
	t6.join();

	auto start = std::chrono::system_clock::now();
	for (int i = 1; i <= 100000000; i++) {
		int m = i + 10;
		m += 100 * 2 / 2;
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> tm = end - start;
	std::cout << "time: " << tm.count() << "ms" << '\n';

	//主线程
	std::cout << "Id: " << std::this_thread::get_id() << '\n';
	return 0;
}
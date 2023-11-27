#include <iostream>
#include <random>
#include <functional>
#include <chrono>
#include "ThreadPool.hpp"

auto get(int id)->decltype(id) {
	printf("线程开始:%d\n", id);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	printf("线程结束:%d\n", id);
	return id * id;
}
class Test {
public:
	Test() {};
	int get(int id) {
		printf("线程开始:%d\n", id);
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		printf("线程结束:%d\n", id);
		return id * id;
	}
};
void test_thread_pool() {
	ThreadPool pool(5);
	pool.init();
	Test t;
	std::vector<std::future<int>> res;
	for (int i = 1; i <= 12; i++) {
		//res.emplace_back(pool.subm it(get, i));
		res.emplace_back(pool.submit(std::bind(&Test::get, &t, i)));
	}

	for (auto& x : res) {
		printf("结束: %d\n", x.get());
	}
	pool.shutdown();
}
int main(){
	test_thread_pool();
	return 0;
}
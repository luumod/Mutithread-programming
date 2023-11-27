#include <iostream>
#include <future>
#include <mutex>
#include <functional>
#include <thread>
#include <string>

template <typename A,typename B>
auto add(const A& a, const B& b) -> decltype(a + b) {
	return a + b;
}

class Compute {
public:
	template <typename T>
	auto operator()(T x)->decltype(2 * x) {
		return 2 * x;
	}
};
template <typename T,typename F>
auto calculate( T x, F func) -> decltype(func(x)) {
	return func(x);
}
void test1() {
	std::string a = "hello", b = " world";
	//int a = 1, b = 2;
	//double a = 2.0, b = 2.4;
	//包装
#if 1
	std::packaged_task<decltype(add(a, b))(decltype(a), decltype(b))> task(add<decltype(a), decltype(b)>);
#else

#endif
	std::future<decltype(a + b)> res = task.get_future();
	std::thread(std::move(task), a, b).join();
	res.wait();
	decltype(a + b) ans = res.get();
	std::cout << ans << '\n';
}

void test2() {
	double a = 10;
	//std::string a = "ylh";
	Compute com;
	std::packaged_task<decltype(calculate(a,com))(decltype(a), decltype(com)) > task(calculate<decltype(a),decltype(com)>);
	std::future<decltype(calculate(a, com))> res = task.get_future();
	std::thread(std::move(task), a,com).join();
	res.wait();
	decltype(calculate(a,com)) ans = res.get();
	std::cout << ans << '\n';
}
int main() {
	test2();
	std::string a = "dd";
	std::cout << a * 2 << '\n';
	return 0;
}

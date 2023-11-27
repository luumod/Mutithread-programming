#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>
#include <functional>
#include <numeric>
#include <random>
#include <chrono>
#include <execution>
class TestPolicy {
public:
	TestPolicy(size_t size)
		:arrs(size){
		std::random_device rd;
		std::mt19937 mt;
		std::uniform_real_distribution<double> dist(1, 10000);
		for (auto& x : arrs) {
			x = dist(mt);
		}
		copy = arrs;
	}
	void time_begin() {
		start = std::chrono::steady_clock::now();

	}
	void time_end() {
		end = std::chrono::steady_clock::now();
		const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << __FUNCTION__ << ' ' << millis << "ms \n";
	}
	template <typename _Execu = decltype(std::execution::seq)>
	void sort(const _Execu& policy = _Execu{}) {
		arrs = copy;

		//按照顺序执行，无并行
		time_begin();
		std::sort(policy, arrs.begin(), arrs.end());
		time_end();
	}
	~TestPolicy() {}
protected:
	std::vector<double> copy;
	std::vector<double> arrs;
	std::chrono::steady_clock::time_point start, end;
};
int main() {
	TestPolicy test(1e6); 
	test.sort();
	test.sort(std::execution::seq);
	test.sort(std::execution::par);
	test.sort(std::execution::par_unseq);


	return 0;
}

#include <iostream>
#include <mutex>
#include <atomic>
#include <string>
#include <vector>
#include <thread>
#include <stack>

//异常处理
class EmptyStack_Error :public std::exception {
public:
	char const* what() const noexcept override {
		return "stack is empty!\n";
	}
private:
};

template <typename T>
class ThreadSafeStack {
public:
	ThreadSafeStack(){}
	~ThreadSafeStack() {}

	void push(const T& value) {
		std::lock_guard<std::mutex> lock(mutex);
		sta.push(value);
	}

	void pop(T& value) {
		std::lock_guard<std::mutex> ulock(mutex);
		if (sta.empty()) {
			throw EmptyStack_Error();
		}
		value = std::move(sta.top());
		sta.pop();
	}

	bool empty()const {
		std::lock_guard<std::mutex> lock(mutex);
		return sta.empty();
	}

private:
	std::mutex mutex;
	std::stack<T> sta;
};

template <typename T>
void insertData(ThreadSafeStack<T>& safe_stack, const T& value) {
	safe_stack.push(value);
}

template <typename T>
void popData(ThreadSafeStack<T>& safe_stack) {

	try{
		T value = 0;
		safe_stack.pop(value);
		printf("%d\n", value);
	}
	catch (const EmptyStack_Error& res){
		printf("%s\n", res.what());
	}
	
}

void test1() {
	std::vector<std::thread> threads;
	ThreadSafeStack<int> safe_stack;
	for (int i = 0; i < 10; i++) {
		threads.emplace_back(std::thread(insertData<int>, std::ref(safe_stack), i));
		threads.emplace_back(std::thread(popData<int>, std::ref(safe_stack)));
	}
	for (auto& x : threads) {
		x.join();
	}
}
int main() {
	test1();

	return 0;
}

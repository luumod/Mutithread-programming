#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>

template <typename T>
class Thread_Safe_Queue {
public:
	Thread_Safe_Queue() {}
	~Thread_Safe_Queue() {}

	void push(const T& val) {
		std::lock_guard<std::mutex> lock(mutex);
		q.push(val);
		cv.notify_one();
	}
	void wait_and_pop(T& val) {
		std::unique_lock<std::mutex> ulock(mutex);
		cv.wait(ulock, [=]() {return !q.empty(); });

		val = q.front();
		q.pop();
	}
	std::shared_ptr<T> wait_and_pop() {
		std::unique_lock<std::mutex> ulock(mutex);
		cv.wait(ulock, [=]() {return !q.empty(); });

		std::shared_ptr<T> res(std::make_shared<T>(q.front()));
		q.pop();
		return res;
	}

	void insert(const T& val) {
		push(val);
	}

	void print() {
		while (!q.empty()) {
			auto res = wait_and_pop();
			std::cout << *res.get() << ' ';
		}
	}

	//会引发异常的插入
	void bad_insert(const T& val) {
		q.push(val);
	}
	//会引发异常的弹出
	void bad_print() {
		while (!q.empty()) {
			std::cout << q.front() << ' '; //-842150451 0 4 5 1 7 8 9 6 3 不安全
			q.pop();
		}
	}

	void test() {
		//线程安全的测试
		for (int i = 0; i < 10; i++) {
			std::thread t(&Thread_Safe_Queue::insert,this,i);
			t.detach();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(2000));
		print();
		
	}
private:
	std::mutex mutex;
	std::condition_variable cv;
	std::queue<T> q;
};

int main() {
	Thread_Safe_Queue<int> q;
	q.test();

	return 0;
}

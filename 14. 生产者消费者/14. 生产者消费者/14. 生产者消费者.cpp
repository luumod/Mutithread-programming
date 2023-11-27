#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

class PAC {
public:
	PAC() {}
	~PAC() {}

	void producer() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			std::unique_lock<std::mutex> lock(mutex);
			std::cout << "生产产品: " << id << '\n';
			producer_box.push(id++);
			notified = true;
			cond.notify_all();
		}
	}
	void consumer() {
		while (true) {
			std::unique_lock<std::mutex> lock(mutex);
			while (!notified) {
				cond.wait(lock);
			}	
			lock.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			lock.lock();
			while (!producer_box.empty()) {
				std::cout << "消费者消费: " << producer_box.front() << '\n';
				producer_box.pop();
			}
			notified = false;
		}
	}
protected:
	std::queue<int> producer_box; 
	std::condition_variable cond;
	std::mutex mutex;
	bool notified = false;
	int id = 0;
};

void test() {
	PAC p;
	std::thread st(&PAC::producer, &p);
	std::vector<std::thread> cons;
	for (int i = 0; i < 2; i++) {
		cons.emplace_back(std::thread(&PAC::consumer, &p));
	}
	st.join();
	for (auto& x : cons) {
		x.join();
	}
}
int main(){
	test();
	
	return 0;
}

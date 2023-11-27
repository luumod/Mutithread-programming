#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>

//一个生产工厂
class Factory {
public:
	Factory() {}
	~Factory() {}

	//生产者
	void producer_thread() {
		{
			std::unique_lock<std::mutex> ulock(mutex);
			vec.push_back(i++);
			std::cout << "生产" << '\n';
			product_ok = true;
		}
		//在生产者生成数据后《及时》通知消费者进行消费
		cv.notify_one();
	}

	//消费者
	void consumer_thread() {
		std::unique_lock<std::mutex> ulock(mutex);
		cv.wait(ulock, [=]() {return product_ok; });
		vec.pop_back();
		std::cout << "取出"<< '\n';
		product_ok = false;
	}
private:
	int i = 1;
	std::vector<int> vec;
	std::condition_variable cv;
	bool product_ok = false;
	bool consume_ok = false;
	std::mutex mutex;
};

//任务队列
class TaskQueue {
public:
	TaskQueue(){}
	~TaskQueue(){}

	void execute_task() {
		std::unique_lock<std::mutex> ulock(mutex);
		cv.wait(ulock, [=]() {return !q.empty(); });
		std::cout << "处理任务: " << q.front();
		q.pop();
	}

	void addTask(int id) {
		std::unique_lock<std::mutex> ulock(mutex);
		q.push(id); //添加任务id
		cv.notify_one();
	}
private:
	std::queue<int> q;
	std::condition_variable cv;
	std::mutex mutex;
};

void testThread() {
	Factory t;
	std::thread t1(&Factory::producer_thread, &t);
	std::thread t2(&Factory::consumer_thread, &t);
	t1.join();
	t2.join();
}

void testThread2() {
	TaskQueue t;
	std::thread t1(&TaskQueue::addTask, &t,1);
	std::thread t2(&TaskQueue::execute_task, &t);
	t1.join();
	t2.join();
}

int main1() {

	testThread2();

	return 0;
}

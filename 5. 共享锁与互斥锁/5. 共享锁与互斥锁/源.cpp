#include <iostream>
#include <thread>
#include <vector>
#include <cstdio>
#include <mutex>

/*
1. mutex 实现加锁与开锁
2. lock_guard 的构造与析构函数实现lock与unlock
*/
class singleMan {
public:
	void makeGrilFriend() {
		for (int i = 0; i < 10000; i++) {
			std::lock_guard<std::mutex> lgObj(m_mutex);
			//m_mutex.lock();
			vec.push_back(i);
			printf("增加一个女朋友: %d\n", vec.back());
			//m_mutex.unlock();
		}
	}
	void leaveGrilFriend() {
		for (int i = 0; i < 10000; i++) {
			if (!vec.empty()) {
				std::lock_guard<std::mutex> lgObj(m_mutex);
				//m_mutex.lock();
				printf("分手一个女朋友: %d\n", vec.back());
				vec.pop_back();
				//m_mutex.unlock();
			}
			else {
				printf("单身!\n");
			}
		}
	}
private:
	std::vector<int> vec;
	std::mutex m_mutex;
};


class doSomeThing {
public:
	void goToTheToilet() {
		for (int i = 0; i < 1000; i++) {
			//1. 
			//std::unique_lock<std::mutex> uLk(m_mutex);
			//2. 
			//m_mutex.lock(); //首先要加锁
			//std::unique_lock<std::mutex> uLk(m_mutex, std::adopt_lock);
			//3. 
			std::unique_lock<std::mutex> uLk(m_mutex, std::defer_lock);
			uLk.lock();
			//4. 
			//std::unique_lock<std::mutex> uLk(m_mutex, std::try_to_lock);
			//if (uLk.owns_lock()) {
			// vec.push_back(i);
			//}
			vec.push_back(i);
			uLk.unlock();
			printf("上厕所\n");
		}
	}
	void goToTheBath() {
		for (int i = 0; i < 1000; i++) {
			if (!vec.empty()) {
				std::unique_lock<std::mutex> uLk(m_mutex);
				printf("洗澡\n");
				vec.pop_back();
			}
			else {
				printf("完事了!\n");
			}
		}
	}
private:
	std::vector<int> vec;
	std::mutex m_mutex;
};
int main() {
#if 0
	singleMan man;
	std::thread t1(&singleMan::makeGrilFriend, &man);
	std::thread t2(&singleMan::leaveGrilFriend, &man);
	t1.join();
	t2.join();
#else
	doSomeThing man;
	std::thread t1(&doSomeThing::goToTheToilet, &man);
	std::thread t2(&doSomeThing::goToTheBath, &man);
	t1.join();
	t2.join();

#endif


	return 0;
}
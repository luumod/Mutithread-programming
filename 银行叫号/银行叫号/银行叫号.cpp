#include <iostream>
#include <semaphore>
#include <mutex>
#include <queue>
#include "ThreadPool.hpp"

class Bank{
public:
	Bank()
		:empty(10), window(0), id(1) {}
	~Bank() {}

	//顾客排队取号
	void customer() {
		do {
			std::unique_lock<std::mutex> lock(queue_mutex);
			wait(empty);
			//获取号码
			push(id); //领取号码，进入等待队列
			//等待叫号
			printf("%d号顾客进入等待队列...此时排队情况: ", id++);
			show();

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			//获得服务
			signal(window);

		} while (true);
		
	}
	//提供服务
	void worker() {
		do{
			std::unique_lock<std::mutex> lock(work_mutex);
			wait(window); //等待服务窗口空闲
			//叫号
			int nid{}; 
			if (!get(nid)) { //从队列中取出第一个顾客
				signal(window);
				continue;
			}
			signal(empty); //空位+1
			//为此顾客提供服务
			printf("\n为%d号顾客服务中...\n", nid);
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			printf("---%d号顾客服务完成\n", nid);
		} while (true);
	}

	void test() {
		ThreadPool pool(2);
		pool.init();
		std::vector<std::future<void>> res;
		res.emplace_back(pool.enqueue(std::bind(&Bank::customer, this)));
		res.emplace_back(pool.enqueue(std::bind(&Bank::worker, this)));
		for (auto& x : res) {
			x.wait();
		}
		/*std::vector<std::thread> threads;
		threads.emplace_back(&Bank::worker, this);
		for (auto& x : threads) {
			x.join();
		}*/
		pool.shutdown();
	}
protected:
	std::counting_semaphore<10> empty;	//空位
	std::counting_semaphore<1> window;	//服务窗口
	std::mutex work_mutex, queue_mutex, q_mutex;
	std::queue<int> q;
	int id;

	void push(int id) {
		std::unique_lock<std::mutex> lock(q_mutex);
		q.push(id);
	}
	bool get(int& id) {
		std::unique_lock<std::mutex> lock(q_mutex);
		if (q.empty()) {
			return false;
		}
		id = std::move(q.front());
		q.pop();
		return true;
	}
	int size() {
		std::unique_lock<std::mutex> lock(q_mutex);
		return q.size();
	}
	void show() {
		std::unique_lock<std::mutex> lock(q_mutex);
		auto qt = q;
		while (!qt.empty()) {
			printf("%d ", qt.front());
			qt.pop();
		}
		putchar('\n');
	}

	template <typename T>
	void wait(T& s) {
		s.acquire();
	}

	template <typename T>
	void signal(T& s) {
		s.release();
	}
};
int main() {
	Bank a;
	a.test();

	/*
	1号顾客进入等待队列...此时排队情况: 1
2号顾客进入等待队列...此时排队情况:
为1号顾客服务中...
2
3号顾客进入等待队列...此时排队情况: 2 3
4号顾客进入等待队列...此时排队情况: 2 3 4
5号顾客进入等待队列...此时排队情况: 2 3 4 5
6号顾客进入等待队列...此时排队情况: 2 3 4 5 6
---1号顾客服务完成

为2号顾客服务中...
7号顾客进入等待队列...此时排队情况: 3 4 5 6 7
8号顾客进入等待队列...此时排队情况: 3 4 5 6 7 8
9号顾客进入等待队列...此时排队情况: 3 4 5 6 7 8 9
10号顾客进入等待队列...此时排队情况: 3 4 5 6 7 8 9 10
11号顾客进入等待队列...此时排队情况: 3 4 5 6 7 8 9 10 11
---2号顾客服务完成

为3号顾客服务中...
12号顾客进入等待队列...此时排队情况: 4 5 6 7 8 9 10 11 12
13号顾客进入等待队列...此时排队情况: 4 5 6 7 8 9 10 11 12 13
---3号顾客服务完成

为4号顾客服务中...
14号顾客进入等待队列...此时排队情况: 5 6 7 8 9 10 11 12 13 14
---4号顾客服务完成

为5号顾客服务中...
15号顾客进入等待队列...此时排队情况: 6 7 8 9 10 11 12 13 14 15
---5号顾客服务完成

为6号顾客服务中...
16号顾客进入等待队列...此时排队情况: 7 8 9 10 11 12 13 14 15 16
---6号顾客服务完成

为7号顾客服务中...
17号顾客进入等待队列...此时排队情况: 8 9 10 11 12 13 14 15 16 17
---7号顾客服务完成

为8号顾客服务中...
18号顾客进入等待队列...此时排队情况: 9 10 11 12 13 14 15 16 17 18
	*/

	return 0;
}

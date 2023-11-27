#include "SafeQueue.hpp"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>

// Thread-pool
class ThreadPool {
private:
	class ThreadWorker {
	public:
		ThreadWorker(ThreadPool* threadPool, int id)
			:threadPool(threadPool), task_id(id) {}

		~ThreadWorker() {}

		void operator()() {
			std::function<void()> func;
			bool isDequeued = false;
			while (!threadPool->is_shutdown) {
				{
					std::unique_lock<std::mutex> lock(threadPool->condition_mutex);

					if (threadPool->safe_queue.empty()) {
						threadPool->condition_flag.wait(lock);
					}

					isDequeued = threadPool->safe_queue.dequeue(func);
				}

				if (isDequeued) {
					func();
				}
			}

		}
	private:
		int task_id;
		ThreadPool* threadPool = nullptr;
	};

public:
	ThreadPool(const int n_threads = 5)
		:threads(n_threads), is_shutdown(false) {}
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&&) = delete;
	~ThreadPool() {}

	void init() {
		for (int i = 0; i < threads.size();i++) {
			threads.at(i) = std::thread(ThreadWorker(this, i));
		}
	}

	void shutdown() {
		is_shutdown = true;
		condition_flag.notify_all();
		for (int i = 0; i < threads.size();i++) {
			if (threads.at(i).joinable()) {
				threads.at(i).join();
			}
		}
	}

	//submit tasks to the wait queue.
	template <typename Func,typename ...Args>
	auto submit(Func&& func,Args&& ...args)->std::future<decltype(func(args...))> {
		std::function<decltype(func(args...))()> f =
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
	
		auto task_ptr = std::make_shared<std::packaged_task<decltype(func(args...))()>>(f);

		std::function<void()> task_func = [task_ptr] { (*task_ptr)(); };

		safe_queue.enqueue(task_func);
			
		condition_flag.notify_one();

		return task_ptr->get_future();
	}
protected:
	std::vector<std::thread> threads;
	ThreadSafeQueue<std::function<void()>> safe_queue;
	std::condition_variable condition_flag;
	std::mutex condition_mutex;
	bool is_shutdown = false;
};

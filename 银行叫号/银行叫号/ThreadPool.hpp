#include "ThreadSafeQueue.hpp"
#include <functional>
#include <thread>
#include <vector>
#include <future>
#include <condition_variable>

class ThreadPool {

	class ThreadPoolWorker {
	public:
		ThreadPoolWorker(ThreadPool* pool,int id):pool(pool),id(id) {}
		~ThreadPoolWorker() {}
		
		void operator()() {
			std::function<void()> func;
			bool is_dequeued = false;
			while (!pool->is_shutdown) {
				{
					std::unique_lock<std::mutex> lock(pool->mutex);

					//任务队列为空，则等待
					if (pool->tasks.empty()) {
						pool->condition.wait(lock);
					}

					is_dequeued = pool->tasks.pop(func);
				}

				if (is_dequeued) {
					func();
				}
			}
		}
	protected:
		ThreadPool* pool = nullptr;
		int id;
	};
public:
	ThreadPool(int nthreads)
		:threads(nthreads), is_shutdown(false){}
	~ThreadPool() {

	}

	void init() {
		for (int i = 0; i < threads.size(); i++) {
			threads.at(i) = std::thread(ThreadPoolWorker(this, i));
		}
	}

	void shutdown() {
		condition.notify_all();
		is_shutdown = true;
		for (auto& x : threads) {
			if (x.joinable()) {
				x.join();
			}	
		}
	}

	//任务放入任务队列中
	template <typename F,typename ...Args>
	auto enqueue(F&& f, Args&&...args)->std::future<decltype(f(args...))> {
		std::function<decltype(f(args...))()> func =
			std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
		std::function<void()> task_func = [task_ptr]() { (*task_ptr)(); };
		tasks.push(task_func);
		condition.notify_one();
		return task_ptr->get_future();
	}
protected:
	std::vector<std::thread> threads; //线程池
	ThreadSafeQueue<std::function<void()>> tasks; //任务队列
	std::mutex mutex;
	std::condition_variable condition;
	bool is_shutdown;
};

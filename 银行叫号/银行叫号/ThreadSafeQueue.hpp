#include <queue>
#include <mutex>

template <typename T>
class ThreadSafeQueue {
public:
	ThreadSafeQueue() {}
	~ThreadSafeQueue() {}
	
	template <typename T>
	void push(const T& value) {
		std::unique_lock<std::mutex> lock(q_mutex);
		q.push(value);
	}

	template <typename T>
	bool pop(T& value) {
		std::unique_lock<std::mutex> lock(q_mutex);
		if (q.empty()) {
			return false;
		}
		value = std::move(q.front());
		q.pop();
		return true;
	}

	bool empty() {
		std::unique_lock<std::mutex> lock(q_mutex);
		return q.empty();
	}
protected:
	std::mutex q_mutex;
	std::queue<T> q;
};

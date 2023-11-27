#include <queue>
#include <mutex>

// Thread-safe queue to storage tasks
template <typename T>
class ThreadSafeQueue {
public:
	ThreadSafeQueue() {

	}
	~ThreadSafeQueue() {

	}

	bool empty()const {
		std::unique_lock<std::mutex> lock(q_mutex);
		return q.empty();
	}
	size_t size()const {
		std::unique_lock<std::mutex> lock(q_mutex);
		return q.size();
	}
	void enqueue(const T& value) {
		std::unique_lock<std::mutex> lock(q_mutex);
		q.emplace(value);
	}
	bool dequeue(T& get_value) {
		std::unique_lock<std::mutex> lock(q_mutex);
		if (q.empty()) {
			return false;
		}
		get_value = std::move(q.front());
		q.pop();
		return true;
	}
protected:
	std::queue<T> q;
	mutable std::mutex q_mutex;
};

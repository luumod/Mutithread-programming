#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
int p = 0;
std::atomic<int> ato_p = 0;
void func() {
	for (int i = 0; i < 10000; i++) {
		p++;
		ato_p++;
	}
}
void test1() {
	std::vector<std::thread> threads;
	for (int i = 0; i < 10; i++) {
		threads.emplace_back(func);
	}
	for (auto& x : threads) {
		x.join();
	}
	std::cout << "原子操作: " << ato_p << '\n';
	std::cout << "非原子操作: " << p << '\n';
}

std::atomic<int> data(0);
std::atomic<bool> ready(false);

void writer() {
    // 写入数据
    data.store(42, std::memory_order_relaxed);

    // 数据准备完毕
    ready.store(true, std::memory_order_relaxed);
}

void reader() {
    // 等待数据准备完毕
    while (!ready.load(std::memory_order_relaxed)) {
        std::this_thread::yield();
    }

    std::cout << "Data: " << data.load(std::memory_order_relaxed) << std::endl;
}


//无锁队列的实现：
template <typename T>
class AtomicDeque {
private:
    struct Node {
        T data;
        Node* next;

        Node(const T& data) :data(data), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
public:
    AtomicDeque() 
        :head(new Node(T())),
        tail(head.load()) {
    }

    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        Node* currentNode = tail.exchange(newNode);
        currentNode->next = newNode;
    }

    bool dequeue(T& result) {
        Node* currentHead = head.load();
        Node* newHead = currentHead->next;

        if (newHead) {
            result = newHead->data;
            head.store(newHead);
            delete currentHead;
            currentHead = nullptr;
            return true;
        }
        return false;
    }
};

//带锁队列
template <typename T>
class LockingQueue {
private:
    std::queue<T> data;
    std::mutex mutex;

public:
    void enqueue(const T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        data.push(value);
    }

    bool dequeue(T& result) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!data.empty()) {
            result = data.front();
            data.pop();
            return true;
        }
        return false;
    }
};

void test_efficiency() {
    const int numIterations = 1000000;
    const int numThreads = 4;

    //带锁队列
    AtomicDeque<int> lockFreeQueue;
    //记录时间
    auto startLockFree = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> lockFreeThreads;

    for (int i = 0; i < numThreads; ++i) {
        lockFreeThreads.emplace_back([&] {
            for (int j = 0; j < numIterations; ++j) {
                lockFreeQueue.enqueue(j);
            }

            int value = 0;
            while (lockFreeQueue.dequeue(value)){}
            });
    }

    for (auto& thread : lockFreeThreads) {
        thread.join();
    }

    auto endLockFree = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationLockFree = endLockFree - startLockFree;

    // Test Locking Queue
    LockingQueue<int> lockingQueue;
    auto startLocking = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> lockingThreads;

    for (int i = 0; i < numThreads; ++i) {
        lockingThreads.emplace_back([&] {
            for (int j = 0; j < numIterations; ++j) {
                lockingQueue.enqueue(j);
            }
            int value = 0;
            while (lockingQueue.dequeue(value)) {}
            });
    }

    for (auto& thread : lockingThreads) {
        thread.join();
    }

    auto endLocking = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationLocking = endLocking - startLocking;

    // Output Results
    std::cout << "Lock-Free Queue Duration: " << durationLockFree.count() << " seconds\n";
    std::cout << "Locking Queue Duration: " << durationLocking.count() << " seconds\n";
}
int main() {
    test_efficiency();

    return 0;
}
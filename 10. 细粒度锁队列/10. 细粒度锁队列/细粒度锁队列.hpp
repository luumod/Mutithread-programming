#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>

class EmptyQueueError : public std::exception {
public:
    const char* what() const noexcept override {
        return "Queue is empty!";
    }
};

template <typename T>
class ThreadSafeQueue {
private:
    struct Node {
        std::shared_ptr<T> value;
        std::unique_ptr<Node> next;
        Node(std::shared_ptr<T> val) : value(val), next(nullptr) {}
    };
    
    Node* get_tail() {
        std::lock_guard<std::mutex> lock(tail_mutex);
        return tail;
    }
    std::unique_ptr<Node> pop_head() {
        std::lock_guard<std::mutex> lock(head_mutex);
        if (head.get() == get_tail()) {
            return nullptr;
        }
        std::unique_ptr<Node> old_head_node = std::move(head);
        head = std::move(old_head_node->next);
        return old_head_node;
    }

public:
    ThreadSafeQueue() : head(new Node(std::make_shared<T>())), tail(head.get()) {}
    ~ThreadSafeQueue() {
    }
    void push(T value) {
        std::shared_ptr<T> new_value = std::make_shared<T>(std::move(value));
        std::unique_ptr<Node> new_node(new Node(new_value));
        Node* new_tail = new_node.get();
        //对表尾操作
        std::lock_guard<std::mutex> lock(tail_mutex);
        tail->next = std::move(new_node);
        tail = new_tail;
    }

    std::shared_ptr<T> try_pop() {
        std::unique_ptr<Node> r_head = pop_head();
        if (!r_head) {
            throw EmptyQueueError();
        }
        return r_head->value;
    }

private:
    std::unique_ptr<Node> head;
    Node* tail;
    std::mutex head_mutex, tail_mutex;
};

template <typename T>
void insertData(ThreadSafeQueue<T>& safe_queue, T value) {
    safe_queue.push(value);
}

template <typename T>
void popData(ThreadSafeQueue<T>& safe_queue) {
    try {
        std::shared_ptr<T> get_value = safe_queue.try_pop();
        printf("id:%u --> %d\n",std::this_thread::get_id(), *get_value);
    }
    catch (const EmptyQueueError& err) {
        printf("id:%u ---> %s\n", std::this_thread::get_id(), err.what());
    }
}

void test() {
    ThreadSafeQueue<int> t;
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++) {
        threads.emplace_back(std::thread(insertData<int>, std::ref(t), i));
        threads.emplace_back(std::thread(popData<int>, std::ref(t)));
    }

    for (auto& x : threads) {
        x.join();
    }
}

#include <iostream>
#include <queue>
#include <semaphore>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

class NumberBuffer {
public:
    NumberBuffer() : p1(n), p2(0), p3(0), odd_sum(0), even_sum(0) {}

    void produce() {
        do {
            std::unique_lock<std::mutex> lock(p1_mutex);
            wait(p1);
            int x = dist(mt);
            put(x);
            signal(p2);
            signal(p3);
        } while (true);
    }

    void getOdd() {
        do {
            std::unique_lock<std::mutex> lock(p2_mutex);
            wait(p2);
            int x{};
            if (!get(x)) {
                signal(p1);
                continue;
            }
            if (x % 2 == 0) {
                signal(p3);
            }
            else {
                countOdd();
                signal(p1);
            }
        } while (true);
    }

    void getEven() {
        do {
            std::unique_lock<std::mutex> lock(p3_mutex);
            wait(p3);
            int x{};
            if (get(x)) {
                signal(p1);
                continue;
            }
            if (x % 2 == 0) {
                countEven();
                signal(p1);
            }
            else {
                signal(p2);
            }
        } while (true);
    }

    void test() {
        std::vector<std::thread> threads;
        threads.emplace_back(&NumberBuffer::produce, this);
        threads.emplace_back(&NumberBuffer::getOdd, this);
        threads.emplace_back(&NumberBuffer::getEven, this);

        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    std::random_device rd;
    std::mt19937 mt{ rd() };
    std::uniform_int_distribution<int> dist{ 1, 100 };
    constexpr static auto n = 100;
    std::counting_semaphore<n> p1;
    std::counting_semaphore<1> p2, p3;
    std::queue<int> q;
    std::mutex q_mutex, p1_mutex, p2_mutex, p3_mutex;
    int odd_sum;
    int even_sum;

    template <typename T>
    void wait(T& s) {
        s.acquire();
    }

    template <typename T>
    void signal(T& s) {
        s.release();
    }

    void put(int x) {
        std::unique_lock<std::mutex> lock(q_mutex);
        q.push(x);
    }

    bool get(int& value) {
        std::unique_lock<std::mutex> lock(q_mutex);
        if (q.empty()) {
            return false;
        }
        value = std::move(q.front());
        q.pop();
        return true;
    }

    void countOdd() {
        std::unique_lock<std::mutex> lock(q_mutex);
        odd_sum++;
        printf("奇数个数：%d\n", odd_sum);
    }

    void countEven() {
        std::unique_lock<std::mutex> lock(q_mutex);
        even_sum++;
        printf("偶数个数：%d\n", even_sum);
    }
};

int main() {
    NumberBuffer buffer;
    buffer.test();
    return 0;
}

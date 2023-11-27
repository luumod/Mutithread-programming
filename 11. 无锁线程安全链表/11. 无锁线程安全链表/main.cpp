#include <iostream>
#include "ThreadSafeList.hpp"
#include "example.hpp"

#define STDLIST 1

void test() {
#if STDLIST 
    std::list<int> std_list;
#else
    ThreadSafeList<int> safe_list;
#endif
    std::vector<std::thread> threads;
    // Create two threads that concurrently append to the list
    for (int i = 0; i < 20000; i++) {
#if STDLIST 
        threads.emplace_back(std::thread([=, &std_list] {
            std_list.push_back(i);
            }));
        threads.emplace_back(std::thread([=, &std_list] {
            std_list.push_front(i);
            }));
        threads.emplace_back(std::thread([=, &std_list] {
            std_list.pop_back();
            }));
        threads.emplace_back(std::thread([=, &std_list] {
            std_list.pop_front();
            }));
#else
        threads.emplace_back(std::thread([=, &safe_list] {
            safe_list.push_back(i);
            }));
        threads.emplace_back(std::thread([=, &safe_list] {
            safe_list.push_front(i);
            }));
        threads.emplace_back(std::thread([=, &safe_list] {
            int n{};
            safe_list.pop_back(n);
            }));
        threads.emplace_back(std::thread([=, &safe_list] {
            int n{};
            safe_list.pop_front(n);
            }));
#endif
    }
    for (auto& x : threads) {
        x.join();
    }
#if STDLIST
    for (auto& x : std_list) {
        std::cout << x << ' ';
    }   
#else
    std::cout << safe_list << '\n';
#endif // STDLIST
}

int main1() {

	test();

	return 0;
}
#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "ThreadSafeList.hpp"  // Include your ThreadSafeList header file

const int NUM_THREADS = 5;
const int OPERATIONS_PER_THREAD = 10000;

void pushTest() {
    
}

void popTest() {
    
}

void test11() {
    ThreadSafeList<int> myThreadSafeList;
    // Seed for random number generation
    std::srand(std::time(0));

    // Create threads for push and pop operations
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (std::rand() % 2 == 0) {
            threads.emplace_back([=,&myThreadSafeList] {
                for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                    if (std::rand() % 2 == 0) {
                        myThreadSafeList.push_front(j);
                    }
                    else {
                        myThreadSafeList.push_back(j);
                    }
                }
                });
        }
        else {
            threads.emplace_back([=,&myThreadSafeList] {
                int value;
            for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
                if (std::rand() % 2 == 0) {
                    myThreadSafeList.pop_front(value);
                }
                else {
                    myThreadSafeList.pop_back(value);
                }
            }
                });
        }
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Print the final state of the list
    std::cout << "Final state of the list: " << myThreadSafeList << std::endl;
}
int main() {
    for (int i = 0; i <= 10; i++) {
        test11();
    }

    return 0;
}

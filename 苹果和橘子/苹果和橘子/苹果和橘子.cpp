#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>
#include <mutex>

using semaphore = std::counting_semaphore<1>;

semaphore empty(1), full_apple(0), full_orange(0), mutex(1);

void wait(semaphore& s) {
    s.acquire();
}

void signal(semaphore& s) {
    s.release();
}

void fatherPutApple() {
    do {
        wait(empty);
        wait(mutex);
        std::cout << "爸爸放一个苹果\n";
        signal(mutex);
        signal(full_apple);
    } while (true);
}

void motherPutOrange() {
    do
    {
        wait(empty);
        wait(mutex);
        std::cout << "妈妈放一个橘子\n";
        signal(mutex);
        signal(full_orange);
    } while (true);
}

void sonGetApple() {
    do
    {
        wait(full_apple);
        wait(mutex);
        // 拿一个苹果
        std::cout << "儿子拿一个苹果\n";
        signal(mutex);
        signal(empty);
    } while (true);
}

void daughterGetOrange() {
    do
    {
        wait(full_orange);
        wait(mutex);
        // 拿一个橘子
        std::cout << "女儿拿一个橘子\n";
        signal(mutex);
        signal(empty);
    } while (true);
}

int main() {
    // 在这里调用你的函数    
    std::vector<std::thread> threads;
    threads.emplace_back([] {
        fatherPutApple();
        });

    threads.emplace_back([] {
        motherPutOrange();
        });

    threads.emplace_back([] {
        sonGetApple();
        });

    threads.emplace_back([] {
        daughterGetOrange();
        });

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    /*
    爸爸放一个苹果
    儿子拿一个苹果
    妈妈放一个橘子
    女儿拿一个橘子

    爸爸放一个苹果
    儿子拿一个苹果
    妈妈放一个橘子
    女儿拿一个橘子

    爸爸放一个苹果
    儿子拿一个苹果
    妈妈放一个橘子
    女儿拿一个橘子

    爸爸放一个苹果
    儿子拿一个苹果
    妈妈放一个橘子
    女儿拿一个橘子

    爸爸放一个苹果
    儿子拿一个苹果
    妈妈放一个橘子
    女儿拿一个橘子

    爸爸放一个苹果
    儿子拿一个苹果
    妈妈放一个橘子
    女儿拿一个橘子
    */
    return 0;
}

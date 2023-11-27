#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#define XILIDU 0

//细粒度数组测试
class CoarseGrainedLockArray {
public:
    CoarseGrainedLockArray(int num_blocks)
        : data(num_blocks, 0)
#if XILIDU
        , mutexs(num_blocks)
#else 
#endif

    {
    }

    void modifyBlock(int blockIndex, int value) {
#if XILIDU
        std::lock_guard<std::mutex> lock(mutexs[blockIndex]);
#else
        std::lock_guard<std::mutex> lock(mutex);
#endif
        data[blockIndex] += value;
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        printf("%u: %d: %d\n",std::this_thread::get_id(), blockIndex,value);
    }

    std::vector<int> getData() const {
        return data;
    }

private:
    std::vector<int> data;
#if XILIDU
    std::vector<std::mutex> mutexs;
#else
    std::mutex mutex;
#endif
};

void test() {
    const int numBlocks = 5;
    CoarseGrainedLockArray queue(numBlocks);

    // 多个线程并发地修改数组的不同部分
    auto worker = [&](int blockIndex) {
        for (int i = 0; i < 10; ++i) {
            queue.modifyBlock(blockIndex, 1);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numBlocks; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // 打印最终结果
    auto result = queue.getData();
    for (int value : result) {
        std::cout << value << " ";
    }
}
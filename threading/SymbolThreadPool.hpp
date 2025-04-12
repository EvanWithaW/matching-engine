#ifndef MATCHING_ENGINE_SYMBOLTHREADPOOL_HPP
#define MATCHING_ENGINE_SYMBOLTHREADPOOL_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <queue>
#include <functional>
#include <atomic>
#include <vector>
#include <string>
#include <memory>

class SymbolThreadPool {
public:
    SymbolThreadPool(size_t numThreads);
    ~SymbolThreadPool();

    void start();
    void stop();
    
    // Submit a task for a specific symbol
    void submitTask(const std::string& symbol, std::function<void()> task);
    
    // Get the current thread assignment for a symbol
    int getThreadForSymbol(const std::string& symbol) const;
    
    // Check if the thread pool is running
    bool isRunning() const;

private:
    struct ThreadData {
        std::queue<std::function<void()>> taskQueue;
        std::mutex queueMutex;
        std::condition_variable condition;
    };

    size_t numThreads;
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<ThreadData>> threadData;
    std::atomic<bool> running;
    
    // Map symbols to thread indices
    mutable std::mutex symbolMapMutex;
    std::unordered_map<std::string, size_t> symbolToThread;
    
    // Worker thread function
    void workerThread(size_t threadIndex);
    
    // Assign a symbol to a thread (using hash or load balancing)
    size_t assignSymbolToThread(const std::string& symbol);
};

#endif // MATCHING_ENGINE_SYMBOLTHREADPOOL_HPP

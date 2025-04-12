#include "SymbolThreadPool.hpp"
#include <iostream>
#include <functional>

SymbolThreadPool::SymbolThreadPool(size_t numThreads)
    : numThreads(numThreads), running(false) {
    
    threadData.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        threadData.push_back(std::make_unique<ThreadData>());
    }
}

SymbolThreadPool::~SymbolThreadPool() {
    stop();
}

bool SymbolThreadPool::isRunning() const {
    return running.load();
}

void SymbolThreadPool::start() {
    if (isRunning()) {
        return;
    }
    
    running.store(true);
    
    threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back(&SymbolThreadPool::workerThread, this, i);
    }
    
    std::cout << "Symbol Thread Pool started with " << numThreads << " threads" << std::endl;
}

void SymbolThreadPool::stop() {
    if (!isRunning()) {
        return;
    }
    
    running.store(false);
    
    // Notify all threads to wake up and check running status
    for (size_t i = 0; i < numThreads; ++i) {
        threadData[i]->condition.notify_one();
    }
    
    // Join all threads
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    threads.clear();
    std::cout << "Symbol Thread Pool stopped" << std::endl;
}

void SymbolThreadPool::submitTask(const std::string& symbol, std::function<void()> task) {
    // First, ensure the symbol is assigned to a thread
    size_t threadIndex = assignSymbolToThread(symbol);
    
    {
        std::lock_guard<std::mutex> lock(threadData[threadIndex]->queueMutex);
        threadData[threadIndex]->taskQueue.push(std::move(task));
    }
    
    threadData[threadIndex]->condition.notify_one();
}

int SymbolThreadPool::getThreadForSymbol(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(symbolMapMutex);
    
    auto it = symbolToThread.find(symbol);
    if (it != symbolToThread.end()) {
        return static_cast<int>(it->second);
    }
    
    return -1; // Symbol not assigned to any thread yet
}

void SymbolThreadPool::workerThread(size_t threadIndex) {
    auto& data = threadData[threadIndex];
    
    while (isRunning()) {
        std::function<void()> task;
        bool hasTask = false;
        
        {
            std::unique_lock<std::mutex> lock(data->queueMutex);
            
            data->condition.wait(lock, [this, &data] {
                return !data->taskQueue.empty() || !isRunning();
            });
            
            if (!isRunning() && data->taskQueue.empty()) {
                break;
            }
            
            if (!data->taskQueue.empty()) {
                task = std::move(data->taskQueue.front());
                data->taskQueue.pop();
                hasTask = true;
            }
        }
        
        if (hasTask) {
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "Exception in thread " << threadIndex << ": " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown exception in thread " << threadIndex << std::endl;
            }
        }
    }
}

size_t SymbolThreadPool::assignSymbolToThread(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(symbolMapMutex);
    
    auto it = symbolToThread.find(symbol);
    if (it != symbolToThread.end()) {
        return it->second;
    }
    
    // Simple character-based hash for stock symbols (since they 1-5 characters)
    // This avoids the overhead of std::hash for short strings
    size_t hashValue = 0;
    for (char c : symbol) {
        hashValue = hashValue * 31 + c; // Simple and fast hash function
    }
    size_t threadIndex = hashValue % numThreads;
    
    symbolToThread[symbol] = threadIndex;
    return threadIndex;
}

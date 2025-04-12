//
// Created by Evan Weidner on 4/11/25.
//

#ifndef MATCHING_ENGINE_CONTINUOUSMATCHINGENGINE_HPP
#define MATCHING_ENGINE_CONTINUOUSMATCHINGENGINE_HPP

#include "MatchingEngine.hpp"
#include "../order/Order.hpp"
#include "Trade.hpp"
#include "../threading/SymbolThreadPool.hpp"
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <functional>
#include <vector>

class OrderProcessingResult;

class ContinuousMatchingEngine {
public:
    ContinuousMatchingEngine(size_t numThreads = 4);
    ~ContinuousMatchingEngine();

    void start();
    void stop();
    bool isRunning() const;
    void submitOrder(std::shared_ptr<Order> order);
    void cancelOrder(const std::string& orderId, const std::string& symbol);
    bool addSymbol(const std::string& symbol);
    bool removeSymbol(const std::string& symbol);
    bool hasSymbol(const std::string& symbol) const;
    std::vector<std::string> getSymbols() const;
    std::shared_ptr<OrderBook> getOrderBook(const std::string& symbol) const;
    void registerTradeCallback(std::function<void(std::shared_ptr<Trade>)> callback);
    void registerOrderProcessingCallback(std::function<void(std::shared_ptr<OrderProcessingResult>)> callback);
    std::string toString() const;
    int getThreadForSymbol(const std::string& symbol) const;

private:
    enum class OrderAction {
        SUBMIT,
        CANCEL
    };
    
    struct OrderRequest {
        OrderAction action;
        std::shared_ptr<Order> order;
        std::string orderId;
        std::string symbol;
    };
    
    std::unique_ptr<MatchingEngine> matchingEngine;
    std::unique_ptr<SymbolThreadPool> threadPool;
    std::atomic<bool> running;
    mutable std::mutex callbackMutex;
    std::vector<std::function<void(std::shared_ptr<Trade>)>> tradeCallbacks;
    std::vector<std::function<void(std::shared_ptr<OrderProcessingResult>)>> orderProcessingCallbacks;
    
    void processOrder(const OrderRequest& request);
    void notifyTradeCallbacks(std::shared_ptr<Trade> trade);
    void notifyOrderProcessingCallbacks(std::shared_ptr<OrderProcessingResult> result);
};

class OrderProcessingResult {
public:
    enum class Status {
        SUCCESS,
        PARTIAL_FILL,
        NO_MATCH,
        ERROR
    };
    
    OrderProcessingResult(Status status, 
                         const std::string& orderId, 
                         const std::string& symbol, 
                         const std::vector<std::shared_ptr<Trade>>& trades = {}, 
                         const std::string& errorMessage = "");
    
    Status getStatus() const;
    const std::string& getOrderId() const;
    const std::string& getSymbol() const;
    const std::vector<std::shared_ptr<Trade>>& getTrades() const;
    const std::string& getErrorMessage() const;
    
private:
    Status status;
    std::string orderId;
    std::string symbol;
    std::vector<std::shared_ptr<Trade>> trades;
    std::string errorMessage;
};

#endif // MATCHING_ENGINE_CONTINUOUSMATCHINGENGINE_HPP

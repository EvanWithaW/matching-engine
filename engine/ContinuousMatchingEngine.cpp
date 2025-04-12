//
// Created by Evan Weidner on 4/11/25.
//

#include "ContinuousMatchingEngine.hpp"
#include <iostream>

OrderProcessingResult::OrderProcessingResult(Status status, 
                                           const std::string& orderId, 
                                           const std::string& symbol, 
                                           const std::vector<std::shared_ptr<Trade>>& trades, 
                                           const std::string& errorMessage)
    : status(status), 
      orderId(orderId), 
      symbol(symbol), 
      trades(trades), 
      errorMessage(errorMessage) {
}

OrderProcessingResult::Status OrderProcessingResult::getStatus() const {
    return status;
}

const std::string& OrderProcessingResult::getOrderId() const {
    return orderId;
}

const std::string& OrderProcessingResult::getSymbol() const {
    return symbol;
}

const std::vector<std::shared_ptr<Trade>>& OrderProcessingResult::getTrades() const {
    return trades;
}

const std::string& OrderProcessingResult::getErrorMessage() const {
    return errorMessage;
}

// constructor & destructor
ContinuousMatchingEngine::ContinuousMatchingEngine(size_t numThreads) 
    : matchingEngine(std::make_unique<MatchingEngine>()), 
      threadPool(std::make_unique<SymbolThreadPool>(numThreads)),
      running(false) {
}

ContinuousMatchingEngine::~ContinuousMatchingEngine() {
    stop();
}

// start and stop methods for constructor & destructor

void ContinuousMatchingEngine::start() {
    // if engine is already running then do nothing
    if (isRunning()) {
        return;
    }

    // else, store that engine is now running
    running.store(true);

    // Start the thread pool
    threadPool->start();
    
    std::cout << "Continuous Matching Engine started" << std::endl;
}

void ContinuousMatchingEngine::stop() {
    // if the engine is not running then do nothing (already stopped)
    if (!isRunning()) {
        return;
    }

    // else, store that engine is no longer running
    running.store(false);

    // Stop the thread pool
    threadPool->stop();
    
    std::cout << "Continuous Matching Engine stopped" << std::endl;
}

bool ContinuousMatchingEngine::isRunning() const {
    return running.load();
}

void ContinuousMatchingEngine::submitOrder(std::shared_ptr<Order> order) {
    if (!order) {
        std::cerr << "Invalid order submitted" << std::endl;
        return;
    }
    
    if (!isRunning()) {
        std::cerr << "Engine is not running" << std::endl;
        return;
    }
    
    OrderRequest request;
    request.action = OrderAction::SUBMIT;
    request.order = order;
    
    // Submit the task to the thread pool for the specific symbol
    threadPool->submitTask(order->getSymbol(), [this, request]() {
        processOrder(request);
    });
}

void ContinuousMatchingEngine::cancelOrder(const std::string& orderId, const std::string& symbol) {
    if (!isRunning()) {
        std::cerr << "Engine is not running" << std::endl;
        return;
    }
    
    OrderRequest request;
    request.action = OrderAction::CANCEL;
    request.orderId = orderId;
    request.symbol = symbol;
    
    // Submit the task to the thread pool for the specific symbol
    threadPool->submitTask(symbol, [this, request]() {
        processOrder(request);
    });
}

bool ContinuousMatchingEngine::addSymbol(const std::string& symbol) {
    return matchingEngine->addSymbol(symbol);
}

bool ContinuousMatchingEngine::removeSymbol(const std::string& symbol) {
    return matchingEngine->removeSymbol(symbol);
}

bool ContinuousMatchingEngine::hasSymbol(const std::string& symbol) const {
    return matchingEngine->hasSymbol(symbol);
}

std::vector<std::string> ContinuousMatchingEngine::getSymbols() const {
    return matchingEngine->getSymbols();
}

std::shared_ptr<OrderBook> ContinuousMatchingEngine::getOrderBook(const std::string& symbol) const {
    return matchingEngine->getOrderBook(symbol);
}

void ContinuousMatchingEngine::registerTradeCallback(std::function<void(std::shared_ptr<Trade>)> callback) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    tradeCallbacks.push_back(callback);
}

void ContinuousMatchingEngine::registerOrderProcessingCallback(std::function<void(std::shared_ptr<OrderProcessingResult>)> callback) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    orderProcessingCallbacks.push_back(callback);
}

std::string ContinuousMatchingEngine::toString() const {
    // No need for mutex lock on matchingEngine access since it's read-only
    return matchingEngine->toString();
}

int ContinuousMatchingEngine::getThreadForSymbol(const std::string& symbol) const {
    return threadPool->getThreadForSymbol(symbol);
}

void ContinuousMatchingEngine::processOrder(const OrderRequest& request) {
    if (request.action == OrderAction::SUBMIT) {
        auto trades = matchingEngine->processOrder(request.order);
        
        OrderProcessingResult::Status status;
        if (trades.empty()) {
            if (request.order->getPrice() == 0.0) {
                status = OrderProcessingResult::Status::NO_MATCH;
            } else {
                status = OrderProcessingResult::Status::SUCCESS;
            }
        } else if (request.order->getQuantity() > 0) {
            status = OrderProcessingResult::Status::PARTIAL_FILL;
        } else {
            status = OrderProcessingResult::Status::SUCCESS;
        }
        
        auto result = std::shared_ptr<OrderProcessingResult>(
            new OrderProcessingResult(
                status,
                request.order->getId(),
                request.order->getSymbol(),
                trades
            )
        );
        
        notifyOrderProcessingCallbacks(result);
        
        for (const auto& trade : trades) {
            notifyTradeCallbacks(trade);
        }
    } else if (request.action == OrderAction::CANCEL) {
        bool success = matchingEngine->cancelOrder(request.orderId, request.symbol);
        
        auto result = std::shared_ptr<OrderProcessingResult>(
            new OrderProcessingResult(
                success ? OrderProcessingResult::Status::SUCCESS : OrderProcessingResult::Status::ERROR,
                request.orderId,
                request.symbol,
                {},
                success ? "" : "Failed to cancel order"
            )
        );
        
        notifyOrderProcessingCallbacks(result);
    }
}

void ContinuousMatchingEngine::notifyTradeCallbacks(std::shared_ptr<Trade> trade) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    for (const auto& callback : tradeCallbacks) {
        callback(trade);
    }
}

void ContinuousMatchingEngine::notifyOrderProcessingCallbacks(std::shared_ptr<OrderProcessingResult> result) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    for (const auto& callback : orderProcessingCallbacks) {
        callback(result);
    }
}

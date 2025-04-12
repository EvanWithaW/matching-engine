//
// Created by Evan Weidner on 4/11/25.
//

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include "../engine/ContinuousMatchingEngine.hpp"
#include "../order/OrderFactory.hpp"

class ContinuousMatchingEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        matchingEngine = std::make_unique<ContinuousMatchingEngine>();
        matchingEngine->addSymbol("AAPL");
        matchingEngine->start();
    }

    void TearDown() override {
        matchingEngine->stop();
    }

    std::unique_ptr<ContinuousMatchingEngine> matchingEngine;
};

// Test that the engine starts and stops correctly
TEST_F(ContinuousMatchingEngineTest, StartStop) {
    EXPECT_TRUE(matchingEngine->isRunning());
    
    matchingEngine->stop();
    EXPECT_FALSE(matchingEngine->isRunning());
    
    matchingEngine->start();
    EXPECT_TRUE(matchingEngine->isRunning());
}

// Test submitting a limit buy order with no matching sell orders
TEST_F(ContinuousMatchingEngineTest, SubmitLimitBuyOrderNoMatch) {
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
    
    // Set up a flag to track if the callback was called
    std::atomic<bool> callbackCalled(false);
    
    // Register a callback to check the result
    matchingEngine->registerOrderProcessingCallback([&callbackCalled](std::shared_ptr<OrderProcessingResult> result) {
        EXPECT_EQ(result->getStatus(), OrderProcessingResult::Status::SUCCESS);
        EXPECT_EQ(result->getTrades().size(), 0);
        callbackCalled = true;
    });
    
    // Submit the order
    matchingEngine->submitOrder(buyOrder);
    
    // Wait for the callback to be called
    for (int i = 0; i < 10 && !callbackCalled; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_TRUE(callbackCalled);
    
    // Check the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 0);
}

// Test submitting a limit sell order with no matching buy orders
TEST_F(ContinuousMatchingEngineTest, SubmitLimitSellOrderNoMatch) {
    auto sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 160.0, 100);
    
    // Set up a flag to track if the callback was called
    std::atomic<bool> callbackCalled(false);
    
    // Register a callback to check the result
    matchingEngine->registerOrderProcessingCallback([&callbackCalled](std::shared_ptr<OrderProcessingResult> result) {
        EXPECT_EQ(result->getStatus(), OrderProcessingResult::Status::SUCCESS);
        EXPECT_EQ(result->getTrades().size(), 0);
        callbackCalled = true;
    });
    
    // Submit the order
    matchingEngine->submitOrder(sellOrder);
    
    // Wait for the callback to be called
    for (int i = 0; i < 10 && !callbackCalled; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_TRUE(callbackCalled);
    
    // Check the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 1);
}

// Test matching a limit buy order with an existing limit sell order
TEST_F(ContinuousMatchingEngineTest, MatchLimitBuyWithLimitSell) {
    // Add a sell order first
    auto sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.0, 100);
    matchingEngine->submitOrder(sellOrder);
    
    // Wait for the sell order to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Set up flags to track if the callbacks were called
    std::atomic<bool> orderCallbackCalled(false);
    std::atomic<bool> tradeCallbackCalled(false);
    
    // Register callbacks to check the results
    matchingEngine->registerOrderProcessingCallback([&orderCallbackCalled](std::shared_ptr<OrderProcessingResult> result) {
        if (result->getStatus() == OrderProcessingResult::Status::SUCCESS && !result->getTrades().empty()) {
            orderCallbackCalled = true;
        }
    });
    
    matchingEngine->registerTradeCallback([&tradeCallbackCalled](std::shared_ptr<Trade> trade) {
        tradeCallbackCalled = true;
    });
    
    // Add a buy order that matches the sell order
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 50);
    matchingEngine->submitOrder(buyOrder);
    
    // Wait for the callbacks to be called
    for (int i = 0; i < 10 && (!orderCallbackCalled || !tradeCallbackCalled); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_TRUE(orderCallbackCalled);
    EXPECT_TRUE(tradeCallbackCalled);
    
    // Wait a bit more for the order book to be updated
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Check the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders()[0]->getQuantity(), 50);
}

// Test canceling an order
TEST_F(ContinuousMatchingEngineTest, CancelOrder) {
    // Add a buy order
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
    matchingEngine->submitOrder(buyOrder);
    
    // Wait for the order to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Set up a flag to track if the callback was called
    std::atomic<bool> callbackCalled(false);
    
    // Register a callback to check the result
    matchingEngine->registerOrderProcessingCallback([&callbackCalled](std::shared_ptr<OrderProcessingResult> result) {
        if (result->getStatus() == OrderProcessingResult::Status::SUCCESS && result->getTrades().empty()) {
            callbackCalled = true;
        }
    });
    
    // Cancel the order
    matchingEngine->cancelOrder(buyOrder->getId(), "AAPL");
    
    // Wait for the callback to be called
    for (int i = 0; i < 10 && !callbackCalled; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_TRUE(callbackCalled);
    
    // Wait a bit more for the order book to be updated
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Check the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
}

// Test submitting a market buy order with matching sell orders
TEST_F(ContinuousMatchingEngineTest, SubmitMarketBuyOrder) {
    // Add a sell order first
    auto sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.0, 100);
    matchingEngine->submitOrder(sellOrder);
    
    // Wait for the sell order to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Set up flags to track if the callbacks were called
    std::atomic<bool> orderCallbackCalled(false);
    std::atomic<bool> tradeCallbackCalled(false);
    
    // Register callbacks to check the results
    matchingEngine->registerOrderProcessingCallback([&orderCallbackCalled](std::shared_ptr<OrderProcessingResult> result) {
        if (result->getStatus() == OrderProcessingResult::Status::SUCCESS && !result->getTrades().empty()) {
            orderCallbackCalled = true;
        }
    });
    
    matchingEngine->registerTradeCallback([&tradeCallbackCalled](std::shared_ptr<Trade> trade) {
        tradeCallbackCalled = true;
    });
    
    // Add a market buy order
    auto buyOrder = OrderFactory::createMarketOrder("AAPL", OrderSide::BUY, 50);
    matchingEngine->submitOrder(buyOrder);
    
    // Wait for the callbacks to be called
    for (int i = 0; i < 10 && (!orderCallbackCalled || !tradeCallbackCalled); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_TRUE(orderCallbackCalled);
    EXPECT_TRUE(tradeCallbackCalled);
    
    // Wait a bit more for the order book to be updated
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Check the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders()[0]->getQuantity(), 50);
}

// Test submitting a market buy order with no matching sell orders
TEST_F(ContinuousMatchingEngineTest, SubmitMarketBuyOrderNoMatch) {
    // Set up a flag to track if the callback was called
    std::atomic<bool> callbackCalled(false);
    
    // Register a callback to check the result
    matchingEngine->registerOrderProcessingCallback([&callbackCalled](std::shared_ptr<OrderProcessingResult> result) {
        EXPECT_EQ(result->getStatus(), OrderProcessingResult::Status::NO_MATCH);
        EXPECT_EQ(result->getTrades().size(), 0);
        callbackCalled = true;
    });
    
    // Add a market buy order with no matching sell orders
    auto buyOrder = OrderFactory::createMarketOrder("AAPL", OrderSide::BUY, 50);
    matchingEngine->submitOrder(buyOrder);
    
    // Wait for the callback to be called
    for (int i = 0; i < 10 && !callbackCalled; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_TRUE(callbackCalled);
    
    // Check the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 0);
}

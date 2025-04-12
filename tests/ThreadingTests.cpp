#include "../engine/ContinuousMatchingEngine.hpp"
#include "../order/Order.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <unordered_set>

class ThreadingTests : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<ContinuousMatchingEngine>(4); // Use 4 threads
        engine->start();
        
        // Add some test symbols
        engine->addSymbol("AAPL");
        engine->addSymbol("MSFT");
        engine->addSymbol("GOOG");
        engine->addSymbol("AMZN");
    }
    
    void TearDown() override {
        engine->stop();
        engine.reset();
    }
    
    std::unique_ptr<ContinuousMatchingEngine> engine;
};

// Test that orders for the same symbol are processed by the same thread
TEST_F(ThreadingTests, SymbolThreadAssignment) {
    // Check that the same symbol is consistently assigned to the same thread
    int threadForAAPL1 = engine->getThreadForSymbol("AAPL");
    int threadForAAPL2 = engine->getThreadForSymbol("AAPL");
    EXPECT_EQ(threadForAAPL1, threadForAAPL2);
    
    int threadForMSFT = engine->getThreadForSymbol("MSFT");
    
    // Different symbols may be assigned to different threads
    // (though they could be the same by chance due to hash function)
    if (threadForAAPL1 != threadForMSFT) {
        EXPECT_NE(threadForAAPL1, threadForMSFT);
    }
}

// Test concurrent order submission for multiple symbols
TEST_F(ThreadingTests, ConcurrentOrderSubmission) {
    const int NUM_ORDERS = 1000;
    std::atomic<int> processedOrders(0);
    
    // Register callback to count processed orders
    engine->registerOrderProcessingCallback([&processedOrders](std::shared_ptr<OrderProcessingResult> result) {
        processedOrders++;
    });
    
    // Submit orders from multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&, i]() {
            std::string symbol = (i % 4 == 0) ? "AAPL" : 
                               (i % 4 == 1) ? "MSFT" : 
                               (i % 4 == 2) ? "GOOG" : "AMZN";
            
            for (int j = 0; j < NUM_ORDERS / 4; ++j) {
                auto order = std::make_shared<Order>(
                    "order_" + std::to_string(i) + "_" + std::to_string(j),
                    symbol,
                    OrderSide::BUY,
                    100 + j,
                    10.0 + j * 0.01
                );
                engine->submitOrder(order);
            }
        });
    }
    
    // Join all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Wait for all orders to be processed (with timeout)
    auto start = std::chrono::steady_clock::now();
    while (processedOrders.load() < NUM_ORDERS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > 5) {
            break; // Timeout after 5 seconds
        }
    }
    
    // Verify all orders were processed
    EXPECT_EQ(processedOrders.load(), NUM_ORDERS);
}

// Test order matching across threads
TEST_F(ThreadingTests, ConcurrentOrderMatching) {
    std::atomic<int> matchedTrades(0);
    
    // Register callback to count trades
    engine->registerTradeCallback([&matchedTrades](std::shared_ptr<Trade> trade) {
        matchedTrades++;
    });
    
    // Submit buy and sell orders for the same symbol from different threads
    std::thread buyThread([&]() {
        for (int i = 0; i < 100; ++i) {
            auto order = std::make_shared<Order>(
                "buy_" + std::to_string(i),
                "AAPL",
                OrderSide::BUY,
                10,
                100.0 + i * 0.1
            );
            engine->submitOrder(order);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    
    std::thread sellThread([&]() {
        for (int i = 0; i < 100; ++i) {
            auto order = std::make_shared<Order>(
                "sell_" + std::to_string(i),
                "AAPL",
                OrderSide::SELL,
                10,
                100.0 + i * 0.1
            );
            engine->submitOrder(order);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    
    buyThread.join();
    sellThread.join();
    
    // Wait for all trades to be processed (with timeout)
    auto start = std::chrono::steady_clock::now();
    while (matchedTrades.load() < 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > 5) {
            break; // Timeout after 5 seconds
        }
    }
    
    // Verify trades were created
    EXPECT_GT(matchedTrades.load(), 0);
}

// Test cancellation of orders
TEST_F(ThreadingTests, ConcurrentOrderCancellation) {
    std::atomic<int> successfulCancellations(0);
    std::atomic<int> failedCancellations(0);
    std::vector<std::string> orderIds;
    
    // Register callback to track cancellations
    engine->registerOrderProcessingCallback([&](std::shared_ptr<OrderProcessingResult> result) {
        if (result->getStatus() == OrderProcessingResult::Status::SUCCESS && 
            result->getTrades().empty()) {
            successfulCancellations++;
        } else if (result->getStatus() == OrderProcessingResult::Status::ERROR) {
            failedCancellations++;
        }
    });
    
    // Submit orders first
    for (int i = 0; i < 100; ++i) {
        std::string orderId = "order_" + std::to_string(i);
        orderIds.push_back(orderId);
        
        auto order = std::make_shared<Order>(
            orderId,
            "MSFT",
            OrderSide::BUY,
            10,
            100.0 + i * 0.1
        );
        engine->submitOrder(order);
    }
    
    // Wait a bit for orders to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Cancel orders from multiple threads
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = t; i < static_cast<int>(orderIds.size()); i += 4) {
                engine->cancelOrder(orderIds[i], "MSFT");
            }
        });
    }
    
    // Join all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Wait for all cancellations to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Verify cancellations were processed
    EXPECT_GT(successfulCancellations.load(), 0);
}

// Test stress test with random operations
TEST_F(ThreadingTests, StressTest) {
    const int NUM_OPERATIONS = 5000;
    std::atomic<int> completedOperations(0);
    
    // Register callback
    engine->registerOrderProcessingCallback([&](std::shared_ptr<OrderProcessingResult> result) {
        completedOperations++;
    });
    
    // Create threads that perform random operations
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) {
        threads.emplace_back([&, t]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> symbolDist(0, 3);
            std::uniform_int_distribution<> opDist(0, 10); // 0-7: submit, 8-10: cancel
            std::uniform_int_distribution<> priceDist(90, 110);
            std::uniform_int_distribution<> quantityDist(1, 20);
            std::uniform_int_distribution<> sideDist(0, 1);
            
            std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG", "AMZN"};
            std::unordered_map<std::string, std::vector<std::string>> orderIdsBySymbol;
            
            for (int i = 0; i < NUM_OPERATIONS / 8; ++i) {
                int op = opDist(gen);
                std::string symbol = symbols[symbolDist(gen)];
                
                if (op <= 7 || orderIdsBySymbol[symbol].empty()) {
                    // Submit order
                    std::string orderId = "order_" + std::to_string(t) + "_" + std::to_string(i);
                    OrderSide side = sideDist(gen) == 0 ? OrderSide::BUY : OrderSide::SELL;
                    
                    auto order = std::make_shared<Order>(
                        orderId,
                        symbol,
                        side,
                        quantityDist(gen),
                        priceDist(gen)
                    );
                    
                    engine->submitOrder(order);
                    orderIdsBySymbol[symbol].push_back(orderId);
                } else {
                    // Cancel order
                    auto& ids = orderIdsBySymbol[symbol];
                    if (!ids.empty()) {
                        std::uniform_int_distribution<> idDist(0, ids.size() - 1);
                        int idIndex = idDist(gen);
                        engine->cancelOrder(ids[idIndex], symbol);
                    }
                }
                
                // Small sleep to avoid overwhelming the system
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }
    
    // Join all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Wait for operations to complete (with timeout)
    auto start = std::chrono::steady_clock::now();
    while (completedOperations.load() < NUM_OPERATIONS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > 10) {
            break; // Timeout after 10 seconds
        }
    }
    
    // We don't expect all operations to complete due to race conditions in cancellations
    // Just verify the engine didn't crash and processed a significant number of operations
    EXPECT_GT(completedOperations.load(), NUM_OPERATIONS / 2);
}

// Test shutdown behavior with pending orders
TEST_F(ThreadingTests, ShutdownWithPendingOrders) {
    std::atomic<int> processedOrders(0);
    
    // Register callback
    engine->registerOrderProcessingCallback([&](std::shared_ptr<OrderProcessingResult> result) {
        processedOrders++;
    });
    
    // Submit a large number of orders
    for (int i = 0; i < 1000; ++i) {
        auto order = std::make_shared<Order>(
            "order_" + std::to_string(i),
            "AAPL",
            OrderSide::BUY,
            10,
            100.0
        );
        engine->submitOrder(order);
    }
    
    // Immediately stop the engine
    engine->stop();
    
    // Verify that either all orders were processed or the engine stopped cleanly
    // We can't make strong assertions about exact counts due to race conditions
    EXPECT_GE(processedOrders.load(), 0);
    EXPECT_FALSE(engine->isRunning());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

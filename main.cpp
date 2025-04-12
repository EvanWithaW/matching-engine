//
// Created by Evan Weidner on 4/9/25.
//

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "order/Order.hpp"
#include "order/OrderBook.hpp"
#include "order/OrderFactory.hpp"
#include "engine/MatchingEngine.hpp"
#include "engine/ContinuousMatchingEngine.hpp"
#include "engine/Trade.hpp"

// Callback function for trades
void onTrade(std::shared_ptr<Trade> trade) {
    std::cout << "Trade executed: " << trade->toString() << std::endl;
}

// Callback function for order processing results
void onOrderProcessed(std::shared_ptr<OrderProcessingResult> result) {
    std::cout << "Order processed: " << result->getOrderId() << " for symbol " << result->getSymbol() << std::endl;
    
    switch (result->getStatus()) {
        case OrderProcessingResult::Status::SUCCESS:
            std::cout << "  Status: SUCCESS" << std::endl;
            break;
        case OrderProcessingResult::Status::PARTIAL_FILL:
            std::cout << "  Status: PARTIAL_FILL" << std::endl;
            break;
        case OrderProcessingResult::Status::NO_MATCH:
            std::cout << "  Status: NO_MATCH" << std::endl;
            break;
        case OrderProcessingResult::Status::ERROR:
            std::cout << "  Status: ERROR - " << result->getErrorMessage() << std::endl;
            break;
    }
    
    if (!result->getTrades().empty()) {
        std::cout << "  Trades executed: " << result->getTrades().size() << std::endl;
    }
}

int main() {
    // Create a continuous matching engine
    auto matchingEngine = std::make_unique<ContinuousMatchingEngine>();
    
    // Register callbacks with engine instance
    matchingEngine->registerTradeCallback(onTrade);
    matchingEngine->registerOrderProcessingCallback(onOrderProcessed);
    
    // Add a symbol to the matching engine
    matchingEngine->addSymbol("AAPL");
    
    std::cout << "Continuous Matching Engine initialized with symbol AAPL" << std::endl;
    
    // Start the matching engine
    matchingEngine->start();
    
    // Create some orders
    auto buyOrder1 = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
    auto buyOrder2 = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 155.0, 50);
    auto sellOrder1 = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 160.0, 75);
    auto sellOrder2 = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 165.0, 100);
    
    // Submit the orders to the matching engine
    std::cout << "Submitting buy order: " << buyOrder1->toString() << std::endl;
    matchingEngine->submitOrder(buyOrder1);
    
    // Wait a moment to see the results
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Submitting buy order: " << buyOrder2->toString() << std::endl;
    matchingEngine->submitOrder(buyOrder2);
    
    // Wait a moment to see the results
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Submitting sell order: " << sellOrder1->toString() << std::endl;
    matchingEngine->submitOrder(sellOrder1);
    
    // Wait a moment to see the results
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Submitting sell order: " << sellOrder2->toString() << std::endl;
    matchingEngine->submitOrder(sellOrder2);
    
    // Wait a moment to see the results
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Display the current state of the matching engine
    std::cout << "\nCurrent state of the matching engine:" << std::endl;
    std::cout << matchingEngine->toString() << std::endl;
    
    // Create a market buy order that will match with existing sell orders
    std::cout << "\nCreating a market buy order for 50 shares" << std::endl;
    auto marketBuyOrder = OrderFactory::createMarketOrder("AAPL", OrderSide::BUY, 50);
    matchingEngine->submitOrder(marketBuyOrder);
    
    // Wait a moment to see the results
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Display the updated state of the matching engine
    std::cout << "\nUpdated state of the matching engine:" << std::endl;
    std::cout << matchingEngine->toString() << std::endl;
    
    // Cancel an order
    std::cout << "\nCancelling buy order: " << buyOrder1->getId() << std::endl;
    matchingEngine->cancelOrder(buyOrder1->getId(), "AAPL");
    
    // Wait a moment to see the results
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Display the final state of the matching engine
    std::cout << "\nFinal state of the matching engine:" << std::endl;
    std::cout << matchingEngine->toString() << std::endl;
    
    // Stop the matching engine
    std::cout << "\nStopping the matching engine..." << std::endl;
    matchingEngine->stop();
    
    return 0;
}
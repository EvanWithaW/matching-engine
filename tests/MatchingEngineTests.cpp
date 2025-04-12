//
// Created by Evan Weidner on 4/11/25.
//

#include <gtest/gtest.h>
#include "../engine/MatchingEngine.hpp"
#include "../engine/Trade.hpp"
#include "../order/OrderFactory.hpp"

class MatchingEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        matchingEngine = std::make_unique<MatchingEngine>();
        matchingEngine->addSymbol("AAPL");
    }

    std::unique_ptr<MatchingEngine> matchingEngine;
};

// Test adding and removing symbols
TEST_F(MatchingEngineTest, SymbolManagement) {
    // Add a new symbol
    EXPECT_TRUE(matchingEngine->addSymbol("MSFT"));
    EXPECT_TRUE(matchingEngine->hasSymbol("MSFT"));
    
    // Try to add an existing symbol
    EXPECT_FALSE(matchingEngine->addSymbol("AAPL"));
    
    // Get all symbols
    auto symbols = matchingEngine->getSymbols();
    EXPECT_EQ(symbols.size(), 2);
    EXPECT_TRUE(std::find(symbols.begin(), symbols.end(), "AAPL") != symbols.end());
    EXPECT_TRUE(std::find(symbols.begin(), symbols.end(), "MSFT") != symbols.end());
    
    // Remove a symbol
    EXPECT_TRUE(matchingEngine->removeSymbol("MSFT"));
    EXPECT_FALSE(matchingEngine->hasSymbol("MSFT"));
    
    // Try to remove a non-existent symbol
    EXPECT_FALSE(matchingEngine->removeSymbol("GOOG"));
}

// Test processing a limit buy order with no matching sell orders
TEST_F(MatchingEngineTest, ProcessLimitBuyOrderNoMatch) {
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
    
    auto trades = matchingEngine->processOrder(buyOrder);
    
    // No trades should be executed
    EXPECT_EQ(trades.size(), 0);
    
    // The order should be added to the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 0);
    
    // Check the best bid price
    EXPECT_EQ(matchingEngine->getBestBidPrice("AAPL"), 150.0);
}

// Test processing a limit sell order with no matching buy orders
TEST_F(MatchingEngineTest, ProcessLimitSellOrderNoMatch) {
    auto sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 160.0, 100);
    
    auto trades = matchingEngine->processOrder(sellOrder);
    
    // No trades should be executed
    EXPECT_EQ(trades.size(), 0);
    
    // The order should be added to the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 1);
    
    // Check the best ask price
    EXPECT_EQ(matchingEngine->getBestAskPrice("AAPL"), 160.0);
}

// Test matching a limit buy order with an existing limit sell order
TEST_F(MatchingEngineTest, MatchLimitBuyWithLimitSell) {
    // Add a sell order first
    auto sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.0, 100);
    matchingEngine->processOrder(sellOrder);
    
    // Add a buy order that matches the sell order
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 50);
    auto trades = matchingEngine->processOrder(buyOrder);
    
    // One trade should be executed
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0]->getQuantity(), 50);
    EXPECT_EQ(trades[0]->getPrice(), 150.0);
    EXPECT_EQ(trades[0]->getBuyOrderId(), buyOrder->getId());
    EXPECT_EQ(trades[0]->getSellOrderId(), sellOrder->getId());
    
    // The sell order should be partially filled
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders()[0]->getQuantity(), 50);
}

// Test matching a limit sell order with an existing limit buy order
TEST_F(MatchingEngineTest, MatchLimitSellWithLimitBuy) {
    // Add a buy order first
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
    matchingEngine->processOrder(buyOrder);
    
    // Add a sell order that matches the buy order
    auto sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.0, 50);
    auto trades = matchingEngine->processOrder(sellOrder);
    
    // One trade should be executed
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0]->getQuantity(), 50);
    EXPECT_EQ(trades[0]->getPrice(), 150.0);
    EXPECT_EQ(trades[0]->getBuyOrderId(), buyOrder->getId());
    EXPECT_EQ(trades[0]->getSellOrderId(), sellOrder->getId());
    
    // The buy order should be partially filled
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllBuyOrders()[0]->getQuantity(), 50);
}

// Test matching a market buy order with an existing limit sell order
TEST_F(MatchingEngineTest, MatchMarketBuyWithLimitSell) {
    // Add a sell order first
    auto sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.0, 100);
    matchingEngine->processOrder(sellOrder);
    
    // Add a market buy order
    auto buyOrder = OrderFactory::createMarketOrder("AAPL", OrderSide::BUY, 50);
    auto trades = matchingEngine->processOrder(buyOrder);
    
    // One trade should be executed
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0]->getQuantity(), 50);
    EXPECT_EQ(trades[0]->getPrice(), 150.0);  // Market order executes at the sell order's price
    
    // The sell order should be partially filled
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders()[0]->getQuantity(), 50);
}

// Test matching a market sell order with an existing limit buy order
TEST_F(MatchingEngineTest, MatchMarketSellWithLimitBuy) {
    // Add a buy order first
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
    matchingEngine->processOrder(buyOrder);
    
    // Add a market sell order
    auto sellOrder = OrderFactory::createMarketOrder("AAPL", OrderSide::SELL, 50);
    auto trades = matchingEngine->processOrder(sellOrder);
    
    // One trade should be executed
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0]->getQuantity(), 50);
    EXPECT_EQ(trades[0]->getPrice(), 150.0);  // Market order executes at the buy order's price
    
    // The buy order should be partially filled
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 1);
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 0);
    EXPECT_EQ(orderBook->getAllBuyOrders()[0]->getQuantity(), 50);
}

// Test canceling an order
TEST_F(MatchingEngineTest, CancelOrder) {
    // Add a buy order
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
    matchingEngine->processOrder(buyOrder);
    
    // Cancel the order
    EXPECT_TRUE(matchingEngine->cancelOrder(buyOrder->getId(), "AAPL"));
    
    // The order book should be empty
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
    
    // Try to cancel a non-existent order
    EXPECT_FALSE(matchingEngine->cancelOrder("non-existent-id", "AAPL"));
}

// Test matching multiple orders
TEST_F(MatchingEngineTest, MatchMultipleOrders) {
    // Add multiple sell orders with different prices
    auto sellOrder1 = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.0, 50);
    auto sellOrder2 = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 155.0, 50);
    auto sellOrder3 = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 160.0, 50);
    
    matchingEngine->processOrder(sellOrder1);
    matchingEngine->processOrder(sellOrder2);
    matchingEngine->processOrder(sellOrder3);
    
    // Add a buy order that matches all sell orders
    auto buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 160.0, 150);
    auto trades = matchingEngine->processOrder(buyOrder);
    
    // Three trades should be executed
    EXPECT_EQ(trades.size(), 3);
    
    // Trades should be executed in price-time priority
    EXPECT_EQ(trades[0]->getPrice(), 150.0);
    EXPECT_EQ(trades[1]->getPrice(), 155.0);
    EXPECT_EQ(trades[2]->getPrice(), 160.0);
    
    // All sell orders should be fully matched
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllSellOrders().size(), 0);
    
    // The buy order should be fully matched
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
}

// Test market order with no matching orders
TEST_F(MatchingEngineTest, MarketOrderNoMatch) {
    // Add a market buy order with no matching sell orders
    auto buyOrder = OrderFactory::createMarketOrder("AAPL", OrderSide::BUY, 100);
    auto trades = matchingEngine->processOrder(buyOrder);
    
    // No trades should be executed
    EXPECT_EQ(trades.size(), 0);
    
    // The market order should not be added to the order book
    auto orderBook = matchingEngine->getOrderBook("AAPL");
    EXPECT_EQ(orderBook->getAllBuyOrders().size(), 0);
}

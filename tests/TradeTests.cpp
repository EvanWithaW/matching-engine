//
// Created by Evan Weidner on 4/11/25.
//

#include <gtest/gtest.h>
#include "../engine/Trade.hpp"
#include "../order/OrderFactory.hpp"

class TradeTest : public ::testing::Test {
protected:
    void SetUp() override {
        buyOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.0, 100);
        sellOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.0, 100);
    }

    std::shared_ptr<Order> buyOrder;
    std::shared_ptr<Order> sellOrder;
};

// Test trade creation
TEST_F(TradeTest, CreateTrade) {
    auto trade = Trade::createTrade(buyOrder, sellOrder, 150.0, 50);
    
    EXPECT_NE(trade, nullptr);
    EXPECT_EQ(trade->getSymbol(), "AAPL");
    EXPECT_EQ(trade->getBuyOrderId(), buyOrder->getId());
    EXPECT_EQ(trade->getSellOrderId(), sellOrder->getId());
    EXPECT_EQ(trade->getPrice(), 150.0);
    EXPECT_EQ(trade->getQuantity(), 50);
}

// Test trade creation with invalid parameters
TEST_F(TradeTest, CreateTradeInvalidParams) {
    // Null buy order
    auto trade1 = Trade::createTrade(nullptr, sellOrder, 150.0, 50);
    EXPECT_EQ(trade1, nullptr);
    
    // Null sell order
    auto trade2 = Trade::createTrade(buyOrder, nullptr, 150.0, 50);
    EXPECT_EQ(trade2, nullptr);
    
    // Invalid price
    auto trade3 = Trade::createTrade(buyOrder, sellOrder, 0.0, 50);
    EXPECT_EQ(trade3, nullptr);
    
    // Invalid quantity
    auto trade4 = Trade::createTrade(buyOrder, sellOrder, 150.0, 0);
    EXPECT_EQ(trade4, nullptr);
    
    // Different symbols
    auto otherSellOrder = OrderFactory::createLimitOrder("MSFT", OrderSide::SELL, 150.0, 100);
    auto trade5 = Trade::createTrade(buyOrder, otherSellOrder, 150.0, 50);
    EXPECT_EQ(trade5, nullptr);
}

// Test trade ID generation
TEST_F(TradeTest, GenerateTradeId) {
    std::string id1 = Trade::generateTradeId();
    std::string id2 = Trade::generateTradeId();
    
    // IDs should be unique
    EXPECT_NE(id1, id2);
    
    // IDs should start with 'T'
    EXPECT_EQ(id1[0], 'T');
    EXPECT_EQ(id2[0], 'T');
}

// Test trade toString
TEST_F(TradeTest, ToString) {
    auto trade = Trade::createTrade(buyOrder, sellOrder, 150.0, 50);
    
    std::string tradeString = trade->toString();
    
    // Check that the string contains the trade details
    EXPECT_NE(tradeString.find("Trade"), std::string::npos);
    EXPECT_NE(tradeString.find(trade->getId()), std::string::npos);
    EXPECT_NE(tradeString.find("AAPL"), std::string::npos);
    EXPECT_NE(tradeString.find(buyOrder->getId()), std::string::npos);
    EXPECT_NE(tradeString.find(sellOrder->getId()), std::string::npos);
    EXPECT_NE(tradeString.find("150"), std::string::npos);
    EXPECT_NE(tradeString.find("50"), std::string::npos);
}

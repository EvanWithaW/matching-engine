#include <gtest/gtest.h>
#include "order/OrderFactory.hpp"

TEST(OrderFactoryTest, CreateLimitOrder) {
    auto order = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.25, 100);
    
    ASSERT_NE(nullptr, order);
    EXPECT_EQ("AAPL", order->getSymbol());
    EXPECT_EQ(OrderSide::BUY, order->getSide());
    EXPECT_DOUBLE_EQ(150.25, order->getPrice());
    EXPECT_EQ(100, order->getQuantity());
}

TEST(OrderFactoryTest, CreateMarketOrder) {
    auto order = OrderFactory::createMarketOrder("AAPL", OrderSide::SELL, 50);
    
    ASSERT_NE(nullptr, order);
    EXPECT_EQ("AAPL", order->getSymbol());
    EXPECT_EQ(OrderSide::SELL, order->getSide());
    EXPECT_DOUBLE_EQ(0.0, order->getPrice()); // Market orders use price 0.0
    EXPECT_EQ(50, order->getQuantity());
}

TEST(OrderFactoryTest, InvalidLimitOrder) {
    // Test with invalid quantity
    auto invalidOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.25, -10);
    EXPECT_EQ(nullptr, invalidOrder);
    
    // Test with invalid price
    invalidOrder = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, -5.0, 100);
    EXPECT_EQ(nullptr, invalidOrder);
}

TEST(OrderFactoryTest, InvalidMarketOrder) {
    // Test with invalid quantity
    auto invalidOrder = OrderFactory::createMarketOrder("AAPL", OrderSide::SELL, -10);
    EXPECT_EQ(nullptr, invalidOrder);
}

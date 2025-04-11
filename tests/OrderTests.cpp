#include <gtest/gtest.h>
#include "order/Order.hpp"

TEST(OrderTest, ConstructorAndGetters) {
    Order order("ORD001", "AAPL", OrderSide::BUY, 150.25, 100);
    
    EXPECT_EQ("ORD001", order.getId());
    EXPECT_EQ("AAPL", order.getSymbol());
    EXPECT_EQ(OrderSide::BUY, order.getSide());
    EXPECT_DOUBLE_EQ(150.25, order.getPrice());
    EXPECT_EQ(100, order.getQuantity());
}

TEST(OrderTest, SideChecks) {
    Order buyOrder("ORD001", "AAPL", OrderSide::BUY, 150.25, 100);
    Order sellOrder("ORD002", "AAPL", OrderSide::SELL, 150.50, 50);
    
    EXPECT_TRUE(buyOrder.isBuy());
    EXPECT_FALSE(buyOrder.isSell());
    
    EXPECT_FALSE(sellOrder.isBuy());
    EXPECT_TRUE(sellOrder.isSell());
}

TEST(OrderTest, SetQuantity) {
    Order order("ORD001", "AAPL", OrderSide::BUY, 150.25, 100);
    
    order.setQuantity(75);
    EXPECT_EQ(75, order.getQuantity());
}

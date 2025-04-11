#include <gtest/gtest.h>
#include "order/OrderBook.hpp"
#include "order/OrderFactory.hpp"

class OrderBookTest : public ::testing::Test {
protected:
    void SetUp() override {
        orderBook = std::make_unique<OrderBook>("AAPL");
        buyOrder1 = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.25, 100);
        buyOrder2 = OrderFactory::createLimitOrder("AAPL", OrderSide::BUY, 150.50, 75);
        sellOrder1 = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 150.75, 50);
        sellOrder2 = OrderFactory::createLimitOrder("AAPL", OrderSide::SELL, 151.00, 25);
    }

    std::unique_ptr<OrderBook> orderBook;
    std::shared_ptr<Order> buyOrder1;
    std::shared_ptr<Order> buyOrder2;
    std::shared_ptr<Order> sellOrder1;
    std::shared_ptr<Order> sellOrder2;
};

TEST_F(OrderBookTest, AddOrder) {
    EXPECT_TRUE(orderBook->addOrder(buyOrder1));
    EXPECT_TRUE(orderBook->addOrder(sellOrder1));
    
    // Adding the same order twice should fail
    EXPECT_FALSE(orderBook->addOrder(buyOrder1));
}

TEST_F(OrderBookTest, CancelOrder) {
    orderBook->addOrder(buyOrder1);
    
    EXPECT_TRUE(orderBook->cancelOrder(buyOrder1->getId()));
    EXPECT_FALSE(orderBook->cancelOrder(buyOrder1->getId())); // Already cancelled
    EXPECT_FALSE(orderBook->cancelOrder("non-existent-id")); // Non-existent order
}

TEST_F(OrderBookTest, GetOrderById) {
    orderBook->addOrder(buyOrder1);
    
    auto retrievedOrder = orderBook->getOrderById(buyOrder1->getId());
    ASSERT_NE(nullptr, retrievedOrder);
    EXPECT_EQ(buyOrder1->getId(), retrievedOrder->getId());
    
    // Non-existent order should return nullptr
    EXPECT_EQ(nullptr, orderBook->getOrderById("non-existent-id"));
}

TEST_F(OrderBookTest, BestPrices) {
    // Empty order book should return 0 for best prices
    EXPECT_DOUBLE_EQ(0.0, orderBook->getBestBidPrice());
    EXPECT_DOUBLE_EQ(0.0, orderBook->getBestAskPrice());
    
    // Add orders
    orderBook->addOrder(buyOrder1);
    orderBook->addOrder(buyOrder2);
    orderBook->addOrder(sellOrder1);
    orderBook->addOrder(sellOrder2);
    
    // Check best prices
    EXPECT_DOUBLE_EQ(150.50, orderBook->getBestBidPrice()); // Highest buy price
    EXPECT_DOUBLE_EQ(150.75, orderBook->getBestAskPrice()); // Lowest sell price
}

TEST_F(OrderBookTest, GetSizes) {
    orderBook->addOrder(buyOrder1);
    orderBook->addOrder(buyOrder2);
    orderBook->addOrder(sellOrder1);
    orderBook->addOrder(sellOrder2);
    
    EXPECT_EQ(100, orderBook->getBidSize(150.25));
    EXPECT_EQ(75, orderBook->getBidSize(150.50));
    EXPECT_EQ(50, orderBook->getAskSize(150.75));
    EXPECT_EQ(25, orderBook->getAskSize(151.00));
    
    // Non-existent price levels should return 0
    EXPECT_EQ(0, orderBook->getBidSize(999.99));
    EXPECT_EQ(0, orderBook->getAskSize(999.99));
}

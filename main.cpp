//
// Created by Evan Weidner on 4/9/25.
//

#include <iostream>
#include "Order.h"

int main() {
    std::cout << "=== Matching Engine Test ===" << std::endl;
    
    // Create a buy order
    Order buyOrder("ORD001", "AAPL", OrderSide::BUY, 150.25, 100);
    
    // Create a sell order
    Order sellOrder("ORD002", "AAPL", OrderSide::SELL, 150.50, 50);
    
    // Print the orders
    std::cout << "Buy Order: " << buyOrder.toString() << std::endl;
    std::cout << "Sell Order: " << sellOrder.toString() << std::endl;
    
    // Test the utility methods
    std::cout << "\nBuy Order is buy: " << (buyOrder.isBuy() ? "true" : "false") << std::endl;
    std::cout << "Sell Order is sell: " << (sellOrder.isSell() ? "true" : "false") << std::endl;
    
    // Test quantity modification
    buyOrder.setQuantity(75);
    std::cout << "\nModified Buy Order: " << buyOrder.toString() << std::endl;
    
    return 0;
}
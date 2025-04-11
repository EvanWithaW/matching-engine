//
// Created by Evan Weidner on 4/11/25.
//

#include "OrderFactory.hpp"
#include <iostream>

int OrderFactory::orderIdCounter = 0;

std::string OrderFactory::generateOrderId() {
    return std::to_string(++orderIdCounter);
}

bool OrderFactory::validateOrderParameters(const std::string& symbol, OrderSide side, double price, int quantity) {
    // TODO: add logic for symbol checking (does it exist for example)

    // price checking (never negative)
    if (price < 0) {
        std::cerr << "Invalid price: " << price << std::endl;
        return false;
    }

    // quantity checking (never negative)
    if (quantity <= 0) {
        std::cerr << "Invalid quantity: " << quantity << std::endl;
        return false;
    }

    return true;
}

std::shared_ptr<Order> OrderFactory::createLimitOrder(const std::string& symbol, OrderSide side, double price, int quantity, const std::string& callerId) {
    if (!validateOrderParameters(symbol, side, price, quantity)) {
        return nullptr;
    }

    return std::make_shared<Order>(generateOrderId(), symbol, side, price, quantity);
}

std::shared_ptr<Order> OrderFactory::createMarketOrder(const std::string& symbol, OrderSide side, int quantity, const std::string& callerId) {
    // Market orders don't have a price, but we still need to validate the quantity
    if (quantity <= 0) {
        std::cerr << "Invalid quantity: " << quantity << std::endl;
        return nullptr;
    }

    // For market orders, we use price 0.0 as a convention to indicate it's a market order
    // The actual execution price will be determined at matching time
    return std::make_shared<Order>(generateOrderId(), symbol, side, 0.0, quantity);
}
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
    if (price < 0) {
        std::cerr << "Invalid price: " << price << std::endl;
        return false;
    }

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
    if (quantity <= 0) {
        std::cerr << "Invalid quantity: " << quantity << std::endl;
        return nullptr;
    }

    return std::make_shared<Order>(generateOrderId(), symbol, side, 0.0, quantity);
}
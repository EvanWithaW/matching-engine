//
// Created by Evan Weidner on 4/9/25.
//

#include "Order.hpp"
#include <sstream>
// This class should only be used to create an order, all setting should be done at initialization time.
// Therefore, we don't need any setters, only getter methods.

Order::Order(const std::string &id, const std::string &symbol, OrderSide side, double price, int quantity) {
    this->id = id;
    this->symbol = symbol;
    this->side = side;
    this->price = price;
    this->quantity = quantity;
    this->timestamp = std::chrono::system_clock::now();
}

// Getters
const std::string& Order::getId() const {
    return id;
}

const std::string& Order::getSymbol() const {
    return symbol;
}

OrderSide Order::getSide() const {
    return side;
}

double Order::getPrice() const {
    return price;
}

int Order::getQuantity() const {
    return quantity;
}

// Setters
void Order::setQuantity(int newQuantity) {
    this->quantity = newQuantity;
}

const std::chrono::time_point<std::chrono::system_clock>& Order::getTimestamp() const {
    return timestamp;
}

// Logic methods for is buy/sell

bool Order::isBuy() const {
    return side == OrderSide::BUY;
}

bool Order::isSell() const {
    return side == OrderSide::SELL;
}

// ToString impl for testing

std::string Order::toString() const {
    std::stringstream ss;
    ss << "Order{id='" << id << "'"
       << ", symbol='" << symbol << "'"
       << ", side=" << (side == OrderSide::BUY ? "BUY" : "SELL")
       << ", price=" << price
       << ", quantity=" << quantity
       << "}";
    return ss.str();
}
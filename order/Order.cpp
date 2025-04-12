//
// Created by Evan Weidner on 4/9/25.
//

#include "Order.hpp"
#include <sstream>

Order::Order(const std::string& id, const std::string& symbol, OrderSide side, double price, int quantity)
    : id(id), symbol(symbol), side(side), price(price), quantity(quantity), timestamp(std::chrono::system_clock::now()) {
}

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

const std::chrono::time_point<std::chrono::system_clock>& Order::getTimestamp() const {
    return timestamp;
}

void Order::setQuantity(int newQuantity) {
    quantity = newQuantity;
}

bool Order::isBuy() const {
    return side == OrderSide::BUY;
}

bool Order::isSell() const {
    return side == OrderSide::SELL;
}

std::string Order::toString() const {
    std::stringstream ss;
    ss << "Order{id='" << id << "', symbol='" << symbol << "', side=";
    
    if (side == OrderSide::BUY) {
        ss << "BUY";
    } else {
        ss << "SELL";
    }
    
    ss << ", price=" << price << ", quantity=" << quantity << "}";
    return ss.str();
}
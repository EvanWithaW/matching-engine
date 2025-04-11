//
// Created by Evan Weidner on 4/9/25.
//

#include "OrderBook.hpp"
#include <sstream>

OrderBook::OrderBook(const std::string& symbol) : symbol(symbol) {
}

bool OrderBook::addOrder(std::shared_ptr<Order> order) {
    // Check if order already exists
    if (ordersSortedById.find(order->getId()) != ordersSortedById.end()) {
        return false;
    }
    
    // Add to the appropriate set based on side
    if (order->isBuy()) {
        buyOrders.insert(order);
    } else {
        sellOrders.insert(order);
    }
    
    // Add to id lookup map
    ordersSortedById[order->getId()] = order;
    return true;
}

bool OrderBook::cancelOrder(const std::string& orderId) {
    auto it = ordersSortedById.find(orderId);
    if (it == ordersSortedById.end()) {
        return false;
    }
    
    std::shared_ptr<Order> order = it->second;
    
    // Remove from appropriate set
    if (order->isBuy()) {
        buyOrders.erase(order);
    } else {
        sellOrders.erase(order);
    }
    
    // Remove from id map
    ordersSortedById.erase(it);
    return true;
}

std::shared_ptr<Order> OrderBook::getOrderById(const std::string& orderId) const {
    auto it = ordersSortedById.find(orderId);
    if (it != ordersSortedById.end()) {
        return it->second;
    }
    return nullptr;
}

double OrderBook::getBestBidPrice() const {
    if (buyOrders.empty()) {
        return 0.0;
    }
    return (*buyOrders.begin())->getPrice();
}

double OrderBook::getBestAskPrice() const {
    if (sellOrders.empty()) {
        return 0.0;
    }
    return (*sellOrders.begin())->getPrice();
}

int OrderBook::getBidSize(double price) const {
    int totalSize = 0;
    for (const auto& order : buyOrders) {
        if (order->getPrice() == price) {
            totalSize += order->getQuantity();
        }
    }
    return totalSize;
}

int OrderBook::getAskSize(double price) const {
    int totalSize = 0;
    for (const auto& order : sellOrders) {
        if (order->getPrice() == price) {
            totalSize += order->getQuantity();
        }
    }
    return totalSize;
}

std::string OrderBook::getSymbol() const {
    return symbol;
}

std::vector<std::shared_ptr<Order>> OrderBook::getAllBuyOrders() const {
    return std::vector<std::shared_ptr<Order>>(buyOrders.begin(), buyOrders.end());
}

std::vector<std::shared_ptr<Order>> OrderBook::getAllSellOrders() const {
    return std::vector<std::shared_ptr<Order>>(sellOrders.begin(), sellOrders.end());
}

std::string OrderBook::toString() const {
    std::stringstream ss;
    ss << "OrderBook for " << symbol << ":\n";
    
    ss << "Buy Orders:\n";
    for (const auto& order : buyOrders) {
        ss << "  " << order->toString() << "\n";
    }
    
    ss << "Sell Orders:\n";
    for (const auto& order : sellOrders) {
        ss << "  " << order->toString() << "\n";
    }
    
    return ss.str();
}

//
// Created by Evan Weidner on 4/9/25.
//

#include "OrderBook.hpp"
#include <sstream>
#include <iostream>

OrderBook::OrderBook(const ::std::string& symbol) : symbol(symbol) {
}

bool OrderBook::addOrder(::std::shared_ptr<Order> order) {
    if (!order) {
        return false;
    }

    if (order->getSymbol() != symbol) {
        return false;
    }

    if (ordersSortedById.find(order->getId()) != ordersSortedById.end()) {
        return false;
    }

    ordersSortedById[order->getId()] = order;

    if (order->isBuy()) {
        buyOrders.insert(order);
    } else {
        sellOrders.insert(order);
    }

    return true;
}

bool OrderBook::cancelOrder(const ::std::string& orderId) {
    auto it = ordersSortedById.find(orderId);
    if (it == ordersSortedById.end()) {
        return false;
    }

    auto order = it->second;
    ordersSortedById.erase(it);

    if (order->isBuy()) {
        buyOrders.erase(order);
    } else {
        sellOrders.erase(order);
    }

    return true;
}

::std::shared_ptr<Order> OrderBook::getOrderById(const ::std::string& orderId) const {
    auto it = ordersSortedById.find(orderId);
    if (it == ordersSortedById.end()) {
        return nullptr;
    }
    return it->second;
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
    int size = 0;
    for (const auto& order : buyOrders) {
        if (order->getPrice() == price) {
            size += order->getQuantity();
        } else if (order->getPrice() < price) {
            break;
        }
    }
    return size;
}

int OrderBook::getAskSize(double price) const {
    int size = 0;
    for (const auto& order : sellOrders) {
        if (order->getPrice() == price) {
            size += order->getQuantity();
        } else if (order->getPrice() > price) {
            break;
        }
    }
    return size;
}

::std::string OrderBook::getSymbol() const {
    return symbol;
}

::std::vector<::std::shared_ptr<Order>> OrderBook::getAllBuyOrders() const {
    return ::std::vector<::std::shared_ptr<Order>>(buyOrders.begin(), buyOrders.end());
}

::std::vector<::std::shared_ptr<Order>> OrderBook::getAllSellOrders() const {
    return ::std::vector<::std::shared_ptr<Order>>(sellOrders.begin(), sellOrders.end());
}

::std::string OrderBook::toString() const {
    ::std::stringstream ss;
    ss << "OrderBook for " << symbol << ":" << ::std::endl;
    ss << "Buy Orders:" << ::std::endl;
    for (const auto& order : buyOrders) {
        ss << "  " << order->toString() << ::std::endl;
    }
    ss << "Sell Orders:" << ::std::endl;
    for (const auto& order : sellOrders) {
        ss << "  " << order->toString() << ::std::endl;
    }
    return ss.str();
}

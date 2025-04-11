//
// Created by Evan Weidner on 4/9/25.
//

#ifndef MATCHING_ENGINE_ORDERBOOK_H
#define MATCHING_ENGINE_ORDERBOOK_H

#include "Order.hpp"
#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>

struct BuyComparator {
    bool operator()(const ::std::shared_ptr<Order>& lhs, const ::std::shared_ptr<Order>& rhs) const {
        // if the same price, then return the earliest as they came first
        if (lhs->getPrice() == rhs->getPrice()) {
            return lhs->getTimestamp() < rhs->getTimestamp();
        } else {
            // else return the higher price as they're paying more (better bid)
            return lhs->getPrice() > rhs->getPrice();
        }
    }
};

struct SellComparator {
    bool operator()(const ::std::shared_ptr<Order>& lhs, const ::std::shared_ptr<Order>& rhs) const {
        // if the same price, then return the earliest as they came first
        if (lhs->getPrice() == rhs->getPrice()) {
            return lhs->getTimestamp() < rhs->getTimestamp();
        } else {
            // else return the lower price as they're selling for less (better ask)
            return lhs->getPrice() < rhs->getPrice();
        }
    }
};

class OrderBook {
private:
    ::std::string symbol;
    ::std::set<::std::shared_ptr<Order>, BuyComparator> buyOrders;
    ::std::set<::std::shared_ptr<Order>, SellComparator> sellOrders;
    ::std::map<::std::string, ::std::shared_ptr<Order>> ordersSortedById;

public:
    explicit OrderBook(const ::std::string& symbol);

    bool addOrder(::std::shared_ptr<Order> order);
    bool cancelOrder(const ::std::string& orderId);
    ::std::shared_ptr<Order> getOrderById(const ::std::string& orderId) const;

    double getBestBidPrice() const;
    double getBestAskPrice() const;
    int getBidSize(double price) const;
    int getAskSize(double price) const;

    ::std::string getSymbol() const;
    ::std::vector<::std::shared_ptr<Order>> getAllBuyOrders() const;
    ::std::vector<::std::shared_ptr<Order>> getAllSellOrders() const;

    ::std::string toString() const;
};

#endif // MATCHING_ENGINE_ORDERBOOK_H
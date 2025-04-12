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
    bool operator()(const ::std::shared_ptr<Order>& lhs, const ::std::shared_ptr<Order>& rhs) const;
};

struct SellComparator {
    bool operator()(const ::std::shared_ptr<Order>& lhs, const ::std::shared_ptr<Order>& rhs) const;
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
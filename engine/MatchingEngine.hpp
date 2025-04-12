//
// Created by Evan Weidner on 4/11/25.
//

#ifndef MATCHING_ENGINE_MATCHINGENGINE_HPP
#define MATCHING_ENGINE_MATCHINGENGINE_HPP

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include "../order/OrderBook.hpp"
#include "../order/Order.hpp"

class Trade;

class MatchingEngine {
private:
    std::unordered_map<std::string, std::shared_ptr<OrderBook>> orderBooks;

public:
    MatchingEngine();
    ~MatchingEngine();

    bool addSymbol(const std::string& symbol);
    bool removeSymbol(const std::string& symbol);
    bool hasSymbol(const std::string& symbol) const;
    std::vector<std::string> getSymbols() const;
    std::shared_ptr<OrderBook> getOrderBook(const std::string& symbol) const;
    std::vector<std::shared_ptr<Trade>> processOrder(std::shared_ptr<Order> order);
    bool cancelOrder(const std::string& orderId, const std::string& symbol);
    double getBestBidPrice(const std::string& symbol) const;
    double getBestAskPrice(const std::string& symbol) const;
    int getBidSize(const std::string& symbol, double price) const;
    int getAskSize(const std::string& symbol, double price) const;
    std::string toString() const;

private:
    std::vector<std::shared_ptr<Trade>> matchBuyOrder(std::shared_ptr<Order> order, std::shared_ptr<OrderBook> orderBook);
    std::vector<std::shared_ptr<Trade>> matchSellOrder(std::shared_ptr<Order> order, std::shared_ptr<OrderBook> orderBook);
    bool canMatchMarketOrder(std::shared_ptr<Order> order, std::shared_ptr<OrderBook> orderBook) const;
};

#endif // MATCHING_ENGINE_MATCHINGENGINE_HPP

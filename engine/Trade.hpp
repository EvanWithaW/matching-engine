//
// Created by Evan Weidner on 4/11/25.
//

#ifndef MATCHING_ENGINE_TRADE_HPP
#define MATCHING_ENGINE_TRADE_HPP

#include <string>
#include <chrono>
#include <memory>
#include "../order/Order.hpp"

class Trade {
private:
    std::string id;
    std::string symbol;
    std::string buyOrderId;
    std::string sellOrderId;
    double price;
    int quantity;
    std::chrono::time_point<std::chrono::system_clock> timestamp;

public:
    Trade(const std::string& id, 
          const std::string& symbol, 
          const std::string& buyOrderId, 
          const std::string& sellOrderId, 
          double price, 
          int quantity);

    const std::string& getId() const;
    const std::string& getSymbol() const;
    const std::string& getBuyOrderId() const;
    const std::string& getSellOrderId() const;
    double getPrice() const;
    int getQuantity() const;
    const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;

    static std::shared_ptr<Trade> createTrade(
        std::shared_ptr<Order> buyOrder,
        std::shared_ptr<Order> sellOrder,
        double price,
        int quantity);

    static std::string generateTradeId();

    std::string toString() const;

private:
    static int tradeIdCounter;
};

#endif // MATCHING_ENGINE_TRADE_HPP

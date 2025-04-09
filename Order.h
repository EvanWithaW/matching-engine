//
// Created by Evan Weidner on 4/9/25.
//

#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <chrono>

enum class OrderSide {
    BUY,
    SELL
};

class Order {
private:
    std::string id;
    std::string symbol;
    OrderSide side;
    double price;
    int quantity;
    std::chrono::time_point<std::chrono::system_clock> timestamp;

public:
    // Constructor declaration
    Order(const std::string& id, const std::string& symbol, OrderSide side, double price, int quantity);

    // Other method declarations
    const std::string& getId() const;
    const std::string& getSymbol() const;
    OrderSide getSide() const;
    double getPrice() const;
    int getQuantity() const;
    const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;

    void setQuantity(int newQuantity);
    bool isBuy() const;
    bool isSell() const;
    std::string toString() const;
};

#endif // ORDER_H
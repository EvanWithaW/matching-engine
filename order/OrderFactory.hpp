//
// Created by Evan Weidner on 4/11/25.
//

#ifndef ORDERFACTORY_HPP
#define ORDERFACTORY_HPP

#include <memory>
#include "Order.hpp"


class OrderFactory {
public:
    static std::shared_ptr<Order> createLimitOrder(const std::string& symbol, OrderSide side, double price, int quantity, const std::string& callerId="");
    static std::shared_ptr<Order> createMarketOrder(const std::string& symbol, OrderSide side, int quantity, const std::string& callerId="");
private:
    static std::string generateOrderId();
    static int orderIdCounter;
    static bool validateOrderParameters(const std::string& symbol, OrderSide side, double price, int quantity);
};

#endif //ORDERFACTORY_HPP

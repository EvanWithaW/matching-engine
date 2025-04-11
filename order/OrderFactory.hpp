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
    // To generate a unique order id to categorize the order
    static std::string generateOrderId();
    // To use with the generateOrderId method for unique ids
    static int orderIdCounter;
    // I don't want to have to handle validating order parameters for all methods
    static bool validateOrderParameters(const std::string& symbol, OrderSide side, double price, int quantity);
};

#endif //ORDERFACTORY_HPP

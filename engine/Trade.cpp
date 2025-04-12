//
// Created by Evan Weidner on 4/11/25.
//

#include "Trade.hpp"
#include <sstream>

int Trade::tradeIdCounter = 0;

Trade::Trade(const std::string& id, 
             const std::string& symbol, 
             const std::string& buyOrderId, 
             const std::string& sellOrderId, 
             double price, 
             int quantity) 
    : id(id), 
      symbol(symbol), 
      buyOrderId(buyOrderId), 
      sellOrderId(sellOrderId), 
      price(price), 
      quantity(quantity),
      timestamp(std::chrono::system_clock::now()) {
}

const std::string& Trade::getId() const {
    return id;
}

const std::string& Trade::getSymbol() const {
    return symbol;
}

const std::string& Trade::getBuyOrderId() const {
    return buyOrderId;
}

const std::string& Trade::getSellOrderId() const {
    return sellOrderId;
}

double Trade::getPrice() const {
    return price;
}

int Trade::getQuantity() const {
    return quantity;
}

const std::chrono::time_point<std::chrono::system_clock>& Trade::getTimestamp() const {
    return timestamp;
}

std::shared_ptr<Trade> Trade::createTrade(
    std::shared_ptr<Order> buyOrder,
    std::shared_ptr<Order> sellOrder,
    double price,
    int quantity) {
    
    if (!buyOrder || !sellOrder || price <= 0 || quantity <= 0) {
        return nullptr;
    }
    
    if (buyOrder->getSymbol() != sellOrder->getSymbol()) {
        return nullptr;
    }
    
    return std::make_shared<Trade>(
        generateTradeId(),
        buyOrder->getSymbol(),
        buyOrder->getId(),
        sellOrder->getId(),
        price,
        quantity
    );
}

std::string Trade::generateTradeId() {
    return "T" + std::to_string(++tradeIdCounter);
}

std::string Trade::toString() const {
    std::stringstream ss;
    ss << "Trade{id=" << id
       << ", symbol=" << symbol
       << ", buyOrderId=" << buyOrderId
       << ", sellOrderId=" << sellOrderId
       << ", price=" << price
       << ", quantity=" << quantity
       << "}";
    return ss.str();
}

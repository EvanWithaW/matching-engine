//
// Created by Evan Weidner on 4/11/25.
//

#include "MatchingEngine.hpp"
#include "Trade.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

MatchingEngine::MatchingEngine() {
}

MatchingEngine::~MatchingEngine() {
    orderBooks.clear();
}

bool MatchingEngine::addSymbol(const std::string& symbol) {
    if (orderBooks.find(symbol) != orderBooks.end()) {
        return false;
    }
    
    orderBooks[symbol] = std::make_shared<OrderBook>(symbol);
    return true;
}

bool MatchingEngine::removeSymbol(const std::string& symbol) {
    auto it = orderBooks.find(symbol);
    if (it == orderBooks.end()) {
        return false;
    }
    
    orderBooks.erase(it);
    return true;
}

bool MatchingEngine::hasSymbol(const std::string& symbol) const {
    return orderBooks.find(symbol) != orderBooks.end();
}

std::vector<std::string> MatchingEngine::getSymbols() const {
    std::vector<std::string> symbols;
    symbols.reserve(orderBooks.size());
    
    for (const auto& [symbol, _] : orderBooks) {
        symbols.push_back(symbol);
    }
    
    return symbols;
}

std::shared_ptr<OrderBook> MatchingEngine::getOrderBook(const std::string& symbol) const {
    auto it = orderBooks.find(symbol);
    if (it == orderBooks.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::vector<std::shared_ptr<Trade>> MatchingEngine::processOrder(std::shared_ptr<Order> order) {
    if (!order) {
        return {};
    }
    
    const std::string& symbol = order->getSymbol();
    auto orderBook = getOrderBook(symbol);
    
    if (!orderBook) {
        if (!addSymbol(symbol)) {
            return {};
        }
        orderBook = getOrderBook(symbol);
    }
    
    std::vector<std::shared_ptr<Trade>> trades;
    
    if (order->getPrice() == 0.0) {
        if (!canMatchMarketOrder(order, orderBook)) {
            std::cerr << "Cannot match market order: " << order->toString() << std::endl;
            return {};
        }
    }
    
    if (order->isBuy()) {
        trades = matchBuyOrder(order, orderBook);
    } else {
        trades = matchSellOrder(order, orderBook);
    }
    
    if (order->getQuantity() > 0) {
        orderBook->addOrder(order);
    }
    
    return trades;
}

bool MatchingEngine::cancelOrder(const std::string& orderId, const std::string& symbol) {
    auto orderBook = getOrderBook(symbol);
    if (!orderBook) {
        return false;
    }
    
    return orderBook->cancelOrder(orderId);
}

double MatchingEngine::getBestBidPrice(const std::string& symbol) const {
    auto orderBook = getOrderBook(symbol);
    if (!orderBook) {
        return 0.0;
    }
    
    return orderBook->getBestBidPrice();
}

double MatchingEngine::getBestAskPrice(const std::string& symbol) const {
    auto orderBook = getOrderBook(symbol);
    if (!orderBook) {
        return 0.0;
    }
    
    return orderBook->getBestAskPrice();
}

int MatchingEngine::getBidSize(const std::string& symbol, double price) const {
    auto orderBook = getOrderBook(symbol);
    if (!orderBook) {
        return 0;
    }
    
    return orderBook->getBidSize(price);
}

int MatchingEngine::getAskSize(const std::string& symbol, double price) const {
    auto orderBook = getOrderBook(symbol);
    if (!orderBook) {
        return 0;
    }
    
    return orderBook->getAskSize(price);
}

std::string MatchingEngine::toString() const {
    std::stringstream ss;
    ss << "MatchingEngine{" << std::endl;
    
    for (const auto& [symbol, orderBook] : orderBooks) {
        ss << "  " << orderBook->toString() << std::endl;
    }
    
    ss << "}";
    return ss.str();
}

std::vector<std::shared_ptr<Trade>> MatchingEngine::matchBuyOrder(std::shared_ptr<Order> buyOrder, std::shared_ptr<OrderBook> orderBook) {
    std::vector<std::shared_ptr<Trade>> trades;
    
    auto sellOrders = orderBook->getAllSellOrders();
    
    for (auto& sellOrder : sellOrders) {
        if (buyOrder->getQuantity() <= 0) {
            break;
        }
        
        if (buyOrder->getPrice() == 0.0 || buyOrder->getPrice() >= sellOrder->getPrice()) {
            int matchQuantity = std::min(buyOrder->getQuantity(), sellOrder->getQuantity());
            
            double tradePrice = sellOrder->getPrice();
            auto trade = Trade::createTrade(buyOrder, sellOrder, tradePrice, matchQuantity);
            
            if (trade) {
                trades.push_back(trade);
                
                buyOrder->setQuantity(buyOrder->getQuantity() - matchQuantity);
                sellOrder->setQuantity(sellOrder->getQuantity() - matchQuantity);
                
                if (sellOrder->getQuantity() <= 0) {
                    orderBook->cancelOrder(sellOrder->getId());
                }
            }
        } else {
            break;
        }
    }
    
    return trades;
}

std::vector<std::shared_ptr<Trade>> MatchingEngine::matchSellOrder(std::shared_ptr<Order> sellOrder, std::shared_ptr<OrderBook> orderBook) {
    std::vector<std::shared_ptr<Trade>> trades;
    
    auto buyOrders = orderBook->getAllBuyOrders();
    
    for (auto& buyOrder : buyOrders) {
        if (sellOrder->getQuantity() <= 0) {
            break;
        }
        
        if (sellOrder->getPrice() == 0.0 || sellOrder->getPrice() <= buyOrder->getPrice()) {
            int matchQuantity = std::min(sellOrder->getQuantity(), buyOrder->getQuantity());
            
            double tradePrice = buyOrder->getPrice();
            auto trade = Trade::createTrade(buyOrder, sellOrder, tradePrice, matchQuantity);
            
            if (trade) {
                trades.push_back(trade);
                
                sellOrder->setQuantity(sellOrder->getQuantity() - matchQuantity);
                buyOrder->setQuantity(buyOrder->getQuantity() - matchQuantity);
                
                if (buyOrder->getQuantity() <= 0) {
                    orderBook->cancelOrder(buyOrder->getId());
                }
            }
        } else {
            break;
        }
    }
    
    return trades;
}

bool MatchingEngine::canMatchMarketOrder(std::shared_ptr<Order> order, std::shared_ptr<OrderBook> orderBook) const {
    if (order->isBuy()) {
        return orderBook->getBestAskPrice() > 0.0;
    } else {
        return orderBook->getBestBidPrice() > 0.0;
    }
}

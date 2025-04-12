//
// Created by Evan Weidner on 4/11/25.
//

#include "OrderBook.hpp"

bool BuyComparator::operator()(const ::std::shared_ptr<Order>& lhs, const ::std::shared_ptr<Order>& rhs) const {
    if (lhs->getPrice() == rhs->getPrice()) {
        return lhs->getTimestamp() < rhs->getTimestamp();
    } else {
        return lhs->getPrice() > rhs->getPrice();
    }
}

bool SellComparator::operator()(const ::std::shared_ptr<Order>& lhs, const ::std::shared_ptr<Order>& rhs) const {
    if (lhs->getPrice() == rhs->getPrice()) {
        return lhs->getTimestamp() < rhs->getTimestamp();
    } else {
        return lhs->getPrice() < rhs->getPrice();
    }
}

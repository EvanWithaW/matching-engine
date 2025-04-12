# Multithreaded Matching Engine

A high-performance, multi-threaded matching engine for processing financial orders.

## Features

- [x] Continuous matching of buy and sell orders
- [x] Support for limit and market orders
- [x] Multi-threaded processing with symbol-based thread assignment
- [x] Order book management for each symbol
- [x] Trade generation and reporting
- [x] Order cancellation
- [x] Callback system for trade and order processing notifications
- [ ] Order persistence to disk (load trades from a file to simulate)

## Getting Started

### Build

```bash
# Build the project
./build.sh
```

### Run

```bash
# Run the main program
./build/matching_engine

# Run the tests
./build/tests/unit_tests
```

## Usage Example

```cpp
// Initialize the engine
auto engine = std::make_unique<ContinuousMatchingEngine>(4); // 4 threads
engine->start();

// Add symbols
engine->addSymbol("AAPL");
engine->addSymbol("MSFT");

// Register callbacks
engine->registerTradeCallback([](std::shared_ptr<Trade> trade) {
    std::cout << "Trade executed: " << trade->toString() << std::endl;
});

// Submit orders
auto buyOrder = std::make_shared<Order>("order1", "AAPL", OrderSide::BUY, 100, 150.0);
engine->submitOrder(buyOrder);

auto sellOrder = std::make_shared<Order>("order2", "AAPL", OrderSide::SELL, 100, 150.0);
engine->submitOrder(sellOrder);

// Cancel an order
engine->cancelOrder("order1", "AAPL");

// Shutdown
engine->stop();
```

## Performance

The matching engine uses a thread pool with symbol-based sharding to achieve high throughput:

- Orders for the same symbol are processed sequentially (integrity of orderbook is preserved)
- Orders for different symbols are processed in parallel
- Thread assignment is consistent to prevent race conditions

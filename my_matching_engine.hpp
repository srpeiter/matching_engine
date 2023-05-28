#pragma once

#include <memory>

#include "matching_engine.hpp"
#include "types.hpp"
#include <cstddef>
#include <ctime>
#include <map>

struct OrderInfo {
    uint64_t id;
    std::string symbol;
    Side side;
    int64_t price = 0;
    int64_t volume = 0;
    time_t timestamp;

    bool operator==(const OrderInfo &order2) { return id == order2.id; }
};

struct OrderBook {
    std::vector<OrderInfo> buySide;
    std::vector<OrderInfo> sellSide;
};

std::unique_ptr<MatchingEngine> create_matching_engine();

class MyMatchingEngine : public MatchingEngine {
  public:
    ~MyMatchingEngine() override;
    MyMatchingEngine();

    // We delete the copy/move operator to be explicit
    MyMatchingEngine(MyMatchingEngine const &rhs) = delete;
    MyMatchingEngine &operator=(MyMatchingEngine const &rhs) = delete;

    MyMatchingEngine(MyMatchingEngine &&rhs) = delete;
    MyMatchingEngine &operator=(MyMatchingEngine &&rhs) = delete;

    bool add_order(uint64_t order_id, std::string const &symbol, Side side,
                   int64_t price, int64_t volume,
                   std::vector<Fill> &fills) override;

    bool amend_order(uint64_t order_id, int64_t new_price,
                     int64_t new_active_volume,
                     std::vector<Fill> &fills) override;

    bool pull_order(uint64_t order_id) override;

    BestBidOffer get_top_of_book(std::string const &symbol) const override;

  private:
    void sortBook(std::vector<OrderInfo> &bookSide);

    int64_t getBestAskPrice(const std::string &symbol) const;
    int64_t getBestBidPrice(const std::string &symbol) const;
    int64_t getBestAskVolume(const std::string &symbol) const;
    int64_t getBestBidVolume(const std::string &symbol) const;
    OrderBook *getOrderByBook(uint64_t order_id);
    OrderInfo *getOrderById(uint64_t order_id, OrderBook *book);
    OrderInfo *getOrder(uint64_t order_id);
    int getOrderLocInBook(uint64_t, Side side, OrderBook *book,
                          OrderInfo *orderInfo);
    bool matchOrder(OrderInfo &newOrder, OrderBook &book,
                    std::vector<Fill> &fills);

    std::map<std::string, OrderBook> symbolBook;
};

bool sortPriceTimePriority(const OrderInfo &order1, const OrderInfo &order2);

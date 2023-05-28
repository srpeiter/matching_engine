#pragma once

#include "types.hpp"

#include <stdint.h>
#include <string>
#include <vector>

// MatchingEngine maintains a central limit orderbook for multiple symbols.
// It allows you to modify the orderbooks by adding, amending and pulling
// orders. Any trades that occur because of these actions are written into the
// vector of fills. Order ids are unique, you cannot have 2 orders with the same
// ID at the same time.
class MatchingEngine {
  public:
    virtual ~MatchingEngine() = default;
    MatchingEngine() = default;

    // Add order adds a new order to the orderbook of the given symbol.
    // All fills will be written into the given vector.
    // If there is volume leftover the volume should be added to the orderbook.
    //
    // The function returns false if
    // - the order_id is 0
    // - the order_id already exists
    // - the volume is 0 or negative
    // - the price is 0 or negative
    // - the symbol is empty
    //
    // Fills contains all trades that occur because of this add
    virtual bool add_order(uint64_t order_id, std::string const &symbol,
                           Side side, int64_t price, int64_t volume,
                           std::vector<Fill> &fills) = 0;

    // Amends an existing order to a new volume and price.
    // All fills will be written in the given vector.
    // If there is volume leftover the volume should be added to the orderbook.
    //
    // NOTE: amends that go down or stay the same in volume (e.g. 5 to 3) and
    // are the same price do not lose time priority. All other amends should
    // lose time priority.
    //
    // The function returns false if
    // - the order_id does not exist
    // - the new_volume is 0 or negative
    // - the new_price is 0 or negative
    //
    // Fills contains all trades that occur because of this amend
    virtual bool amend_order(uint64_t order_id, int64_t new_price,
                             int64_t new_active_volume,
                             std::vector<Fill> &fills) = 0;

    // Pulls an existing order such that it is deleted from the orderbook
    //
    // The function returns false if
    // - the order_id does not exist
    virtual bool pull_order(uint64_t order_id) = 0;

    // Returns the top of the book
    // In case there are no orders for a side, a volume of 0 should be returned
    // for that side
    virtual BestBidOffer get_top_of_book(std::string const &symbol) const = 0;
};

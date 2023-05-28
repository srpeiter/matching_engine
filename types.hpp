#pragma once

#include <stdint.h>
#include <string>
#include <vector>

enum class Side {
    Buy,
    Sell,
};

struct Fill {
    uint64_t other_order_id = 0;
    int64_t trade_price = 0;
    int64_t trade_volume = 0;
};

struct BestBidOffer {
    int64_t bid_volume = 0;
    int64_t bid_price = 0;

    int64_t ask_volume = 0;
    int64_t ask_price = 0;
};



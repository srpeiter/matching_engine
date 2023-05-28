#include "my_matching_engine.hpp"
#include <ctime>
#include <iostream>

using namespace std;

int main() {

    auto matching_engine = create_matching_engine();
    auto fills = std::vector<Fill>();
    
    // string symbol = "webb";

    // auto bbo = matching_engine->get_top_of_book("webb");
    // matching_engine->add_order(1, "webb", Side::Sell, 118, 5, fills);
    // matching_engine->add_order(4, "webb", Side::Sell, 119, 1, fills);
    // matching_engine->add_order(4, "webb", Side::Sell, 119, 1, fills);
    // matching_engine->add_order(2, "webb", Side::Sell, 125, 5, fills);
    // matching_engine->add_order(5, "webb", Side::Sell, 125, 5, fills);

    // bbo = matching_engine->get_top_of_book("webb");

    // matching_engine->add_order(6, "webb", Side::Sell, 128, 5, fills);
    // matching_engine->add_order(7, "webb", Side::Sell, 129, 5, fills);
    // matching_engine->add_order(14, "webb", Side::Buy, 116, 5, fills);
    // matching_engine->add_order(13, "webb", Side::Buy, 122, 8, fills);
    // matching_engine->add_order(15, "webb", Side::Buy, 122, 8, fills);
    // matching_engine->add_order(33, "webb", Side::Buy, 125, 8, fills);
    // matching_engine->add_order(23, "webb", Side::Buy, 124, 8, fills);
    // matching_engine->add_order(12, "appl", Side::Sell, 118, 5, fills);
    // matching_engine->amend_order(23, 124, 2, fills);
    // matching_engine->pull_order(23);

    // bbo = matching_engine->get_top_of_book("webb");

    // matching_engine->add_order(4, "webb", Side::Sell, 122, 2, fills);

    auto symbol_a = std::string("webb");
    auto symbol_b = std::string("trading");
    auto symbol_c = std::string("challenge");

    // auto fills = std::vector<Fill>();

    // auto matching_engine = create_matching_engine();
    // REQUIRE(matching_engine != nullptr);

    bool success =
        matching_engine->add_order(10, symbol_a, Side::Buy, 120, 5, fills);
    // REQUIRE(success);
    // REQUIRE(fills.size() == 0);

    auto bbo = matching_engine->get_top_of_book(symbol_a);
    // REQUIRE(bbo.bid_volume == 5);
    // REQUIRE(bbo.bid_price == 120);
    // REQUIRE(bbo.ask_volume == 0);

    success =
        matching_engine->add_order(5, symbol_b, Side::Sell, 24, 31, fills);
    // REQUIRE(success);
    // REQUIRE(fills.size() == 0);

    success =
        matching_engine->add_order(7, symbol_b, Side::Sell, 23, 47, fills);
    // REQUIRE(success);
    // REQUIRE(fills.size() == 0);

    bbo = matching_engine->get_top_of_book(symbol_b);
    // REQUIRE(bbo.bid_volume == 0);
    // REQUIRE(bbo.ask_volume == 47);
    // REQUIRE(bbo.ask_price == 23);

    // Order id already used in symbol_a
    success =
        matching_engine->add_order(16, symbol_c, Side::Sell, 45, 8, fills);
    // REQUIRE(!success);
    // REQUIRE(fills.size() == 0);

    // Match order 10
    success =
        matching_engine->add_order(11, symbol_a, Side::Sell, 120, 5, fills);
    // REQUIRE(success);
    // REQUIRE(fills.size() == 1);

    // REQUIRE(fills[0].other_order_id == 10);
    // REQUIRE(fills[0].trade_price == 120);
    // REQUIRE(fills[0].trade_volume == 5);
    fills.clear();

    bbo = matching_engine->get_top_of_book(symbol_a);
    // REQUIRE(bbo.bid_volume == 0);
    // REQUIRE(bbo.ask_volume == 0);

    success =
        matching_engine->add_order(13, symbol_a, Side::Buy, 120, 5, fills);
    // REQUIRE(success);
    // REQUIRE(fills.size() == 0);

    bbo = matching_engine->get_top_of_book(symbol_a);
    //     REQUIRE(bbo.bid_volume == 5);
    //     REQUIRE(bbo.bid_price == 120);
    //     REQUIRE(bbo.ask_volume == 0);
}

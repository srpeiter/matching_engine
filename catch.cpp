
#include "../../../../../opt/local/include/catch2/catch_all.hpp"
#include "my_matching_engine.hpp"

TEST_CASE("add and pull") {
    auto symbol = std::string("webb");
    auto fills = std::vector<Fill>();

    auto matching_engine = create_matching_engine();
    REQUIRE(matching_engine != nullptr);

    auto bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 0);
    REQUIRE(bbo.ask_volume == 0);

    bool success =
        matching_engine->add_order(1, symbol, Side::Buy, 120, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 5);
    REQUIRE(bbo.bid_price == 120);
    REQUIRE(bbo.ask_volume == 0);

    success = matching_engine->pull_order(1);
    REQUIRE(success);

    bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 0);
    REQUIRE(bbo.ask_volume == 0);
}

TEST_CASE("simple match") {
    auto symbol = std::string("webb");
    auto fills = std::vector<Fill>();

    auto matching_engine = create_matching_engine();
    REQUIRE(matching_engine != nullptr);

    bool success =
        matching_engine->add_order(1, symbol, Side::Buy, 120, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(2, symbol, Side::Sell, 119, 8, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 1);

    REQUIRE(fills[0].other_order_id == 1);
    REQUIRE(fills[0].trade_price == 120);
    REQUIRE(fills[0].trade_volume == 5);
    fills.clear();

    auto bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 0);
    REQUIRE(bbo.ask_volume == 3);
    REQUIRE(bbo.ask_price == 119);
}

TEST_CASE("multi insert and multi match") {
    auto symbol = std::string("webb");
    auto fills = std::vector<Fill>();

    auto matching_engine = create_matching_engine();
    REQUIRE(matching_engine != nullptr);

    bool success =
        matching_engine->add_order(1, symbol, Side::Buy, 120, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(2, symbol, Side::Buy, 120, 6, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(3, symbol, Side::Buy, 120, 12, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(4, symbol, Side::Buy, 119, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(5, symbol, Side::Buy, 118, 3, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(6, symbol, Side::Sell, 121, 8, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(7, symbol, Side::Sell, 122, 9, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->pull_order(1);
    REQUIRE(success);

    auto bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 18);
    REQUIRE(bbo.bid_price == 120);
    REQUIRE(bbo.ask_volume == 8);
    REQUIRE(bbo.ask_price == 121);

    success = matching_engine->add_order(8, symbol, Side::Sell, 119, 25, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 3);

    REQUIRE(fills[0].other_order_id == 2);
    REQUIRE(fills[0].trade_price == 120);
    REQUIRE(fills[0].trade_volume == 6);

    REQUIRE(fills[1].other_order_id == 3);
    REQUIRE(fills[1].trade_price == 120);
    REQUIRE(fills[1].trade_volume == 12);

    REQUIRE(fills[2].other_order_id == 4);
    REQUIRE(fills[2].trade_price == 119);
    REQUIRE(fills[2].trade_volume == 5);
    fills.clear();

    bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 3);
    REQUIRE(bbo.bid_price == 118);
    REQUIRE(bbo.ask_volume == 2);
    REQUIRE(bbo.ask_price == 119);
}

TEST_CASE("multi symbol") {
    auto symbol_a = std::string("webb");
    auto symbol_b = std::string("trading");
    auto symbol_c = std::string("challenge");

    auto fills = std::vector<Fill>();

    auto matching_engine = create_matching_engine();
    REQUIRE(matching_engine != nullptr);

    bool success =
        matching_engine->add_order(10, symbol_a, Side::Buy, 120, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    auto bbo = matching_engine->get_top_of_book(symbol_a);
    REQUIRE(bbo.bid_volume == 5);
    REQUIRE(bbo.bid_price == 120);
    REQUIRE(bbo.ask_volume == 0);

    success =
        matching_engine->add_order(5, symbol_b, Side::Sell, 24, 31, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success =
        matching_engine->add_order(7, symbol_b, Side::Sell, 23, 47, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    bbo = matching_engine->get_top_of_book(symbol_b);
    REQUIRE(bbo.bid_volume == 0);
    REQUIRE(bbo.ask_volume == 47);
    REQUIRE(bbo.ask_price == 23);

    // Order id already used in symbol_a
    success =
        matching_engine->add_order(10, symbol_c, Side::Sell, 45, 8, fills);
    REQUIRE(!success);
    REQUIRE(fills.size() == 0);

    // Match order 10
    success =
        matching_engine->add_order(11, symbol_a, Side::Sell, 120, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 1);

    REQUIRE(fills[0].other_order_id == 10);
    REQUIRE(fills[0].trade_price == 120);
    REQUIRE(fills[0].trade_volume == 5);
    fills.clear();

    bbo = matching_engine->get_top_of_book(symbol_a);
    REQUIRE(bbo.bid_volume == 0);
    REQUIRE(bbo.ask_volume == 0);

    success =
        matching_engine->add_order(13, symbol_a, Side::Buy, 120, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    bbo = matching_engine->get_top_of_book(symbol_a);
    REQUIRE(bbo.bid_volume == 5);
    REQUIRE(bbo.bid_price == 120);
    REQUIRE(bbo.ask_volume == 0);
}

TEST_CASE("amend") {
    auto symbol = std::string("webb");
    auto fills = std::vector<Fill>();

    auto matching_engine = create_matching_engine();
    REQUIRE(matching_engine != nullptr);

    bool success =
        matching_engine->add_order(1, symbol, Side::Buy, 120, 5, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(2, symbol, Side::Buy, 120, 6, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(3, symbol, Side::Buy, 120, 12, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    success = matching_engine->add_order(4, symbol, Side::Sell, 121, 8, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    auto bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 23);
    REQUIRE(bbo.bid_price == 120);
    REQUIRE(bbo.ask_volume == 8);
    REQUIRE(bbo.ask_price == 121);

    success = matching_engine->amend_order(2, 121, 3, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 1);

    REQUIRE(fills[0].other_order_id == 4);
    REQUIRE(fills[0].trade_price == 121);
    REQUIRE(fills[0].trade_volume == 3);
    fills.clear();

    bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 17);
    REQUIRE(bbo.bid_price == 120);
    REQUIRE(bbo.ask_volume == 5);
    REQUIRE(bbo.ask_price == 121);

    // This shouldn't lose time priority
    success = matching_engine->amend_order(1, 120, 3, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    // So this needs to match against order 1
    success = matching_engine->add_order(5, symbol, Side::Sell, 120, 1, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 1);

    REQUIRE(fills[0].other_order_id == 1);
    REQUIRE(fills[0].trade_price == 120);
    REQUIRE(fills[0].trade_volume == 1);
    fills.clear();

    // This should lose time priority (since the active volume of the order is
    // 2)
    success = matching_engine->amend_order(1, 120, 3, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 0);

    // So this needs to match against order 3 (order 2 got amended to fill
    // before)
    success = matching_engine->add_order(6, symbol, Side::Sell, 120, 1, fills);
    REQUIRE(success);
    REQUIRE(fills.size() == 1);

    REQUIRE(fills[0].other_order_id == 3);
    REQUIRE(fills[0].trade_price == 120);
    REQUIRE(fills[0].trade_volume == 1);
    fills.clear();

    bbo = matching_engine->get_top_of_book(symbol);
    REQUIRE(bbo.bid_volume == 14);
    REQUIRE(bbo.bid_price == 120);
    REQUIRE(bbo.ask_volume == 5);
    REQUIRE(bbo.ask_price == 121);
}

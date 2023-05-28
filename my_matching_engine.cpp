#include "my_matching_engine.hpp"
#include <vector>

std::unique_ptr<MatchingEngine> create_matching_engine() {
    return std::make_unique<MyMatchingEngine>();
}

bool sortPriceTimePriority(const OrderInfo &order1, const OrderInfo &order2) {
    if (order1.price == order2.price) {
        if (order1.side == Side::Sell)
            return (order1.timestamp < order2.timestamp);
    }
    if (order1.side == Side::Sell) // ascending sort
        return (order1.price < order2.price);
    else // descending sort
        return (order1.price > order2.price);
}

MyMatchingEngine::~MyMatchingEngine() {}

MyMatchingEngine::MyMatchingEngine() {}

int64_t MyMatchingEngine::getBestAskPrice(const std::string &symbol) const {

    if (!symbolBook.at(symbol).sellSide.size())
        return 0;

    return symbolBook.at(symbol).sellSide[0].price;
}

int64_t MyMatchingEngine::getBestAskVolume(const std::string &symbol) const {

    std::vector<OrderInfo> book = symbolBook.at(symbol).sellSide;
    if (!book.size())
        return 0;

    if (book.size() == 1)
        return book[0].volume;

    int64_t totalVolume = book[0].volume;
    for (int i = 1; i < book.size(); i++) {
        if (book[i].price == book[0].price)
            totalVolume += book[i].volume;

        else
            return totalVolume;
    }

    return totalVolume;
}

int64_t MyMatchingEngine::getBestBidPrice(const std::string &symbol) const {

    if (!symbolBook.at(symbol).buySide.size())
        return 0;

    return symbolBook.at(symbol).buySide[0].price;
}

int64_t MyMatchingEngine::getBestBidVolume(const std::string &symbol) const {

    std::vector<OrderInfo> book = symbolBook.at(symbol).buySide;
    if (!book.size())
        return 0;

    if (book.size() == 1)
        return book[0].volume;

    int64_t totalVolume = book[0].volume;
    for (int i = 1; i < book.size(); i++) {
        if (book[i].price == book[0].price)
            totalVolume += book[i].volume;

        else
            return totalVolume;
    }

    return totalVolume;
}

OrderBook *MyMatchingEngine::getOrderByBook(uint64_t order_id) {

    OrderBook *bookdefault = nullptr;

    for (auto &[key, book] : symbolBook) {
        for (auto &side : book.buySide) {
            if (side.id == order_id) {
                return &book;
            }
        }

        for (auto &side : book.sellSide) {
            if (side.id == order_id) {
                return &book;
            }
        }
    }

    return bookdefault;
}

OrderInfo *MyMatchingEngine::getOrderById(uint64_t order_id, OrderBook *book) {
    OrderInfo *defaultOrder = nullptr;

    if (book == nullptr)
        return defaultOrder;

    for (auto &order : book->buySide) {
        if (order.id == order_id) {
            return &order;
        }
    }
    for (auto &order : book->sellSide) {
        if (order.id == order_id) {
            return &order;
        }
    }

    return defaultOrder;
}

OrderInfo *MyMatchingEngine::getOrder(uint64_t order_id) {
    auto book_ptr = getOrderByBook(order_id);

    auto order_ptr = getOrderById(order_id, book_ptr);

    return order_ptr;
}

int MyMatchingEngine::getOrderLocInBook(uint64_t, Side side, OrderBook *book,
                                        OrderInfo *orderInfo) {

    std::vector<OrderInfo>::iterator itr;
    int loc = 0;

    if (side == Side::Buy) {
        itr = std::find(book->buySide.begin(), book->buySide.end(), *orderInfo);
        if (itr != book->buySide.cend())
            loc = std::distance(book->buySide.begin(), itr);
        return loc;
    }

    else if (side == Side::Sell) {
        itr =
            std::find(book->sellSide.begin(), book->sellSide.end(), *orderInfo);

        if (itr != book->sellSide.cend())
            loc = std::distance(book->sellSide.begin(), itr);
        return loc;
    }

    return loc;
}

void MyMatchingEngine::sortBook(std::vector<OrderInfo> &bookSide) {
    std::sort(bookSide.begin(), bookSide.end(), sortPriceTimePriority);
}

bool MyMatchingEngine::add_order(uint64_t order_id, std::string const &symbol,
                                 Side side, int64_t price, int64_t volume,
                                 std::vector<Fill> &fills) {

    if (getOrder(order_id))
        return false;

    if (order_id == 0)
        return false;

    if (symbol.empty())
        return false;
    if (price <= 0)
        return false;
    if (volume <= 0)
        return false;

    time_t timestamp = time(nullptr);

    OrderInfo orderReceipt = {order_id, symbol, side, price, volume, timestamp};
    // initialize if symbol does not exist
    if (!symbolBook.count(symbol)) {
        std::vector<OrderInfo> buySide;
        std::vector<OrderInfo> sellSide;

        if (side == Side::Buy)
            buySide.push_back(orderReceipt);
        else
            sellSide.push_back(orderReceipt);

        symbolBook.insert(
            std::pair<std::string, OrderBook>(symbol, {buySide, sellSide}));

        return true;
    }

    auto &book = symbolBook.at(symbol);

    // check if books are empty
    if (side == Side::Sell && book.buySide.empty()) {
        book.sellSide.push_back(orderReceipt);
        sortBook(book.sellSide);
        return true;
    }

    if (side == Side::Buy && book.sellSide.empty()) {
        book.buySide.push_back(orderReceipt);
        sortBook(book.buySide);
        return true;
    }

    // match orders
    return matchOrder(orderReceipt, book, fills);
}

bool MyMatchingEngine::matchOrder(OrderInfo &newOrder, OrderBook &book,
                                  std::vector<Fill> &fills) {
    /*
    Matching algorithm
    */

    if (newOrder.side == Side::Buy) {

        // check if price crosses book

        if (book.sellSide[0].price > newOrder.price) {
            book.buySide.push_back(newOrder);
            sortBook(book.buySide);
            return true;
        }

        // continue to match and add to fill vector;

        int remainingVolume = 0;
        int count = 0;
        for (OrderInfo &order : book.sellSide) { // match order conditions
            if (newOrder.price >= order.price) {
                remainingVolume = newOrder.volume - order.volume;
                if (remainingVolume > 0) {
                    fills.push_back({order.id, order.price, order.volume});
                    newOrder.volume -= order.volume;
                    count++;
                    continue;
                } else if (remainingVolume == 0) {
                    fills.push_back({order.id, order.price, order.volume});
                    newOrder.volume -= order.volume;
                    count++;
                    break;
                } else {
                    fills.push_back({order.id, order.price, newOrder.volume});
                    order.volume -= newOrder.volume;
                    newOrder.volume = 0;
                    break;
                }
            }
        }

        if (newOrder.volume > 0) {
            // place remaining volume back in book
            newOrder.volume = remainingVolume;
            book.buySide.push_back(newOrder);
            sortBook(book.buySide);
        }

        // remove matched orders
        if (count) {
            book.sellSide.erase(book.sellSide.begin(),
                                book.sellSide.begin() + count);
            return true;
        }

    }

    else if (newOrder.side == Side::Sell) {

        // check if price crosses book

        if (book.buySide[0].price < newOrder.price) {
            book.sellSide.push_back(newOrder);
            sortBook(book.sellSide);
            return true;
        }

        // continue to match and add to fill vector;

        int remainingVolume = 0;
        int count = 0;
        for (OrderInfo &order : book.buySide) {
            if (newOrder.price <= order.price) {
                remainingVolume = newOrder.volume - order.volume;
                if (remainingVolume > 0) {
                    fills.push_back({order.id, order.price, order.volume});
                    newOrder.volume -= order.volume;
                    count++;
                    continue;
                } else if (remainingVolume == 0) {
                    fills.push_back({order.id, order.price, order.volume});
                    newOrder.volume -= order.volume;
                    count++;
                    break;
                } else {
                    fills.push_back({order.id, order.price, newOrder.volume});
                    order.volume -= newOrder.volume;
                    newOrder.volume = 0;
                    break;
                }
            }
        }

        if (newOrder.volume > 0) {
            // place remaining volume back in book
            newOrder.volume = remainingVolume;
            book.sellSide.push_back(newOrder);
            sortBook(book.sellSide);
        }

        // update book
        if (count) {
            book.buySide.erase(book.buySide.begin(),
                               book.buySide.begin() + count);

            return true;
        }
    }
    return true;
}

bool MyMatchingEngine::amend_order(uint64_t order_id, int64_t new_price,
                                   int64_t new_active_volume,
                                   std::vector<Fill> &fills) {

    // get orders from Map
    auto book_ptr = getOrderByBook(order_id);

    auto order_ptr = getOrderById(order_id, book_ptr);

    if (!book_ptr)
        return false;
    if (new_active_volume <= 0)
        return false;
    if (new_price <= 0)
        return false;

    std::string symbol = order_ptr->symbol;
    Side side = order_ptr->side;

    if (new_price != order_ptr->price ||
        new_active_volume > order_ptr->volume) {
        // pull order from the book and match order again
        pull_order(order_id);
        add_order(order_id, symbol, side, new_price, new_active_volume, fills);
        return true;
    }

    // update order
    if (new_active_volume <= order_ptr->volume) {
        order_ptr->volume = new_active_volume;
        return true;
    }

    return false;
}

bool MyMatchingEngine::pull_order(uint64_t order_id) {

    auto book_ptr = getOrderByBook(order_id);

    auto order_ptr = getOrderById(order_id, book_ptr);

    if (!order_ptr)
        return false;

    int index =
        getOrderLocInBook(order_id, order_ptr->side, book_ptr, order_ptr);

    if (order_ptr->side == Side::Buy) {
        book_ptr->buySide.erase(book_ptr->buySide.begin() + index);
        return true;
    }

    if (order_ptr->side == Side::Sell) {

        book_ptr->sellSide.erase(book_ptr->sellSide.begin() + index);
        return true;
    }

    return false;
}

BestBidOffer
MyMatchingEngine::get_top_of_book(std::string const &symbol) const {

    if (!symbolBook.count(symbol))
        return BestBidOffer();

    BestBidOffer bestOffers;

    bestOffers.ask_price = getBestAskPrice(symbol);
    bestOffers.ask_volume = getBestAskVolume(symbol);

    bestOffers.bid_price = getBestBidPrice(symbol);
    bestOffers.bid_volume = getBestBidVolume(symbol);

    return bestOffers;
}

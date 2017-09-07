/*!
    \file order_book.inl
    \brief Order book inline implementation
    \author Ivan Shynkarenka
    \date 02.08.2017
    \copyright MIT License
*/

namespace CppTrader {
namespace Matching {

inline OrderBook::OrderBook(const Symbol& symbol)
    : _symbol(symbol),
      _auxiliary_memory_manager(),
      _level_memory_manager(_auxiliary_memory_manager, 1024),
      _level_pool(_level_memory_manager),
      _best_bid(nullptr),
      _best_ask(nullptr),
      _best_buy_stop(nullptr),
      _best_sell_stop(nullptr),
      _best_trailing_buy_stop(nullptr),
      _best_trailing_sell_stop(nullptr),
      _last_bid_price(0),
      _last_ask_price(std::numeric_limits<uint64_t>::max()),
      _trailing_bid_price(0),
      _trailing_ask_price(std::numeric_limits<uint64_t>::max())
{
}

inline std::ostream& operator<<(std::ostream& stream, const OrderBook& order_book)
{
    return stream << "OrderBook(Symbol=" << order_book._symbol
        << "; Bids=" << order_book._bids.size()
        << "; Asks=" << order_book._asks.size()
        << "; BuyStop=" << order_book._buy_stop.size()
        << "; SellStop=" << order_book._sell_stop.size()
        << "; TrailingBuyStop=" << order_book._trailing_buy_stop.size()
        << "; TrailingSellStop=" << order_book._trailing_sell_stop.size()
        << ")";
}

inline const LevelNode* OrderBook::GetBid(uint64_t price) const noexcept
{
    auto it = _bids.find(LevelNode(LevelType::BID, price));
    return (it != _bids.end()) ? it.operator->() : nullptr;
}

inline const LevelNode* OrderBook::GetAsk(uint64_t price) const noexcept
{
    auto it = _asks.find(LevelNode(LevelType::ASK, price));
    return (it != _asks.end()) ? it.operator->() : nullptr;
}

inline const LevelNode* OrderBook::GetBuyStopLevel(uint64_t price) const noexcept
{
    auto it = _buy_stop.find(LevelNode(LevelType::ASK, price));
    return (it != _buy_stop.end()) ? it.operator->() : nullptr;
}

inline const LevelNode* OrderBook::GetSellStopLevel(uint64_t price) const noexcept
{
    auto it = _sell_stop.find(LevelNode(LevelType::BID, price));
    return (it != _sell_stop.end()) ? it.operator->() : nullptr;
}

inline const LevelNode* OrderBook::GetTrailingBuyStopLevel(uint64_t price) const noexcept
{
    auto it = _trailing_buy_stop.find(LevelNode(LevelType::ASK, price));
    return (it != _trailing_buy_stop.end()) ? it.operator->() : nullptr;
}

inline const LevelNode* OrderBook::GetTrailingSellStopLevel(uint64_t price) const noexcept
{
    auto it = _trailing_sell_stop.find(LevelNode(LevelType::BID, price));
    return (it != _trailing_sell_stop.end()) ? it.operator->() : nullptr;
}

inline LevelNode* OrderBook::GetNextLevel(LevelNode* level) noexcept
{
    if (level->IsBid())
    {
        Levels::reverse_iterator it(&_bids, level);
        ++it;
        return it.operator->();
    }
    else
    {
        Levels::iterator it(&_asks, level);
        ++it;
        return it.operator->();
    }
}

inline LevelNode* OrderBook::GetNextStopLevel(LevelNode* level) noexcept
{
    if (level->IsBid())
    {
        Levels::reverse_iterator it(&_sell_stop, level);
        ++it;
        return it.operator->();
    }
    else
    {
        Levels::iterator it(&_buy_stop, level);
        ++it;
        return it.operator->();
    }
}

inline LevelNode* OrderBook::GetNextTrailingStopLevel(LevelNode* level) noexcept
{
    if (level->IsBid())
    {
        Levels::reverse_iterator it(&_trailing_sell_stop, level);
        ++it;
        return it.operator->();
    }
    else
    {
        Levels::iterator it(&_trailing_buy_stop, level);
        ++it;
        return it.operator->();
    }
}

inline uint64_t OrderBook::GetMarketPriceBid() const noexcept
{
    uint64_t last_price = _last_bid_price;
    uint64_t best_price = (_best_bid != nullptr) ? _best_bid->Price : 0;
    return std::max(last_price, best_price);
}

inline uint64_t OrderBook::GetMarketPriceAsk() const noexcept
{
    uint64_t last_price = _last_ask_price;
    uint64_t best_price = (_best_ask != nullptr) ? _best_ask->Price : std::numeric_limits<uint64_t>::max();
    return std::min(last_price, best_price);
}

inline void OrderBook::UpdateLastPrice(const Order& order) noexcept
{
    if (order.IsBuy())
        _last_bid_price = order.Price;
    else
        _last_ask_price = order.Price;
}

} // namespace Matching
} // namespace CppTrader

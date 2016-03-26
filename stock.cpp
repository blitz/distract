#include "stock.hpp"

StockPriceFetcher::StockPriceFetcher(boost::asio::io_service &io, std::string name_,
                                     std::function<void(std::string const &, std::string const &)> callback)
  : simple_http(io, "download.finance.yahoo.com", "/d/quotes.csv?s=" + name_ + "&f=l1",
                [name_, callback] (std::string const &body) -> bool {
                  callback(name_, body);
                  return false;
                })
{ }

// EOF


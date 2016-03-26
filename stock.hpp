#pragma once

#include "simple_http.hpp"

class StockPriceFetcher : public simple_http {
public:
  StockPriceFetcher(boost::asio::io_service &io, std::string name_,
                    std::function<void(std::string const &, std::string const &)> callback);
};

// EOF

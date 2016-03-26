#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <sstream>

#include "safe_cb.hpp"

/// A very simple (braindead) HTTP client.
class simple_http : public std::enable_shared_from_this<simple_http> {
  using tcp         = boost::asio::ip::tcp;
  using callback_fn = std::function<bool(std::string const &)>;
  using socket      = boost::asio::ip::tcp::socket;
  using resolver    = boost::asio::ip::tcp::resolver;
  using error_code  = boost::system::error_code;

  using boost_write_cb   = safe_cb<simple_http, void(error_code, size_t)>;
  using boost_read_cb    = safe_cb<simple_http, void(error_code, size_t)>;
  using boost_connect_cb = safe_cb<simple_http, void(error_code, resolver::iterator)>;
  using boost_resolve_cb = safe_cb<simple_http, void(error_code, resolver::iterator)>;

  callback_fn body_cb;
  std::string host;
  std::string body;

  boost::asio::streambuf response;

  socket      sock;
  resolver    rslv;

  bool reading_body = false;

  void do_send()
  {
    auto cb = boost_write_cb(shared_from_this(), [this] (boost::system::error_code ec, size_t) {
        if (not ec) {
          do_read();
        }
      });

    boost::asio::async_write(sock, boost::asio::buffer(body), std::move(cb));
  }

  void do_read()
  {
    auto cb = boost_read_cb(shared_from_this(), [this] (boost::system::error_code ec, size_t len) {
        bool cont = true;
        if (len) {
          std::istream is(&response);
          std::string line;
          std::getline(is, line);

          if (reading_body) {
            cont = body_cb(line);
          } else {
            reading_body = (line.size() == 0) or (line.size() == 1 and line[0] == '\r');
          }
        }

        if (not ec and cont) {
          do_read();
        }
      });

    boost::asio::async_read_until(sock, response, '\n', std::move(cb));
  }

  void do_connect(resolver::iterator const &it)
  {
    auto cb = boost_connect_cb(shared_from_this(), [this] (boost::system::error_code ec,
                                                           tcp::resolver::iterator)
                               {
                                 if (not ec) {
                                   do_send();
                                 }
                               });

    boost::asio::async_connect(sock, it, std::move(cb));
  }

public:

  void start()
  {
    auto cb = boost_resolve_cb(shared_from_this(), [this] (boost::system::error_code ec, resolver::iterator it) {
        if (not ec) {
          do_connect(it);
        }
      });

    rslv.async_resolve({host, "http"}, std::move(cb));
  }

  simple_http(boost::asio::io_service &io, std::string const &host_, std::string const &url_,
              callback_fn const &callback)
    : body_cb(callback), sock(io), rslv(io), host(host_),
      body("GET " + url_ + " HTTP/1.1\r\nHost: " + host_ + "\r\n\r\n")
  { }
};

// EOF

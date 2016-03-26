#include "bot.hpp"
#include "stock.hpp"

#include <boost/asio.hpp>
#include <sys/utsname.h>

std::map<std::string, BotRoom::cmd_fn_t> BotRoom::cmd_map {
  { "help",    &BotRoom::cmd_help },
  { "version", &BotRoom::cmd_version },
  { "blame",   &BotRoom::cmd_blame },
  { "wisdom",  &BotRoom::cmd_wisdom },
  { "stock",   &BotRoom::cmd_stock },
};

void BotRoom::cmd_wisdom(std::string const &, std::string const &)
{
  send(wisdom.get());
}

void BotRoom::cmd_blame(std::string const &, std::string const &)
{
  auto it = participants.begin();
  std::advance(it, random_index(participants.size()));

  send(std::string("It's ") + *it + "'s fault!");
}

void BotRoom::cmd_stock(std::string const &, std::string const &body)
{
  bool is_empty = body.size() == 0;
  Bot *bot = static_cast<Bot *>(m_parent);

  stock_price = std::make_shared<StockPriceFetcher>(bot->get_io_service(), is_empty ? "FEYE" : body,
                                                    [this] (std::string const &name,
                                                            std::string const &value) {
                                                      send(name + ": " + value);
                                                    });
  stock_price->start();
}

void BotRoom::cmd_version(std::string const &, std::string const &)
{
  struct utsname name;

  if (uname(&name) == 0) {
    std::stringstream ss;

    ss << name.sysname << " " << name.release
       << " " << name.version << " " << name.machine;

    send(ss.str());

  } else {
    send("uname() failed...");
  }
}

void BotRoom::cmd_help(std::string const &, std::string const &)
{
  std::stringstream ss;

  ss << "I know the following commands:";
  for (auto p : cmd_map) {
    ss << " " << p.first;
  }
  send(ss.str());
}

// EOF

#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>

#include "bot.hpp"

int main(int argc, char **argv)
{
  if (argc < 3) {
    fprintf(stderr, "Usage: bot jid password rooms...\n");
    return EXIT_FAILURE;
  }

  std::vector<std::string> rooms;
  for (unsigned i = 3; i < argc; i++) {
    printf("Adding %s\n", argv[i]);
    rooms.emplace_back(argv[i]);
  }

  while (true) {
    boost::asio::io_service io;
    auto bot = std::make_shared<Bot>(io, gloox::JID(std::string(argv[1])), argv[2], rooms);
    io.run();

    fprintf(stderr, "Connection dropped? Waiting to reconnect...\n");
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }

  return EXIT_SUCCESS;
}

// EOF

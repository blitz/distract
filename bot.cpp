#include "bot.hpp"
#include <gloox/connectiontcpclient.h>

void BotRoom::handleMUCMessage(gloox::MUCRoom *room, const gloox::Message &msg, bool priv)
{
  std::smatch match;
  std::string const msg_body { msg.body() };

  if (std::regex_match(msg_body, match, re_cmd)) {
    std::string cmd = match.str(1);
    std::string body = match.str(2);

    printf("Matched! cmd='%s' %s\n", cmd.c_str(), body.c_str());

    auto cpair = cmd_map.find(cmd);

    if (cpair != cmd_map.end()) {
      (this->*(cpair->second))(cmd, body);
    } else {
      send("No command '" + cmd + "'. Try 'help' instead.");
    }
  }
}

void BotRoom::handleMUCParticipantPresence(gloox::MUCRoom *room,
                                           const gloox::MUCRoomParticipant participant,
                                           const gloox::Presence &presence)
{
  std::string const nick = participant.nick->resource();

  // Ignore the bot's presence.
  if (nick == this->nick()) return;

  if (presence.presence() == gloox::Presence::Available) {
    participants.insert(nick);
  } else if (presence.presence() == gloox::Presence::Unavailable) {
    participants.erase(nick);
  }
}

void Bot::onConnect()
{
  printf("Connected.\n");

  for (auto &room : rooms_to_join) {
    room_handlers.emplace_back(this, room);
  }
}

void Bot::onDisconnect(gloox::ConnectionError e)
{
  printf("Disconnected.\n");
  room_handlers.clear();
}

bool Bot::onTLSConnect(gloox::CertInfo const &info)
{
  printf("XXX No TLS certifacte check.\n");
  return true;
}

void Bot::handleMessage(const gloox::Message &stanza,
                        gloox::MessageSession *session)
{
  if (stanza.subtype() != gloox::Message::Chat) {
    printf("Ignoring message: %s\n", stanza.tag()->xml().c_str());
    return;
  }

  gloox::Message msg( gloox::Message::Chat, stanza.from(), "nothing to see here..." );
  gloox::Client::send( msg );
}

void Bot::async_read()
{
  stream.async_read_some(boost::asio::null_buffers(),
                         [this] (boost::system::error_code ec, std::size_t size) {
                           if (not ec) {
                             recv(0);
                             async_read();
                           } else {
                             fprintf(stderr, "error: %s", ec.message().c_str());
                           }
                         });
}

Bot::Bot(boost::asio::io_service &asio_io, gloox::JID const &jid,
         std::string const &password, std::vector<std::string> const &rooms)
  : stream(asio_io), gloox::Client(jid, password), rooms_to_join(rooms)
{
  gloox::Client::registerMessageHandler(this);
  gloox::Client::registerConnectionListener(this);

  connect(false);

  stream.assign(static_cast<gloox::ConnectionTCPClient *>(connectionImpl())->socket());
  async_read();
}

// EOF

#pragma once

#include <regex>
#include <set>
#include <map>
#include <string>
#include <memory>

#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/message.h>
#include <gloox/mucroom.h>
#include <gloox/connectionlistener.h>

#include "wisdom.hpp"
#include "random.hpp"
#include "stock.hpp"

class BotRoom : public gloox::MUCRoom,
                public gloox::MUCRoomHandler {

  using cmd_fn_t = void (BotRoom::*)(std::string const &cmd, std::string const &body);

  /// A regular expression to recognize commands.
  std::regex re_cmd;

  /// A map of all supported commands.
  static std::map<std::string, cmd_fn_t> cmd_map;

  /// All participants
  /// XXX What about MUCRoom::m_participants ?
  std::set<std::string> participants;

  Wisdom wisdom;

  std::shared_ptr<StockPriceFetcher> stock_price;

public:

  void cmd_wisdom (std::string const &, std::string const &);
  void cmd_blame  (std::string const &, std::string const &);
  void cmd_version(std::string const &, std::string const &);
  void cmd_help   (std::string const &, std::string const &);
  void cmd_stock  (std::string const &, std::string const &);

  virtual void handleMUCMessage(gloox::MUCRoom *room, const gloox::Message &msg, bool priv) override
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

  virtual void handleMUCError(gloox::MUCRoom *room, gloox::StanzaError error) override
  {
    printf("Could not join room %s reason %u\n", room->name().c_str(), error);
  }

  virtual void handleMUCItems(gloox::MUCRoom *room, const gloox::Disco::ItemList &items ) override
  {
  }

  virtual bool handleMUCRoomCreation(gloox::MUCRoom *room ) override
  {
    printf("Created room: %s\n", room->name().c_str());

    // Accept configuration.
    return true;
  }

  virtual void handleMUCParticipantPresence(gloox::MUCRoom *room, const gloox::MUCRoomParticipant participant,
                                            const gloox::Presence &presence) override
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

  virtual void handleMUCInfo(gloox::MUCRoom *room, int features, const std::string &name,
                             const gloox::DataForm *infoForm) override
  { }

  virtual void handleMUCSubject(gloox::MUCRoom *room, const std::string &nick,
                                const std::string &subject) override
  { }

  virtual void handleMUCInviteDecline (gloox::MUCRoom *room, const gloox::JID &invitee,
                                       const std::string &reason) override
  { }

  BotRoom(gloox::ClientBase *parent, gloox::JID const &jid)
    : gloox::MUCRoom(parent, jid, this),
      re_cmd(jid.resource() + "[,:]\\s*(\\S+)\\s*(.*)")
  {
    join();
    send("Hello!");
  }

};

class Bot : public std::enable_shared_from_this<Bot>,
            public gloox::ConnectionListener,
            public gloox::MessageHandler,
            public gloox::Client
{
  using stream_descriptor = boost::asio::posix::stream_descriptor;

  stream_descriptor stream;
  
  std::vector<BotRoom> room_handlers;

  /// A list of rooms we are going to join when we are connected.
  std::vector<std::string> rooms_to_join;

public:
  Bot(boost::asio::io_service &asio_io_, gloox::JID const &jid,
      std::string const &password, std::vector<std::string> const &rooms);

  void async_read();

  boost::asio::io_service &get_io_service()
  {
    return stream.get_io_service();
  }

  virtual void onConnect() override
  {
    printf("Connected.\n");

    for (auto &room : rooms_to_join) {
      room_handlers.emplace_back(this, room);
    }
  }

  virtual void onDisconnect(gloox::ConnectionError e) override
  {
    printf("Disconnected.\n");
    room_handlers.clear();
  }

  virtual bool onTLSConnect(gloox::CertInfo const &info) override
  {
    printf("XXX No TLS certifacte check.\n");
    return true;
  }

  virtual void handleMessage(const gloox::Message &stanza,
                             gloox::MessageSession *session = 0) override
  {
    if (stanza.subtype() != gloox::Message::Chat) {
      printf("Ignoring message: %s\n", stanza.tag()->xml().c_str());
      return;
    }

    gloox::Message msg( gloox::Message::Chat, stanza.from(), "nothing to see here..." );
    gloox::Client::send( msg );
  }
};

// EOF

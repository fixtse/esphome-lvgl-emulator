#pragma once
// No Home Assistant connection in the emulator: states come from the scenario.
#include <memory>
#include <string>
#include <vector>
#include "esphome/core/helpers.h"
namespace esphome::api {
struct SubscribeHomeAssistantStateResponse { StringRef entity_id, attribute; bool once{false}; };
struct DisconnectRequest {};
struct HomeassistantServiceMap { StringRef key, value; };
struct HomeassistantActionRequest { StringRef service; FixedVector<HomeassistantServiceMap> data, data_template; };
class APIConnection {
 public:
  template<typename T> bool send_message(const T &) { return true; }
  bool is_connection_setup() const { return false; }
};
struct HomeAssistantStateSubscription { const char *entity_id{nullptr}, *attribute{nullptr}; };
class APIServer {
 public:
  const std::vector<std::unique_ptr<APIConnection>> &active_clients() const { return clients_; }
  const std::vector<HomeAssistantStateSubscription> &get_state_subs() const { return subs_; }
  bool is_connected() const { return false; }
  bool is_connected_with_state_subscription() const { return false; }
  void send_homeassistant_action(const HomeassistantActionRequest &) {}
 private:
  std::vector<std::unique_ptr<APIConnection>> clients_;
  std::vector<HomeAssistantStateSubscription> subs_;
};
}  // namespace esphome::api

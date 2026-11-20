#pragma once
// No web server in the emulator; only the types the config component declares.
#include <optional>
#include <string>
class AsyncWebServerRequest {};
class AsyncWebHandler {
 public:
  virtual ~AsyncWebHandler() = default;
  virtual bool canHandle(AsyncWebServerRequest *) const { return false; }
  virtual void handleRequest(AsyncWebServerRequest *) {}
};

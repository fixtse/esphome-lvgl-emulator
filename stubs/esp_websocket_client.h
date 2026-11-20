#pragma once
#include <cstddef>
#include <cstdint>
#include "esp_err.h"
typedef struct esp_websocket_client *esp_websocket_client_handle_t;
typedef struct { const char *uri; const char *headers; int buffer_size; int task_stack; int task_prio; bool disable_auto_reconnect;
                 int network_timeout_ms; int reconnect_timeout_ms; int ping_interval_sec; int pingpong_timeout_sec; bool keep_alive_enable; void *user_context; bool task_stack_in_psram; } esp_websocket_client_config_t;
struct esp_websocket_error_codes_t { int error_type, esp_ws_handshake_status_code, esp_tls_stack_err, esp_tls_cert_verify_flags, esp_transport_sock_errno; esp_err_t esp_tls_last_esp_err; };
typedef struct { const char *data_ptr; int data_len; uint8_t op_code; int payload_len; int payload_offset; esp_websocket_error_codes_t error_handle; bool fin; } esp_websocket_event_data_t;
typedef enum { WEBSOCKET_EVENT_ANY = -1, WEBSOCKET_EVENT_CONNECTED = 1, WEBSOCKET_EVENT_DISCONNECTED, WEBSOCKET_EVENT_DATA,
               WEBSOCKET_EVENT_ERROR, WEBSOCKET_EVENT_CLOSED } esp_websocket_event_id_t;
typedef enum { WS_TRANSPORT_OPCODES_TEXT = 1, WS_TRANSPORT_OPCODES_PING = 9, WS_TRANSPORT_OPCODES_CLOSE = 8 } ws_transport_opcodes_t;
typedef const char *esp_event_base_t;
typedef void (*esp_event_handler_t)(void *, esp_event_base_t, int32_t, void *);
#define pdMS_TO_TICKS(ms) (ms)
#define portMAX_DELAY 0xFFFFFFFFu
inline esp_websocket_client_handle_t esp_websocket_client_init(const esp_websocket_client_config_t *) { return nullptr; }
inline esp_err_t esp_websocket_register_events(esp_websocket_client_handle_t, esp_websocket_event_id_t, esp_event_handler_t, void *) { return ESP_OK; }
inline esp_err_t esp_websocket_client_start(esp_websocket_client_handle_t) { return ESP_FAIL; }
inline esp_err_t esp_websocket_client_stop(esp_websocket_client_handle_t) { return ESP_OK; }
inline esp_err_t esp_websocket_client_destroy(esp_websocket_client_handle_t) { return ESP_OK; }
inline bool esp_websocket_client_is_connected(esp_websocket_client_handle_t) { return false; }
inline int esp_websocket_client_send_text(esp_websocket_client_handle_t, const char *, int, uint32_t) { return -1; }
inline int esp_websocket_client_send_with_opcode(esp_websocket_client_handle_t, ws_transport_opcodes_t, const uint8_t *, int, uint32_t) { return -1; }

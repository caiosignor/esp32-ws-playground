#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "private.h"
#include <SPIFFS.h>
#include <tictactoe.hpp>
#include <cJSON.h>

// https://RandomNerdTutorials.com/esp32-websocket-server-arduino/

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
TicTacToe game;

void sendWinner()
{
}

void sendGameBoard()
{
  cJSON *root = cJSON_CreateObject();
  if (root != NULL)
  {
    cJSON_AddStringToObject(root, "type", "gameboard");
    cJSON_AddItemToObject(root, "payload", game.getGameBoard());
    ws.textAll(cJSON_PrintUnformatted(root));
    cJSON_Delete(root);
  }
}

void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  Serial.println((char*)data);
  {
    cJSON *root;
    root = cJSON_Parse((const char *)data);
    if (root != NULL)
    {
      char *type = cJSON_GetStringValue(cJSON_GetObjectItem(root, "type"));

      if (strcmp(type, "restart") == 0)
        game.restart();

      if (strcmp(type, "place") == 0)
      {
        cJSON *payload = cJSON_GetObjectItem(root, "payload");
        int i = cJSON_GetObjectItem(payload, "i")->valueint;
        int j = cJSON_GetObjectItem(payload, "j")->valueint;
        game.place(client->id(), i, j);
        sendGameBoard();
      }
      cJSON_Delete(root);
    }
  }
}

void onEvent(AsyncWebSocket *server,
             AsyncWebSocketClient *client,
             AwsEventType type,
             void *arg,
             uint8_t *data,
             size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    if (!game.registerPlayer(client->id()))
      client->printf("{\"type\":\"error\",\"message\":\"Game is full\"}");
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    game.clientDisconnect(client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(client, arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  game = TicTacToe();
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  initWebSocket();

  // Start server
  server.begin();
}

void loop()
{
  ws.cleanupClients();
}
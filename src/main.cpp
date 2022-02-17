#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "private.h"
#include <SPIFFS.h>
#include <vector>
#include <cJSON.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

typedef struct _Client{
  int id;
  String name;
  String avatar;
} StClient;

std::vector<StClient> clients(5);

void sendClientList()
{
  cJSON *root, *client_list, *_client;
  
  root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "type", "client_list");
  client_list = cJSON_CreateArray();

  for (int i = 0; i < clients.size(); i++)
  {
    if(!clients[i].name.isEmpty())
    {
      _client = cJSON_CreateObject();
      cJSON_AddNumberToObject(_client, "id", (double)i);
      cJSON_AddStringToObject(_client, "name", clients.at(i).name.c_str());
      cJSON_AddStringToObject(_client, "avatar", clients.at(i).avatar.c_str());
      cJSON_AddItemToArray(client_list, _client);
    }
  }
  cJSON_AddItemToObject(root, "payload", client_list);
  ws.textAll(cJSON_PrintUnformatted(root));
  cJSON_Delete(root);
}

void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
{
  Serial.printf("msg from %d: %s\n", client->id(), data);
  cJSON *json = cJSON_Parse((char *)data);
  if (json != NULL)
  {
    if (cJSON_HasObjectItem(json, "type"))
    {
      cJSON *payload = cJSON_GetObjectItem(json, "payload");

      cJSON *type = cJSON_GetObjectItem(json, "type");
      char *type_val = cJSON_GetStringValue(type);
      Serial.printf("\t type: %s\n", type_val);
      if (strcmp(type_val, "join") == 0)
      {
        char *name = cJSON_GetObjectItem(payload, "name")->valuestring;
        char *avatar = cJSON_GetObjectItem(payload, "avatar")->valuestring;
        clients.at(client->id()).name = String(name);
        clients.at(client->id()).avatar = String(avatar);
        Serial.printf("\tid %d registered as %s\n", client->id(), clients.at(client->id()).name.c_str());
        sendClientList();
      }

      if (strcmp(type_val, "chat_message") == 0)
      {
        int to = cJSON_GetObjectItem(payload, "to")->valueint;
        char *msg = cJSON_GetObjectItem(payload, "msg")->valuestring;

        cJSON *root, *payload, *obj_payload;
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "type", "chat_message");
        payload = cJSON_CreateObject();

        cJSON_AddStringToObject(payload, "msg", msg);
        cJSON_AddNumberToObject(payload, "from", (double)client->id());
        cJSON_AddItemToObject(root, "payload", payload);
        char *str = cJSON_PrintUnformatted(root);
        ws.text(to, str);
        cJSON_Delete(root);
        Serial.printf("ws send %s to client %d\n", str, to);
      }

    }
    cJSON_Delete(json);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    clients[client->id()].name.clear();
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
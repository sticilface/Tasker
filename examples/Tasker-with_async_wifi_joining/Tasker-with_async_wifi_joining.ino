/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <Tasker.h>

ASyncTasker tasker;

const char* ssid = "your-ssid";
const char* password = "your-password";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  tasker.add([](Task & t) {
    testConnect();
  });

  tasker.add([](Task & t) {
    WiFiConnect();
  }).setTimeout(5000);

  //  print dots to show async methods...
  tasker.add([](Task & t) {
    Serial.println(".");
  }).setTimeout(1000);


  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {

  tasker.loop();
  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}


void testConnect() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(2, val);

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val) ? "high" : "low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}


enum wifi_func_state {
  wifi_disconnected,
  wifi_connecting,
  wifi_connected,
  wifi_error,
  wifi_unknown
} wifi_state;

wifi_state = unknown;

bool WiFiConnect() {
  // Connect to WiFi network



  switch (wifi_state) {

    case wifi_disconnected: {

        break;
      }
    case wifi_connecting: {

        break;
      }
    case wifi_connected: {

        break;
      }

    case wifi_error: {

        break;
      }

    case wifi_unknown: {
        if (WiFi.isConnected() && WiFi.SSID() == ssid && WiFi.psk() == password) {
          wifi_state = wifi_disconnected;
        }
        break;
      }




  }




  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}


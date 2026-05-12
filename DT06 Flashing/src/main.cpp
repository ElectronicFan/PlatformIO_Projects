// Version 1.00
#include <ESP8266WiFi.h>

WiFiServer server(8080); // B4J will connect to this port
WiFiClient client;

// On DT-06/ESP8285, GPIO4 is typically the onboard LED
#define LED_PIN 4

void setup() {
// 1. Boost the Buffer BEFORE Serial.begin
  Serial.setRxBufferSize(1024); 
  Serial.begin(57600);
  delay(1500);
  
  
  // 2. Set CPU to Max Speed
  WiFi.setOutputPower(20.5); // Maximize radio strength
  WiFi.softAP("GoBox_Bridge", "12345678");
  
  client.setNoDelay(true); // Ensure this is set when client connects
  server.begin();

  // Turn on the LED to indicate we're ready (active LOW)
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("WiFi Bridge Ready! Connect to 'GoBox_Bridge' with password '12345678");
}

void loop() {
  // Check for a NEW connection attempt
  WiFiClient newClient = server.accept(); // Use accept() instead of available()
  
  if (newClient) {
    // If we already have a client, stop the old one to free up the socket
    if (client) 
    {
        client.stop();
    }
    client = newClient;
    client.setNoDelay(true); 
    Serial.println("B4J Connected!"); 
  }

  // Ensure client is still valid before trying to move data
  if (client && client.connected()) 
  {
    // 1. WiFi -> Serial (Dumb Pipe)
    while (client.available() > 0) 
    {
      Serial.write(client.read());
    }

    // 2. Serial -> WiFi (Dumb Pipe)
    while (Serial.available() > 0) {
      client.write(Serial.read());
    }

  }
}

// Version 1.01

#include <ESP8266WiFi.h>

WiFiServer server(8080); // B4J will connect to this port
WiFiClient client;

// On DT-06/ESP8285, GPIO4 is typically the onboard LED
#define LED_PIN 4

static uint8_t bridgeBuffer[2048];    
static unsigned long lastSendTime = 0; 
static size_t bufferIdx = 0;

void setup() 
{
// 1. Boost the Buffer BEFORE Serial.begin
  Serial.setRxBufferSize(2048); 
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

void loop() 
{
  // Check for a NEW connection attempt
  WiFiClient newClient = server.accept(); // Use accept() instead of available()
  
  if (newClient) 
  {
    // If we already have a client, stop the old one to free up the socket
    if (client) 
    {
        client.stop();
    }
    client = newClient;
    client.setNoDelay(true); 
  }

  if (client && client.connected()) {
    // Fill the buffer as fast as UART allows
    while (Serial.available() > 0 && bufferIdx < (int)sizeof(bridgeBuffer))
    {
        bridgeBuffer[bufferIdx++] = Serial.read();
    }

    // Send if: Buffer is getting full OR 20ms has passed since last data
    if (bufferIdx > 0) 
    {
        if (bufferIdx >= 256 || (millis() - lastSendTime > 20)) {
            client.write(bridgeBuffer, bufferIdx);
            bufferIdx = 0; // Reset index
            lastSendTime = millis();
        }
    }
    
    // WiFi to PIC should stay direct for the Cancel command
    if (client.available() > 0) {
        Serial.write(client.read());
    }
  }

}
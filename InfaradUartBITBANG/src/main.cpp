#include <Arduino.h>

// Use ESP32e board with a 38kHz IR LED connected to GPIO 18 and a receiver connected to GPIO 16. 
// The code bit-bangs UART data at 2400 baud using a 38kHz carrier
// Tested up to 4800 baud, but 2400 is more reliable for long distances and noisy environments.
// IR LED and Receiver i bought from Aliexpress, but any 38kHz IR LED and receiver should work.  The IR LED is driven by a transistor circuit to provide enough current for long distance transmission.

// ==================== HARDWARE PIN SCHEMATICS ====================
//
//   ESP32-E                                 IR LED TRANSMITTER
//  +---------+         Diode               +-------------------+
//  | GPIO 18 |------->| [>]|---------------| DATA              |
//  |  3.3V   |-----------------------------| VCC               |
//  |   GND   |-----------------------------| GND               |
//  +---------+                             +-------------------+
//
//   ESP32-E                                 IR RECEIVER
//  +---------+                             +-------------------+
//  | GPIO 16 |-----------------------------| DATA              |
//  |  3.3V   |-----------------------------| VCC               |
//  |   GND   |-----------------------------| GND               |
//  +---------+                             +-------------------+
//
// ==================================================================

#define PWM_38KHZ_PIN 18  
#define RXD_PIN       16  

// ==================================================================
// FORWARD DECLARATIONS (The compiler needs to see these first!)
// ==================================================================

void bitBangUartByte(uint8_t data);
void send38kHzBurst(uint32_t durationUs);
void sendQuietWindow(uint32_t durationUs);
void flushHardwareRxBuffer();

void setup() {
    Serial.begin(115200);
    
    pinMode(PWM_38KHZ_PIN, OUTPUT);
    digitalWrite(PWM_38KHZ_PIN, HIGH);          // Drive high to keep diode circuit dark
    
    Serial1.begin(2400, SERIAL_8N1, RXD_PIN, -1); 
}


void loop() {
    static uint32_t lastTxTime = 0;
    if (millis() - lastTxTime >= 2000) {
        lastTxTime = millis();
        
        // Define your payload array here
        uint8_t txPayload[] = {0x33, 0x55, 0xAA};
        const uint8_t len = sizeof(txPayload);  // Automatically calculates length

        Serial.println("\nTransmitting array at 2400 Baud...");
        
        // 1. CLEAR PRE-EXISTING WIGGLE GARBAGE
        flushHardwareRxBuffer(); 
        
        // Disable interrupts for clean bit-bang execution
        portDISABLE_INTERRUPTS(); 
        
        // Loop through the array back-to-back
        for (uint8_t i = 0; i < len; i++) {
            bitBangUartByte(txPayload[i]);
        }

        portENABLE_INTERRUPTS(); 
        
        
        // 2. TIMING BUFFER FOR THE AIR
        delay(5); 

        
        // 3. READ VALID DATA ONLY 
        uint8_t bytesRead = 0;
        while (Serial1.available() > 0) {
            uint8_t receivedByte = Serial1.read();
            Serial.print("Success! Received: 0x");
            Serial.println(receivedByte, HEX);
            
            bytesRead++;
            if (bytesRead >= len) {           // Uses the auto-calculated length!
                break; 
            }
        }
        
        // 4. THE SHUTTER: FLUSH POST-TRANSMISSION JITTER
        flushHardwareRxBuffer(); 
        

    }
}

// Bit-bang a single byte over the 38kHz carrier at 2400 baud
void bitBangUartByte(uint8_t data) {
    send38kHzBurst(416);                        // Start Bit
    
    // 8 data bits, LSB first
    for (int i = 0; i < 8; i++) {
        if ((data >> i) & 0x01) {
            sendQuietWindow(416);               // 1/2400 baud = 416.67us for a '1' bit
        } else {
            send38kHzBurst(416);                // 1/2400 baud = 416.67us for a '0' bit
        }
    }
    
    sendQuietWindow(416);                       // Stop Bit
}

// Send uart data bit using a 38kHz carrier frequency for the specified duration in microseconds
void send38kHzBurst(uint32_t durationUs) {
    uint32_t startTime = micros();
    while ((micros() - startTime) < durationUs) {
        // total period for 38kHz is ~26.3us (1/38000 = 26.3us total delay = 9us LOW and 17us HIGH), so we can approximate with 34% duty cycle.
        // 34 % Duty Cycle: 26.3 * 0.34 = 8.942 us (rounded to 9us for LOW)
        digitalWrite(PWM_38KHZ_PIN, LOW);
        delayMicroseconds(9);                   // 34% duty cycle for 38kHz
        digitalWrite(PWM_38KHZ_PIN, HIGH);
        delayMicroseconds(17);
    }
}

 // Send a quiet window (no 38kHz burst) for the specified duration in microseconds
void sendQuietWindow(uint32_t durationUs) {
   
    digitalWrite(PWM_38KHZ_PIN, HIGH);          // bit '1' is represented by a quiet window (no 38kHz burst)
    delayMicroseconds(durationUs);
}

// Aggressively flushes the internal hardware UART FIFO buffer
// This is important because wiggle tx infarad causing excessive noise can cause the hardware UART to misinterpret incoming data, leading to garbage bytes being read.
void flushHardwareRxBuffer() {
    while (Serial1.available() > 0) {
        Serial1.read(); 
    }
}

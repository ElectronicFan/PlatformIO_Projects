/* * =================================================================
 * PROJECT: Demo Uploader
 * HARDWARE: ESP32 + XPT2046 Touch + TFT Display + SD Card(Optional)
 * CONFIGURATION: Dual SPI (Separate Buses)
 * TFT9488 Seperate Bus. Touch2046/SD Card Shared Bus.
 * =================================================================
* * --- TFT DISPLAY (Bus 1: FSPI) ---
 * MOSI/SDI:   13                       -> Master Out Slave In
 * SCLK:       14                       -> Serial Clock
 * MISO/SDO:   -1                       -> (Not used by ILI9488)
 * CS:         10                       -> Chip Select
 * DC:          2                       -> Data/Command
 * RST:        12                       -> Reset 
 * * --- TOUCH SENSOR OR/AND SD Card (Shared Bus 2: HSPI) ---
 * TOUCH_DIN:  4 (MOSI)                 -> Data In to XPT2046
 * TOUCH_CLK:  5 (SCLK)                 -> Clock from ESP32 to XPT2046
 * TOUCH_DO:   6 (MISO)                 -> Data Out from XPT2046
 * TOUCH_CS:   15                       -> Touch Chip Select
 * TOUCH_IRQ:   1                       -> Touch Interrupt (Active LOW)
 * SD_MOSI      4                       -> Data In to SD Card
 * SD_MISO      6                       -> Data Out from SD Card
 * SD_SCK       5                       -> Clock from ESP32 to SD Card
 * SD_CS:      21                       -> SD Card Chip Select
 * * --- POWER & GROUND ---
 * VCC:    3.3V or 5V (Check Screen Regulator)
 * GND:    Common Ground
 * =================================================================
 */
// SD Card not used in this project but tested with it wired with Touch Display

#include <Arduino.h>
#include <SPI.h>

// TFT and Touch Pins defined in Platformio.ini!
#define SD_CS 21

#include "globals.h"
#include "display_logic.h"
#include "ble.h"



// Define the actual objects here (Allocation)
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

SPIClass touchSPI(HSPI); 

bool wasTouched = false;

void setup() 
{
    Serial.begin(115200);
    delay(2500); // Good delay for the S3 USB Serial to catch up. Can remove if debugging not needed!

    // --- IMPORTANT: QUIET THE SD CARD ---
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH); // Keeps SD card inactive

    // 1. Initialize TFT (Uses Bus 1: FSPI)
    tft.init();
    tft.setRotation(1); // 480x320 Landscape

    // 2. Initialize Touch SPI (Uses Bus 2: HSPI)
    // Make sure these names match your -D flags in platformio.ini
    touchSPI.begin(TOUCH_SCLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS); 

    if (!touch.begin(touchSPI)) {
        Serial.println("Touchscreen init failed!");
    } else {
        Serial.println("Touchscreen online.");
    }
    
    touch.setRotation(1); // Keep this matching tft.setRotation

    drawUI();
    
    Serial.println("Issac Engineer Go-Box Ready.");
}

void loop() 
{
    bool isCurrentlyTouched = touch.touched();

    // Only act if it's a NEW touch
    if (isCurrentlyTouched && !wasTouched) {
        Serial.println("Handle Touch Pressed");
        handleTouch(); 
    }
    
    // Update the state
    wasTouched = isCurrentlyTouched;

    if (doConnect == true) 
    {
        if (bleconnectToServer()) {
            Serial.println("We are now connected to the BLE Server.");
        } else {
            Serial.println("We have failed to connect to the server; there is nothing more we will do.");
            doScan = true; 
        }
    }

    if (!bleIsConnected() && doConnect == false && doScan == true) 
    {
        Serial.println("BLE is not connected, attempting to reconnect...");
        bleDoScan();
    }
 
    delay(10);
}


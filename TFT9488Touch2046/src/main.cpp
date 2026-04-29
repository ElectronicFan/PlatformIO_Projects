/* * =================================================================
 * PROJECT: Issac Engineer Uploader (Go-Box)
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
 * TOUCH_DIN:  4 (MOSI)                 -> Data In to XPT2046/SD Card
 * TOUCH_CLK:  5 (SCLK)                 -> Clock from ESP32 to XPT2046/SD Card
 * TOUCH_DO:   6 (MISO)                 -> Data Out from XPT2046/SD Card
 * TOUCH_CS:   15                       -> Touch Chip Select
 * TOUCH_IRQ:   1                       -> Touch Interrupt (Active LOW) Use 1 and test
 * SD_CS:      21                       -> SD Card Chip Select
 * * --- POWER & GROUND ---
 * VCC:    3.3V or 5V (Check Screen Regulator)
 * GND:    Common Ground
 * =================================================================
 */
// SD Card not used in this project but tested with it wired with Touch Display

#include <Arduino.h>  // <--- ADD THIS AS LINE 1
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// TFT and Touch Pins defined in Platformio.ini!
#define SD_CS 21

enum MenuState { MAIN, FLASH, BT_CHECK, CALIB, ABOUT };

// --- PROTOTYPES (REQUIRED IN .cpp) ---
void handleTouch();
void drawUI();
void drawMainMenu();
void drawFlashMenu();
void drawBTMenu();
void drawCalibMenu();
void drawAboutMenu();
void createButton(int x, int y, int w, int h, uint32_t color, const char *label);
void drawBackButton();
void changeMenu(MenuState next);

TFT_eSPI tft = TFT_eSPI();
SPIClass touchSPI(HSPI); // Dedicated bus for Touch
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

MenuState currentMenu = MAIN;       // Menu State tracking
bool wasTouched = false;

void setup() {
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

void loop() {
    bool isCurrentlyTouched = touch.touched();

    // Only act if it's a NEW touch
    if (isCurrentlyTouched && !wasTouched) {
        handleTouch(); 
    }
    
    // Update the state
    wasTouched = isCurrentlyTouched;
    
    delay(10);
}

// --- TOUCH HANDLER ---
void handleTouch() {
    TS_Point p = touch.getPoint();
    
    // X is fine (Left to Right)
    int x = map(p.x, 300, 3800, 0, 480);
    
    // Y is FLIPPED (Top to Bottom)
    // By putting 3800 first, we tell the ESP32 that the 
    // "high" signal is actually the "top" of the screen.
    int y = map(p.y, 3800, 300, 0, 320); 

    if (currentMenu == MAIN) {
        if (x > 20 && x < 230 && y > 60 && y < 170)    changeMenu(FLASH);
        if (x > 250 && x < 460 && y > 60 && y < 170)   changeMenu(BT_CHECK);
        if (x > 20 && x < 230 && y > 190 && y < 300)   changeMenu(CALIB);
        if (x > 250 && x < 460 && y > 190 && y < 300)  changeMenu(ABOUT);
    } else {
        if (x > 360 && x < 460 && y > 260 && y < 305)  changeMenu(MAIN);        // Back button
    }
}

// --- CORE UI CONTROLLER ---
void drawUI() {
    tft.fillScreen(TFT_BLACK);
    
    switch (currentMenu) {
        case MAIN:     drawMainMenu();    break;
        case FLASH:    drawFlashMenu();   break;
        case BT_CHECK: drawBTMenu();      break;
        case CALIB:    drawCalibMenu();   break;
        case ABOUT:    drawAboutMenu();   break;
    }
}

// --- MAIN MENU ---
void drawMainMenu() {
    tft.setTextColor(TFT_YELLOW);
    tft.drawCentreString("ISSAC ENGINEER UPLOADER", 240, 10, 4);
    
    // Grid: [Flash] [BT] / [Calib] [About]
    createButton(20,  60,  210, 110, TFT_RED,    "FLASH");
    createButton(250, 60,  210, 110, TFT_BLUE,   "CHECK BT");
    createButton(20,  190, 210, 110, TFT_GREEN,  "CALIBRATION");
    createButton(250, 190, 210, 110, TFT_ORANGE, "ABOUT");
}

// --- SUB-MENUS ---
void drawFlashMenu() {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("FLASH MODE", 240, 20, 4);
    tft.drawRect(20, 80, 440, 150, TFT_WHITE); // Hex file list area
    tft.drawCentreString("Waiting for SD...", 240, 140, 2);
    drawBackButton();
}

void drawBTMenu() {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("BLUETOOTH STATUS", 240, 20, 4);
    // Placeholder for your DT-06 / HC-05 status
    tft.setTextColor(TFT_CYAN);
    tft.drawCentreString("BT Module: DISCONNECTED", 240, 150, 2);
    drawBackButton();
}

void drawCalibMenu() {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("TOUCH CALIBRATION", 240, 20, 4);
    tft.drawCircle(40, 40, 10, TFT_WHITE); // Calibration points
    tft.drawCentreString("Tap crosshairs to calibrate", 240, 160, 2);
    drawBackButton();
}

void drawAboutMenu() {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("ABOUT", 240, 20, 4);
    tft.setCursor(60, 100);
    tft.println("Firmware: v1.0");
    tft.setCursor(60, 130);
    tft.println("Engineer: Issac Engineer");
    tft.setCursor(60, 160);
    tft.println("Hardware: ESP32 + ILI9488");
    drawBackButton();
}

// --- UI HELPERS ---
void createButton(int x, int y, int w, int h, uint32_t color, const char *label) {
    tft.fillRoundRect(x, y, w, h, 10, color);
    tft.drawRoundRect(x, y, w, h, 10, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(label, x + (w / 2), y + (h / 2) - 10, 4);
}

void drawBackButton() {
    createButton(360, 260, 100, 45, TFT_DARKGREY, "BACK");
}

void changeMenu(MenuState next) {
    if (currentMenu == next) return;  // prevents redraw spam
    currentMenu = next;
    drawUI();
}
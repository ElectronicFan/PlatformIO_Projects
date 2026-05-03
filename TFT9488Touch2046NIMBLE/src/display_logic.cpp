#include "globals.h"
#include "display_logic.h"
#include "ble.h"

MenuState currentMenu = MAIN;

// --- TOUCH HANDLER ---
void handleTouch() 
{
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
void drawUI() 
{
    tft.fillScreen(TFT_BLACK);
    
    switch (currentMenu) 
    {
        case MAIN:     drawMainMenu();    break;
        case FLASH:    drawFlashMenu();   break;
        case BT_CHECK: drawBTMenu();      break;
        case CALIB:    drawCalibMenu();   break;
        case ABOUT:    drawAboutMenu();   break;
    }
}

// --- MAIN MENU ---
void drawMainMenu() 
{
    tft.setTextColor(TFT_YELLOW);
    tft.drawCentreString("ISSAC ENGINEER UPLOADER", 240, 10, 4);
    
    // Grid: [Flash] [BT] / [Calib] [About]
    createButton(20,  60,  210, 110, TFT_RED,    "FLASH");
    createButton(250, 60,  210, 110, TFT_BLUE,   "CHECK BT");
    createButton(20,  190, 210, 110, TFT_GREEN,  "CALIBRATION");
    createButton(250, 190, 210, 110, TFT_ORANGE, "ABOUT");
}

// --- SUB-MENUS ---
void drawFlashMenu() 
{
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("FLASH MODE", 240, 20, 4);
    tft.drawRect(20, 80, 440, 150, TFT_WHITE); // Hex file list area
    tft.drawCentreString("Waiting for SD...", 240, 140, 4);
    drawBackButton();
}

void drawBTMenu() 
{
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //tft.setTextSize(2); // Multiplies the size of the font you call next
    tft.drawCentreString("BLUETOOTH STATUS", 240, 20, 4);
    tft.setTextColor(TFT_CYAN);
    
    if (bleIsConnected()) 
    {
        // Update your ILI9488 display to show a green icon or "Online"
        tft.drawCentreString("BT Module: CONNECTED", 240, 120, 2);
        // DEVICE NAME
        String dName = myDevice->getName().c_str();
        if (dName == "") dName = "Unknown Device";
        tft.drawCentreString("Device: " + dName, 240, 140, 2);
        // MAC ADDRESS
        String macAddr = myDevice->getAddress().toString().c_str();
        tft.drawCentreString("MAC: " + macAddr, 240, 160, 2);
        // UUID
        String dUUID = myDevice->getServiceUUID().toString().c_str();
        tft.drawCentreString("Service UUID: " + dUUID, 240, 180, 2);

    } else {
        // Show "Offline" or "Scanning..."
         tft.drawCentreString("BT Module: DISCONNECTED", 240, 150, 2);
    }
    //tft.setTextSize(1); 
    drawBackButton();
}

void drawCalibMenu() 
{
    uint16_t calibrationData[5];

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);

    
    tft.drawCentreString("TOUCH CALIBRATION", 240, 20, 4);
    tft.drawCircle(40, 40, 10, TFT_WHITE); // Calibration points
    tft.drawCentreString("Tap crosshairs to calibrate", 240, 160, 2);
    drawBackButton();
    
}

void drawAboutMenu() 
{
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
void createButton(int x, int y, int w, int h, uint32_t color, const char *label) 
{
    tft.fillRoundRect(x, y, w, h, 10, color);
    tft.drawRoundRect(x, y, w, h, 10, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(label, x + (w / 2), y + (h / 2) - 10, 4);
}

void drawBackButton() 
{
    createButton(360, 260, 100, 45, TFT_DARKGREY, "BACK");
}

void changeMenu(MenuState next) 
{
    if (currentMenu == next) return;  // prevents redraw spam
    currentMenu = next;
    drawUI();
}
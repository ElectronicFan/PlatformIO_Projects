```markdown

\# Issac Engineer Go-Box: ESP32-S3 Unified PIC Uploader



A standalone, handheld hardware programmer designed to flash firmware to PIC microcontrollers (16F, 18F, 24F) without a PC. This project features a custom GUI, dual SPI bus management, and SD card integration.



\## 🚀 Features

\- \*\*Standalone Operation:\*\* Flash .hex files directly from an SD Card.

\- \*\*High-Resolution UI:\*\* 3.5" ILI9488 TFT Display (480x320) with touch interface.

\- \*\*Dual SPI Architecture:\*\* - \*\*Bus 1 (FSPI):\*\* Dedicated to high-speed TFT rendering.

&#x20; - \*\*Bus 2 (HSPI):\*\* Shared between XPT2046 Touch Controller and SD Card.

\- \*\*Cross-Platform Compatibility:\*\* Logic designed to interface with B4X and Python ecosystems.

\- \*\*Multi-Protocol Support:\*\* Prepared for TTL-USB, Bluetooth (HC-05/DT-06), and BLE.



\## 🛠 Hardware Configuration

\- \*\*MCU:\*\* ESP32-S3

\- \*\*Display:\*\* ILI9488 3.5" TFT

\- \*\*Touch:\*\* XPT2046 (Resistive)

\- \*\*Storage:\*\* MicroSD via SPI



\### Pin Mapping (ESP32-S3)

| Function    | Pin | Bus      |

|-------------|-----|----------|

| TFT\_CS      | 10  | SPI Bus 1|

| TFT\_DC      | 11  | SPI Bus 1|

| TOUCH\_CS    | 15  | SPI Bus 2|

| SD\_CS       | 21  | SPI Bus 2|

| TOUCH\_SCLK  | 5   | SPI Bus 2|

| TOUCH\_MISO  | 6   | SPI Bus 2|

| TOUCH\_MOSI  | 4   | SPI Bus 2|

| TOUCH\_IRQ   | 1   | Interrupt|



\## 💻 Software Stack

\- \*\*Framework:\*\* Arduino / PlatformIO

\- \*\*Graphics:\*\* \[TFT\_eSPI](https://github.com/Bodmer/TFT\_eSPI)

\- \*\*Touch:\*\* \[XPT2046\_Touchscreen](https://github.com/PaulStoffregen/XPT2046\_Touchscreen)

\- \*\*Development Environment:\*\* VS Code + PlatformIO



\## ⚙️ Installation \& Build

1\. Clone the repository.

2\. Open in VS Code with the PlatformIO extension.

3\. Ensure your `platformio.ini` includes the following build flags:

&#x20;  ```ini

&#x20;  build\_flags =

&#x20;    -D USER\_SETUP\_LOADED=1

&#x20;    -D ILI9488\_DRIVER=1

&#x20;    -D TOUCH\_CS=15

&#x20;    -D SD\_CS=21

&#x20;    -D TOUCH\_SCLK=5

&#x20;    -D TOUCH\_MISO=6

&#x20;    -D TOUCH\_MOSI=4

&#x20;    -D TOUCH\_IRQ=1


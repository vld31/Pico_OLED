# Pico OLED Notification Display

This project displays notifications from a backend server on a Raspberry Pi Pico W with an OLED display.

## WiFi Configuration

Before building the project, you need to configure your WiFi credentials in `CMakeLists.txt`.

Open `CMakeLists.txt` and find these lines (around line 34-35):

```cmake
set(WIFI_SSID "WIFI NAME") #Modify name to your network name
set(WIFI_PASSWORD "WIFI PASSWORD") #Modify this to your network password
```

Replace `"WIFI NAME"` with your actual WiFi network name and `"WIFI PASSWORD"` with your WiFi password:

```cmake
set(WIFI_SSID "MyHomeNetwork")
set(WIFI_PASSWORD "mypassword123")
```

## Building and Flashing

After configuring WiFi credentials, build and flash the project to your Pico W using the provided VS Code tasks.

## Hardware Requirements

- Raspberry Pi Pico W
- SSD1306 OLED Display (128x64, I2C)
- Buzzer (connected to GP20)
- Button (connected to GP10)

## Features

- Connects to WiFi network
- Polls backend server for notifications every 5 seconds
- Displays notifications on OLED with word-wrapping
- Plays beep sound when notification received
- Displays QR code when idle

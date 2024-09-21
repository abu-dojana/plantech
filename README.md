# plantech
CSE 3118 Course Project. Automatic Plant Watering System!

# Automatic Plant Watering System

## Overview

This project is an automated plant watering system built using an ESP32 microcontroller. It monitors soil moisture levels and automatically waters plants when needed. The system includes a web interface for easy configuration and monitoring.

## Features

- Soil moisture monitoring
- Automatic watering based on moisture levels
- Water level monitoring in the reservoir
- Temperature and humidity sensing
- Web interface for system control and monitoring
- WiFi connectivity for remote access

## Hardware Requirements

- ESP32 microcontroller
- Soil moisture sensor
- Water level sensor
- DHT11 temperature and humidity sensor
- Water pump
- L298N Motor Driver
- 12V ~ 1A DC Power supply
- Connecting wires

## Software Requirements

- Arduino IDE
- Required libraries (WiFi, WebServer, DNSServer, DHT)

## Installation

1. Clone this repository or download the source code.
2. Open the `plantech.ino` file in Arduino IDE.
3. Install the required libraries through the Arduino Library Manager.
4. Upload the code to your ESP32 board.

## Setup and Configuration

1. Connect the components according to the provided circuit diagram (available on GitHub).
2. Power on the system.
3. Connect to the "PlanTech" WiFi network using the password "admin123".
4. Open a web browser and navigate to `http://192.168.10.10`.
5. Go to the "Control" tab in the web interface.
6. Calibrate the sensors:
   - For soil moisture sensor: Use "Start Calibrate" when the sensor is in dry soil, then "Stop Calibrate". Repeat the process with the sensor in wet soil.
   - For water level sensor: Use "Start Calibrate" when the sensor is out of water, then "Stop Calibrate". Repeat the process with the sensor fully submerged.
7. Set the pump duration (recommended: 1 second) and cooldown timer (recommended: 6 hours).

## Usage

- The system will automatically monitor soil moisture and water the plants when needed.
- You can manually control the pump and adjust settings through the web interface.
- Monitor current readings and system status on the home page of the web interface.

## Troubleshooting

- If you can't connect to the WiFi network, try resetting the ESP32.
- Ensure all sensors and the pump are correctly connected.
- Check the serial monitor in Arduino IDE for any error messages during setup.

## Contributing

Contributions to improve the project are welcome. Please fork the repository and submit a pull request with your changes.

## License

[MIT License](LICENSE)

## Acknowledgments

- Thanks to all contributors and open-source libraries used in this project.
- Special Thanks to educ8s.tv (https://youtu.be/aUSwEkJCIAA?si=2lvx7QNANX3AVBHA) for the idea of using webpage as Character in ESP32. 

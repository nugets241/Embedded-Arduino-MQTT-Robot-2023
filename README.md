# Autonomous Robot Control System

This project implements an autonomous robot control system using Arduino, ESP8266, and MQTT for communication. The system employs LIDAR for distance sensing, a compass for orientation, and PWM for motor control. The ESP8266 module is utilized for wireless communication using MQTT, enabling remote command processing and telemetry.

## Components

### ArduinoFinal.ino

This file contains the main code for the Arduino board. It manages LIDAR sensor readings, compass data for orientation, and controls the motors using PWM signals. It features:

- LIDAR sensor data processing for accurate distance measurement.
- Compass module integration for determining the robot's direction.
- PWM signal generation for precise motor control.
- A joystick-based control system for manual operation.
- Serial communication for sending and receiving commands and updates.

You can find this file in the ArduinoFinal directory.

### ESPFinal.ino

This ESP8266 sketch is responsible for handling wireless communication and MQTT messaging. It connects to an MQTT broker and publishes messages received from the Arduino, including LIDAR readings, compass directions, and system states. Features include:

- WiFi connectivity for remote communication.
- MQTT client for publishing and subscribing to topics.
- Serial communication to receive data from the Arduino.
- JSON message formatting for data transmission.

You can find this file in the ESPFinal directory.

### mqtt_subpub_Final

The MQTT communication module includes files for setting up an MQTT broker, subscribing to topics, and sending messages. The system is designed to work with a Raspberry Pi or similar device. Key components:

- `main.cpp`: Main program for MQTT communication and user input processing.
- `mqtt.h`, `mqtt_receiv.cpp`, `mqtt_receiv.h`: MQTT subscription, message handling, and publishing.
- `Makefile`, `go1`: Compilation and build scripts.

You can find these files in the mqtt_subpub_Final directory.

## Installation

To install and run this project, follow these steps:

1. Ensure you have Arduino IDE and the necessary libraries for the Arduino sketch.
2. Install the ESP8266 board package in the Arduino IDE for ESPFinal.ino.
3. Compile and upload [ArduinoFinal.ino](ArduinoFinal/ArduinoFinal.ino) to your Arduino board and [ESPFinal.ino](ESPFinal/ESPFinal.ino) to your ESP8266 module.
4. Set up an MQTT broker (e.g., Mosquitto) on a Raspberry Pi or a similar system.
5. Compile the MQTT communication module using the provided Makefile in the [mqtt_subpub_Final](mqtt_subpub_Final) directory.

## Usage

To use the system:

1. Power on the robot with the Arduino and ESP8266 modules.
2. Ensure the MQTT broker is running on your Raspberry Pi or similar device.
3. Use MQTT client software to publish commands to the robot and subscribe to its telemetry data.

## Code Structure

- [ArduinoFinal.ino](ArduinoFinal/ArduinoFinal.ino): This file contains the main code for the Arduino board. It includes functions for controlling the robot's movements and reading sensor data.
- [ESPFinal.ino](ESPFinal/ESPFinal.ino): This file contains the code for the ESP8266 module. It is responsible for connecting to the MQTT broker and publishing/subscribing to MQTT topics.
- [mqtt_subpub_Final](mqtt_subpub_Final): This directory contains the code for the MQTT communication module. It uses the Mosquitto library to connect to the MQTT broker and handle MQTT messages.

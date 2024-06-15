### Introduction

:warning: This repository was created for the Edge Computing in the IoT course project.

:no_entry_sign: It is not intended for reuse.

:heart: Special thanks to my team [Christian Altrichter](https://github.com/Altricch) and [Francesco Huber](https://github.com/Frakk0) for their contributions.

## Overview

This project outlines the development of the Light Schraenkers Intrusion System, designed to enhance security by integrating various sensors and communication protocols. 
The system primarily employs Time of Flight (ToF) sensors, a fingerprint sensor, and Bluetooth communication to monitor and respond to unauthorized entries. 

The primary objective was to create a cost-effective, energy-efficient, and reliable system capable of detecting intrusions and alerting users through a combination of sensors and actuators.

## System Design and Implementation

The system is centered around two key devices: the Arduino Nano 33 and the Arduino Portenta.

The ToF sensors are strategically placed to detect movement, triggering alerts when the distance measurement falls below a predefined threshold. 
Initial attempts to establish communication between the devices via I2C and SPI were unsuccessful due to hardware limitations, leading to the adoption of digitalRead and digitalWrite methods. 

Additionally, the system includes a fingerprint sensor connected via Bluetooth, ensuring secure system activation and deactivation. 
The master-slave configuration between the Portenta and Nano ensures synchronized operation and accurate people counting.

## Challenges and Future Improvements

The development process encountered several challenges, particularly in establishing reliable communication between devices and optimizing energy consumption. 
The I2C bus limitations necessitated alternative communication strategies, and the fingerprint sensor integration required precise calibration and synchronization. 

Future improvements could include enhancing the multi-threading capabilities of the system to allow simultaneous image capture and motor control, as well as implementing push notifications to alert users of intrusions in real-time. 
With additional time and resources, these enhancements would further solidify the system's reliability and efficiency.

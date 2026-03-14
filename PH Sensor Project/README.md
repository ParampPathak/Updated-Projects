# PH Sensor Project

## Overview
This project is an Arduino-based automated pH control system designed to monitor the pH of a solution and respond by dispensing either acid or base when adjustment is needed. The system was built as a team project to demonstrate how sensors, microcontrollers, relay-based control, and pumps can be integrated into a simple closed-loop chemical control setup.

The main goal of the project was to create a prototype that can:
- read the current pH of a liquid solution,
- accept a desired target pH from the user,
- compare the measured pH to the target value,
- activate pumps through relay modules to add acid or base as needed,
- display system prompts and status information on an LCD.

## Project Idea
Maintaining pH is important in many real-world systems such as water treatment, hydroponics, chemical processing, and laboratory automation. Our team designed this prototype to simulate an automatic pH regulation system using low-cost electronics and an Arduino platform.

The system reads data from a pH sensor probe and uses programmed logic to determine whether the solution is too acidic, too basic, or close enough to the target value. Based on that reading, the Arduino controls pumps that can dispense corrective liquid into the container.

## Features
- Real-time pH sensing using a pH sensor probe and interface board
- User-defined target pH entered through serial communication
- LCD output for prompts and status messages
- Automatic control of acid and base dispensing pumps
- Relay module switching for pump control
- Team-built enclosure to house electronics and improve presentation
- Images and videos showing the full setup and project operation

## Hardware Used
The project was built using the following main components:

- Arduino Uno
- pH sensor probe and pH sensor module
- 16x2 LCD display
- Relay module
- Breadboard
- Jumper wires
- Liquid pumps
- Tubing for fluid transfer
- Power connections for pumps and control electronics
- Custom enclosure for the system
- Containers for acid, base, and test solution

## System Operation
1. The user enters a desired pH value through the Arduino Serial Monitor.
2. The Arduino reads the current pH from the sensor.
3. The measured pH is compared to the target pH.
4. If the pH is below the target range, the system activates the base pump.
5. If the pH is above the target range, the system activates the acid pump.
6. The LCD displays prompts or status information during operation.
7. The process continues until the solution moves toward the desired pH.

## Team Contribution
This project was developed collaboratively as a team. Work included:
- designing the physical enclosure,
- wiring the Arduino circuit,
- integrating the pH sensor and LCD,
- programming the control logic,
- setting up the relay-controlled pump system,
- testing the dispensing behavior and display output.

## Media
The repository includes both images and videos showing:
- the full project setup,
- the internal Arduino and breadboard wiring,
- the LCD interface,
- the enclosure exterior,
- the programming and testing setup,
- the project operating in real time.

## Applications
This prototype can be used as a starting point for systems involving:
- water quality monitoring,
- automated chemical balancing,
- hydroponic nutrient control,
- educational demonstrations of sensor-based automation,
- Arduino process control projects.

## Future Improvements
Possible future improvements include:
- keypad input instead of serial input,
- improved calibration for more accurate pH readings,
- a stronger enclosure and cleaner internal wiring,
- data logging for pH history,
- automatic stop conditions and safety limits,
- integration with IoT monitoring or mobile alerts.

## Conclusion
This project demonstrates how Arduino can be used to build a basic automated pH control system by combining sensing, display output, and actuator control. It highlights practical applications of embedded systems in automation and shows how low-cost hardware can be used to solve real monitoring and control problems.

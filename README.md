# Smart Parking Barrier System
Smart parking barrier system using Arduino, ultrasonic sensors, servo motor and LCD display.

This project implements an automated parking barrier system using Arduino, ultrasonic sensors, a servo motor, an LCD display, and LED indicators. The system detects vehicles entering and exiting a parking area and automatically manages access based on the number of available parking slots.

The hardware setup includes an Arduino UNO as the main controller, two HC-SR04 ultrasonic sensors used to detect vehicles at the entry and exit points, a servo motor that controls the barrier, a 16×2 LCD display with I2C interface for user information, and three LEDs (red, yellow, green) used as a traffic light to indicate the system state.

The ultrasonic sensors measure the distance to objects using sound pulses. When the measured distance is below a defined threshold, the system considers that a vehicle is present. One sensor is placed before the barrier to detect vehicles that want to enter the parking area, while the second sensor confirms that a vehicle has passed the barrier or detects vehicles leaving the parking area.

The system maintains a counter representing the number of available parking spaces. When a vehicle approaches the entry sensor and there are available slots, the barrier opens using the servo motor. After the vehicle passes the barrier and is detected by the exit sensor, the number of available slots decreases and the barrier closes. If a vehicle exits the parking area, the process works in reverse and the number of available spaces increases.

The LCD display provides feedback to the driver. During normal operation it shows a welcome message and the number of remaining parking slots. If the parking becomes full, the system displays a “Parking Full” message and activates the yellow LED to warn the driver that entry is not allowed.

To ensure reliable operation, the system uses edge detection so that each vehicle is counted only once. The overall behavior of the system is controlled using a Finite State Machine (FSM) with four main states: IDLE, ENTRY_WAIT_EXIT, EXIT_WAIT_ENTRY, and WAIT_CLEAR. This approach prevents incorrect counting when vehicles move slowly or remain between sensors.

The red LED indicates that the barrier is closed, the green LED indicates that the barrier is open, and the yellow LED signals that the parking area is full.

A simulation of the project can be tested using the following link:
https://wokwi.com/projects/452503766661643265

Author:
Bianca Lavinia Iacob

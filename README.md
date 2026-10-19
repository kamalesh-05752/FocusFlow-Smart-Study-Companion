# FocusFlow – Smart Adaptive Study Companion

An Arduino Nano-based study companion designed to support structured study sessions and provide ambient-light awareness using a bare Light Dependent Resistor (LDR).

## Overview

Students often struggle to maintain consistent study routines because of distractions, irregular breaks, and unsuitable study environments.

FocusFlow combines a Pomodoro-style timer with ambient-light monitoring. The system uses an Arduino Nano to manage study sessions, display timer information on an OLED, and monitor surrounding light levels through an LDR voltage-divider circuit.

The project follows a human-centered Design Thinking approach: identify student needs, build a prototype, test its usability, and improve the design using feedback.

## Features

- Pomodoro-style focus and break timer
- OLED-based timer and status display
- Ambient-light measurement using a bare LDR
- Analog light sensing through Arduino Nano pin A0
- LED and buzzer notifications
- Adjustable light threshold
- Low-cost, accessible hardware design

*Features should be marked as completed only after they have been implemented and tested.*

## Hardware Requirements

- Arduino Nano
- Bare LDR (photoresistor)
- One 22 kΩ resistor
- 0.96-inch I2C OLED display
- Two push buttons
- LED and suitable current-limiting resistor
- Buzzer
- Breadboard and jumper wires

## LDR Circuit Connections

The LDR and 22 kΩ resistor form a voltage divider.

| Component | Arduino Nano connection |
|---|---|
| First LDR leg | 5V |
| Second LDR leg | A0 |
| First resistor leg | A0 |
| Second resistor leg | GND |

The analog reading generally increases as the ambient light increases with this arrangement. The threshold should be calibrated using actual readings.

## Pin Configuration

| Component | Pin |
|---|---|
| LDR voltage-divider output | A0 |
| OLED SDA | A4 |
| OLED SCL | A5 |
| Start button | D2 |
| Reset button | D3 |
| LED | D4 |
| Buzzer | D5 |

The OLED connections assume a standard I2C Arduino Nano setup. Confirm the wiring against your actual hardware.

## How It Works

1. The Arduino reads the analog voltage produced by the LDR circuit.
2. The reading is compared against a configurable light threshold.
3. The OLED can display the light status alongside the study timer.
4. The timer manages focus and break sessions.
5. LED and buzzer notifications provide feedback when programmed.

## Design Thinking and Novelty

The project focuses on improving the study environment rather than providing only a countdown timer.

The prototype combines study-session management with ambient-light awareness in one low-cost device. Its value will be evaluated through student feedback, light-sensor measurements, and usability testing.

## Software

- Arduino IDE
- Embedded C/C++ using Arduino libraries
- Adafruit SSD1306 and Adafruit GFX libraries, if used by the OLED sketch
- Wire library for I2C communication

## Testing and Results

Record the following after testing:

- LDR readings under bright and dim conditions
- Accuracy and stability of light-status detection
- Timer operation and button response
- OLED display behavior
- Buzzer and LED notification behavior
- Feedback from student users

Do not report performance figures until they have been measured.

## Limitations

- The LDR measures relative ambient light, not concentration or eye strain.
- A single LDR does not provide calibrated lux measurements without suitable calibration.
- The prototype is not a medical or eye-health device.
- The basic circuit does not automatically control a room lamp unless an appropriate driver and light source are added.

## Future Improvements

- Automatic study-lamp control using a suitable driver
- Session history and study analytics
- Adjustable focus and break durations
- Improved light-threshold calibration
- A computer dashboard for visualizing study sessions
- User testing and iterative design improvements

## License

This project can be released under the MIT License if you choose to allow reuse and modification with attribution.


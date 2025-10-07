# PV-Based Hover Gesture Recognition - Data Collection

This project demonstrates a proof-of-concept **hover gesture recognition system** using a **photovoltaic (PV) sensor** and the Arduino Nano BLE 33 Sense. The system captures analog readings from the PV sensor in real-time and logs them to an SD card, generating a dataset suitable for machine learning (ML) and deep learning (DL) model training.

The Arduino sketch used for data logging is:  
**`Arduino_voltage_logger_Nano_BLE_Sense_SD.ino`**

## Data Collection Protocol

- Gestures are performed naturally at a fixed speed.  
- Each gesture triggers a **data logging block** of 1000 analog samples from `A0`.  
- Each block is labeled with the performed gesture, creating a dataset for classification.

## Logging Indicator

- In the original sketch, the **built-in LED (pin 13)** indicated when logging was active.  
- **Updated version:** a dedicated **GPIO pin** is used instead of the LED. This allows the logging indicator to be monitored on an **oscilloscope** simultaneously with the PV panel’s **AC-coupled output voltage**, providing a clear visual confirmation of when data is being recorded.

## Gesture Plan

| Gesture                     | Number of Repetitions | Notes                                     |
|------------------------------|--------------------|------------------------------------------|
| Swipe Up                     | 10–20               | Vertical upward motion                    |
| Swipe Down                   | 10–20               | Vertical downward motion                  |
| Swipe Left                   | 10–20               | Horizontal leftward motion                |
| Swipe Right                  | 10–20               | Horizontal rightward motion               |
| Circle Clockwise             | 10–20               | Circular clockwise motion                 |
| Circle Counterclockwise      | 10–20               | Circular counterclockwise motion         |
| Hold Gesture                 | 10–20               | Sustained hover gesture for long-press   |

> **Note:** Each repetition generates a single 1000-sample data block. Even with only one participant, this dataset is sufficient to demonstrate **distinct patterns for ML/DL classification**.

## Dataset Use

- Each gesture is stored as a labeled block of samples.  
- Simple ML approaches (Random Forest, SVM) or DL models (1D CNN, LSTM) can be trained to classify gestures based on these time-series signals.  
- This shows that the system can reliably generate **distinct, labeled datasets** for gesture recognition in principle, even before multi-user studies.

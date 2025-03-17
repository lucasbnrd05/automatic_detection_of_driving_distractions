# Automatic Detection of Driving Distractions

## Overview
This project is an embedded system designed to detect driver distractions using multiple sensors and actuators. The system analyzes real-time data from various sensors to determine potential distractions and responds accordingly to alert the driver or take corrective action.

## Features
- **Real-time monitoring** of head movements, steering behavior, speed, and safe distance.
- **Multiple sensors** including gyroscope, steering wheel position sensor, speedometer, and ultrasonic distance sensor.
- **Automated alerts** via warning lights and display.
- **Automatic braking system** for critical situations.

## System Components
### Sensors
- **Gyroscope**: Measures head tilt on X and Y axes to detect drowsiness or distraction.
- **Steering Wheel Rotation Sensor**: Monitors erratic steering behaviors.
- **Speedometer**: Measures vehicle speed.
- **Distance Sensor**: Detects unsafe following distances.

### Actuators
- **Warning Lights**: Yellow and red lights for different risk levels.
- **Display**: Shows alerts and relevant data.
- **Automatic Brakes**: Engages at varying intensities based on risk level.

## Symptom Detection Logic
| Condition | Detection Interval | Criteria |
|-----------|-------------------|----------|
| **Head Tilt (S1)** | 400ms | X-axis > ±30° (drowsiness/distraction), Y-axis > ±30° (unless turning) |
| **Unsafe Distance (D1, D2, D3)** | 300ms | Distance < Safe Distance Threshold |
| **Sharp Turn (S2)** | 350ms | Steering angle changes > 20° at speeds > 40km/h |

## System Actions
| Condition | Action |
|-----------|--------|
| (S1 or S2) and D0 | Turn on Light 1 |
| (S1 and S2) and D0 | Turn on Light 2 |
| (S1 or S2) and D1 | Turn on Light 1 & Light 2, Brake Level 1 |
| (S1 or S2) and D2 | Turn on Light 1 & Light 2, Brake Level 2 |
| D3 | Brake Level 3 |

## Installation & Execution
### Prerequisites
- Raspberry Pi with necessary sensor connections
- GCC Compiler

### Build & Run
```bash
make C
sudo ./threads
```

## Authors
- Prof. Dr. Javier García Martín (Project Definition)
- Lucas BERNARDEAU (Implementation)
- Dina DIDOUCHE (Implementation)
- Nathan VANATHATHAN (Implementation)

## License
MIT License

---
This project aims to enhance driving safety by reducing distractions and improving reaction times in critical situations.



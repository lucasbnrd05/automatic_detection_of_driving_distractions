# 🚗 Automatic Detection of Driving Distractions


---

## 📜 Introduction
This project, developed for the "Embedded System Modelling" course (Prof. Dr. Javier García Martín), implements an embedded system for the **Automatic Detection of Driving Distractions**. The system aims to enhance road safety by identifying signs of driver inattention through various sensors and responding with appropriate alerts or actions.

## Our Website
🔗 [Visit the  Website](https://lucasbnrd05.github.io/autodistractdetect/index.html)


## 🎯 Project Objective
The primary goal is to detect a driver's lack of attention by analyzing data from vehicle-installed sensors. Upon detecting symptoms of distraction, the system will trigger actions designed to re-engage the driver and mitigate potential risks.

## 🛠️ System Architecture
The system is comprised of sensors for data acquisition, a central processing unit for analysis, and actuators for driver feedback and intervention.

### 📡 Sensors
-   📏 **Gyroscope**:
    *   **Function**: Detects head inclination on two axes (X: forward/backward, Y: left/right).
    *   **Range**: -90º to +90º for each axis. An upright head position is close to 0º.
    *   **Purpose**: Monitors head position for signs of drowsiness or distraction.
-   🔄 **Steering Wheel Rotation Sensor**:
    *   **Function**: Measures steering wheel angle via a DC converter.
    *   **Raw Range**: 0 to 1023.
    *   **Processed Range**: Converted to -180º (left) to +180º (right), with 0º for a straight position.
    *   **Purpose**: Detects erratic driving or sudden corrections indicative of inattention.
-   🚦 **Speedometer**:
    *   **Function**: Provides current vehicle speed via an A/D converter.
    *   **Raw Range**: 0 to 1023.
    *   **Processed Range**: Represents 0 to 140 km/h.
    *   **Purpose**: Provides speed context for other detections (e.g., safety distance, sharp turns).
-   🛰️ **Distance Sensor**:
    *   **Function**: Ultrasonic sensor at the front of the vehicle measuring distance to the preceding vehicle.
    *   **Range**: 1 to 100 meters.
    *   **Purpose**: Monitors safe following distance.

### ⚡ Actuators
-   💡 **Warning Lights**: Two LEDs (yellow and red) to indicate varying levels of risk.
-   📺 **Display**: A screen to present data and alerts to the driver and co-pilot.
-   🛑 **Automatic Brake Activation**: System capable of applying brakes at different intensity levels.

### 🧠 Processing Unit
-   A **Raspberry Pi** serves as the central processing unit, running the C code responsible for:
    *   Reading sensor data.
    *   Implementing symptom detection algorithms.
    *   Triggering actuators based on detected risks.
    *   Managing concurrent tasks (threads for each sensor, symptom analysis, etc.).

## ✨ Features
-   **Real-time Monitoring**: Continuous tracking of head movements, steering behavior, vehicle speed, and proximity to other vehicles.
-   **Multi-Sensor Data Analysis**: Sophisticated logic to interpret combined sensor data for accurate distraction detection.
-   **Graduated Alert System**: Visual alerts (lights, display) corresponding to the severity of the detected risk.
-   **Automated Intervention**: Automatic brake activation in critical, high-risk situations.
-   **Configurable Detection Parameters**: Specific thresholds and timing for symptom detection as per system requirements.

## 📊 Symptom Detection Logic
The system reads and analyzes sensor data to identify the following symptoms:

| Symptom             | Sensor(s) Used         | Polling Interval | Detection Criteria                                                                                                                                                                                             | State ID |
| :------------------ | :--------------------- | :--------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :------- |
| **Head Tilt**       | Gyroscope              | 400 ms           | X-axis tilt > +30° (forward) or < -30° (backward) for at least two consecutive readings. Y-axis tilt > +30° (right) or < -30° (left) for two consecutive readings, *unless* steering wheel is turned in the same direction. | S1       |
| **Safety Distance** | Distance Sensor, Speedometer | 300 ms           | Recommended Safety Distance (RSD) = (Speed_km/h / 10)². <br> - **D1 (INSECURE)**: Actual Distance < RSD. <br> - **D2 (IMPRUDENT)**: Actual Distance < 0.5 * RSD. <br> - **D3 (DANGER COLLISION)**: Actual Distance < 0.33 * RSD. <br> - **D0 (CORRECT)**: If none of the above. | D0-D3    |
| **Sharp Turn**      | Steering Wheel Sensor, Speedometer | 350 ms           | Difference > 20° between two consecutive steering wheel readings AND vehicle speed > 40 km/h.                                                                                                       | S2       |

*The "HEAD TILT" condition (S1) is reset when the head position is corrected.*
*Distance symptoms (D0-D3) change dynamically as the situation evolves.*

## 🚦 System Actions Logic
Symptoms are analyzed **every 200 ms** to determine necessary actions:

| Condition Triggering Action           | Action(s) Undertaken                                     |
| :------------------------------------ | :------------------------------------------------------- |
| (S1 or S2) **and** D0                 | Turn on Yellow Light (Light 1)                           |
| (S1 **and** S2) **and** D0              | Turn on Red Light (Light 2)                              |
| (S1 or S2) **and** D1                 | Turn on Yellow Light (Light 1) & Red Light (Light 2), Activate Brake Level 1 |
| (S1 or S2) **and** D2                 | Turn on Yellow Light (Light 1) & Red Light (Light 2), Activate Brake Level 2 |
| D3                                    | Activate Brake Level 3                                   |

*All actions are maintained until the corresponding risk situation disappears.*

## 📁 Project Structure

```text
.
├── README.md
├── code
│   ├── Makefile         // Compiles the source code
│   ├── devices.c        // Implementation of sensor/actuator interface functions
│   ├── devices.h        // Header for device functions
│   └── threads.c        // Main logic, thread management, symptom analysis
└── diagram
    ├── diagram_in_pdf   // Contains PDF versions of system diagrams
    │   ├── acd.pdf      // Activity Diagram
    │   ├── bdd_and_ibd  // Block Definition & Internal Block Diagrams
    │   │   ├── HeadAnalysis_ibd.pdf
    │   │   ├── SafetyAnalysis_ibd.pdf
    │   │   ├── SteeringAnalysis_ibd.pdf
    │   │   ├── SymptomAnalysis_ibd.pdf
    │   │   ├── bdd_electrical.pdf
    │   │   └── bdd_general.pdf
    │   ├── req.pdf      // Requirements Diagram
    │   ├── sqd.pdf      // Sequence Diagram
    │   ├── std.pdf      // State Diagram
    │   └── ucd.pdf      // Use Case Diagram
    └── project.qea      // Enterprise Architect project file
```

## 🖥️ Installation and Execution

### ✅ Prerequisites
-   **Hardware**: Raspberry Pi with appropriate connections for all specified sensors and actuators.
-   **Software**:
    -   Raspberry Pi OS (or compatible Linux distribution).
    -   GCC Compiler (GNU Compiler Collection) for C.
    -   `make` utility.

### 🛠️ Building the Project
1.  Navigate to the `code` directory:
    ```bash
    cd code
    ```
2.  Compile the project using the Makefile:
    ```bash
    make C
    ```
    *(Note: `make C` assumes 'C' is a specific target in your Makefile, often for cleaning and then building. If it's a standard build, `make` might suffice.)*

### 🚀 Running the System
Execute the compiled program:
```bash
sudo ./threads
```
Root privileges (sudo) are typically required for direct hardware access (e.g., GPIO pins for sensors/actuators) on the Raspberry Pi.

## 📚 Deliverables Overview

This project includes the following deliverables as per Assignment 3 requirements:

- Deliverable 3.1: Block Definition Diagram and Internal Block Diagrams developed in Enterprise Architect. (Found in diagram/project.qea and diagram/diagram_in_pdf/bdd_and_ibd/)

- Deliverable 3.2: Source code for the Raspberry Pi, implementing the system logic. (Found in code/)

- Deliverable 3.3 (Optional but included): State Diagram for the "Risk" task and "Symptoms" shared resource. (Found in diagram/diagram_in_pdf/std.pdf)

## 👥 Authors and Contribution

- Project Definition & Supervision: Prof. Dr. Javier García Martín

- System Design & Implementation:
    - Lucas
    - Dina
    - Nathan

---

🚀 This project endeavors to enhance driving safety by proactively detecting and mitigating risks associated with driver distraction.

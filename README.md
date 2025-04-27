LineFollower_PID
A Line Following Robot (LFR) with PID control using Arduino Nano, TB6612FNG motor driver, 2 N20 motors, 7-sensor analog IR array, a custom 3D-printed chassis, and a PCB managing all wiring (power, motors, buttons, LED) except the sensor array.
Line Following Robot (LFR) with PID Control

This project is a Line Following Robot (LFR) built using an **Arduino Nano, a custom 3D-printed chassis, and PID control for smooth and accurate line tracking.  
A custom PCB handles almost all wiring connections — including motors, motor driver, power (7805 voltage regulator), LED indicators, and two push buttons for calibration and run operations. The 7-sensor analog IR array connects directly to the Arduino Nano.

---

Components Used
- Arduino Nano
- TB6612FNG Motor Driver
- 2 × N20 Motors
- 1 × Caster Wheel
- 7-Sensor Analog IR Array
- Custom 3D-Printed Chassis
- Custom PCB
- 7805 Voltage Regulator (for 5V supply)
- 2 × Push Buttons (Calibrate and Run)
- 1 × LED Indicator
- Battery and Switch

---

Features
- Smooth and precise PID-based line following
- Lightweight and robust custom 3D-printed chassis
- Clean and modular wiring using a custom PCB
- 2 Push Buttons:
  - **Calibrate** sensors before starting
  - **Run** the robot after calibration
- LED status indicator
- Easy PID tuning for different tracks
- Supports modular sensor replacement

---

## 📁 Folder Structure
```plaintext
LFR_PID_Project/
│
├── Arduino_Code/
│   └── LFR_PID.ino
│
├── 3D_Models/
│   ├── chassis.stl
│   ├── motor_mount.stl
│   └── sensor_holder.stl
|
├── Schematics/
│    ├── lfr_pcb_schematic.fzz   <-- (Fritzing file here)
│
├── PID_Tuning/
│   └── PID_notes.md
│
├── Images/
│   ├── assembled_robot.jpg
│   ├── pcb_top_view.jpg
│   └── chassis_design.jpg
│
├── Videos/ 
│   └── line_following_demo.mp4
│
├── README.md
│
└── LICENSE

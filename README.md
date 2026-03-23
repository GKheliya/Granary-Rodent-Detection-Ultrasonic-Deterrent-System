# 🌾 Granary Rodent Detection & Ultrasonic Deterrent System

An Arduino Nano-based embedded system that detects rodent presence in grain storage facilities using **dual-sensor fusion** (PIR motion + vibration) and automatically activates a **frequency-sweeping ultrasonic deterrent** — a humane, automated alternative to poisons and traps.

> 📌 **Status:** Pre-Lab Complete — Circuit designed, code written & Proteus 9 simulation validated  
> 🎓 **Course:** EEB 444 — Electronic Experimental Design | University of Botswana, 2026  
> 👤 **Author:** Gopolang Kheliya (ID: 202202596)

---

## 📖 Overview

Post-harvest grain loss from rodent infestation is a significant challenge for smallholder farmers in sub-Saharan Africa. Traditional control methods — rodenticides, snap traps, physical exclusion — are either hazardous, labour-intensive, or ineffective over time.

This system addresses that gap with a **low-cost, automated, non-lethal solution**: when rodents are detected through a dual-sensor trigger, an ultrasonic buzzer emits a frequency-swept signal (22–32 kHz) that repels rodents without harming stored grain, humans, or the environment.

Proteus 9 simulation confirmed correct system behaviour across all three trigger conditions, with:
- ⚡ **Initial detection recorded within 50ms**
- 🔊 **Deterrent activation within 100ms**

---

## ✨ Key Features

- 🔍 **Dual-sensor detection** — PIR motion sensor + vibration sensor for reduced false positives
- 🎛️ **Smart trigger logic** — 3 configurable conditions before deterrent activates
- 🔊 **Frequency-sweeping deterrent** — 22–32 kHz sweep prevents rodent habituation
- 🔁 **Auto-reset** — 8-second detection window with automatic counter reset
- ❄️ **Cooldown mechanism** — 3-second cooldown between deterrent cycles
- 💡 **LED status indicators** — Green (armed), Red (active), Flashing (cooldown)
- 🖥️ **Serial monitor logging** — Real-time detection logs for debugging and monitoring
- 🌱 **Agriculture-focused** — Designed for rural Botswana grain storage contexts

---

## 🛠️ Hardware Components

| Component | Arduino Pin | Specifications |
|---|---|---|
| Arduino Nano | USB | Main microcontroller, 33 I/O pins |
| PIR Motion Sensor | Digital 4 | 2–3m detection range via infrared |
| Vibration Sensor | Digital 5 | Mechanical disturbance trigger |
| Piezo Buzzer | Digital 8 | 22–32 kHz ultrasonic sweep output |
| Green LED | Digital 6 | System armed indicator |
| Red LED | Digital 7 | Rodent detected / deterrent active indicator |

---

## 🧠 Detection Algorithm

The system uses a **dual-sensor event counting** approach with a sliding 8-second window. The deterrent only activates when one of three conditions is met — significantly reducing false positives from wind, non-rodent animals, or minor structural vibrations.

### Trigger Conditions

| Condition | Motion Events (D4) | Vibration Events (D5) |
|---|---|---|
| Condition 1 | ≥ 2 | 0 |
| Condition 2 | 0 | ≥ 2 |
| Condition 3 | 1 | 1 |

### System Flow

```
[ARMED] → Sensors monitor environment continuously
    ↓
[EVENT] → Motion or vibration detected → counter increments
    ↓
[CHECK] → Trigger condition met within 5-second detection window?
    ↓ YES                             ↓ NO
[ACTIVE] → Buzzer sweeps 22–32kHz    [WAIT] → Reset counters after 8s
    ↓         Red LED ON, Green OFF
[10 sec] → Deterrent runs for 10 seconds
    ↓
[COOLDOWN] → 3-second cooldown, LEDs flash alternately
    ↓
[RE-ARMED] → Green LED ON, system ready
```

---

## 💻 Source Code

The firmware is written in **C/C++** using the Arduino IDE and validated in **Proteus 9 Professional**.

```cpp
// ============================================
// GRAIN STORAGE RODENT DETECTION SYSTEM
// Dual Sensor: PIR Motion + Vibration Detection
// Author: Gopolang Kheliya | University of Botswana
// Course: EEB 444 | 2026
//
// Trigger Conditions:
//   - 2 Motion Events, OR
//   - 2 Vibration Events, OR
//   - 1 Motion + 1 Vibration Event
// ============================================

const int pirPin   = 4;   // PIR motion sensor output
const int vibPin   = 5;   // Vibration sensor output
const int piezoPin = 8;   // Ultrasonic buzzer
const int greenLED = 6;   // System armed indicator
const int redLED   = 7;   // Rodent detected indicator

const unsigned long activeTime       = 10000; // 10s deterrent
const unsigned long cooldownTime     = 3000;  // 3s cooldown
const unsigned long detectionWindow  = 5000;  // 5s trigger window
const unsigned long debounceTime     = 150;   // 150ms debounce
const unsigned long resetCounterTime = 8000;  // 8s counter reset

// Frequency sweep: 22kHz → 32kHz → 22kHz (prevents habituation)
int freq = 22000;
int freqDirection = 1;
```

> 📁 Full annotated source code in [`/src/rat_deterrent.ino`](src/rat_deterrent.ino)

---

## 🔬 Simulation Results (Proteus 9)

Validated in Proteus 9 Professional across all three trigger conditions:

| Test Case | Condition | Result |
|---|---|---|
| Single trigger | 1 motion or 1 vibration only | No activation — green LED stays on, counters reset after 8s |
| Dual same-sensor | 2 motion or 2 vibration within window | Deterrent activates — red LED on, buzzer sweeps 22–32 kHz |
| Mixed sensor | 1 motion + 1 vibration within window | Deterrent activates on second detection regardless of sensor order |

**Response times:**

| Event | Time |
|---|---|
| Initial detection | < 50ms |
| Trigger evaluation | < 100ms |
| Deterrent activation | < 100ms |
| Re-arm after cycle | 3s cooldown |

---

## 📁 Project Structure

```
granary-rodent-detection/
│
├── src/
│   └── rat_deterrent.ino          # Full Arduino sketch
│
├── docs/
│   ├── circuit_diagram.png        # Proteus schematic capture
│   ├── flowchart.png              # System detection flowchart
│   └── Mini_Project_Report.pdf    # Full EEB 444 project report
│
├── simulation/
│   └── proteus_project.pdsprj    # Proteus 9 simulation file
│
├── README.md
└── LICENSE
```

---

## 🚀 Getting Started

### Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) v1.8+ or Arduino IDE 2
- [Proteus 9 Professional](https://www.labcenter.com/) (for simulation only)
- Arduino Nano board
- PIR motion sensor module
- Vibration sensor module (SW-420 or similar)
- Piezo buzzer

### Running in Proteus

1. Open `simulation/proteus_project.pdsprj` in Proteus 9
2. Compile the Arduino sketch — this generates a `.hex` file
3. Load the `.hex` into the Nano's Program File field in Proteus
4. Connect a **virtual terminal** to view serial monitor output
5. Run the simulation and manually trigger PIR/vibration inputs to test

### Uploading to Hardware

```bash
git clone https://github.com/gkheliya/granary-rodent-detection.git
cd granary-rodent-detection
```

1. Open `src/rat_deterrent.ino` in Arduino IDE
2. Wire components per the pin table above
3. Set: `Tools → Board → Arduino Nano` | `Processor → ATmega328P`
4. Select your COM port and click **Upload**
5. Open **Serial Monitor** at 9600 baud to view live detection logs

---

## 🌍 Motivation

Developed for **EEB 444 — Electronic Experimental Design** at the **University of Botswana**, this project targets the real agricultural challenge of post-harvest grain loss affecting smallholder farmers across Botswana and sub-Saharan Africa.

| Method | Problem |
|---|---|
| Chemical rodenticides | Toxic to livestock, grain & non-target species; growing resistance |
| Snap traps | Labour-intensive, require baiting and resetting |
| Physical exclusion | High upfront cost; rats gnaw through sealed gaps |
| **This system** | ✅ Automated, non-lethal, low-cost, no chemicals |

---

## 🔮 Future Improvements

- [ ] Replace piezo buzzer with dedicated ultrasonic transducer for true >20 kHz output
- [ ] Add LCD display for real-time status and detection count
- [ ] Integrate SD card module for long-term event logging
- [ ] Add HC-SR04 ultrasonic distance sensor for improved detection range
- [ ] Explore solar power for off-grid rural deployment
- [ ] Field test in a live grain storage environment

---

## 📚 References

1. H. T. Sihotang and R. S. Simbolon, *"Design and Testing of an Energy-Saving Ultrasonic Rat Repeller Prototype for Open Agricultural Environments."*
2. P. Silakari et al., *"Smart Ultrasonic Insects & Pets Repeller for Farms & Inventories Purpose,"* ICACAT 2018. doi: 10.1109/ICACAT.2018.8933607
3. R. K. W. Schwarting and M. Wöhr, *"On the relationships between ultrasonic calling and anxiety-related behavior in rats,"* 2012. doi: 10.1590/S0100-879X2012007500038

---

## 👤 Author

**Gopolang Kheliya**  
BEng Electronics Engineering (Level 400)  
University of Botswana  
📧 gkheliya16@gmail.com

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

*Built with 💡 for the farmers of Botswana — affordable tech, real-world impact.*

# High-Voltage PV-Powered Gesture Detection System

## Introduction

Gesture recognition using photovoltaic (PV) panels has emerged as a promising approach for **self-powered human–machine interaction**. Prior work (e.g., *A Photovoltaic Light Sensor-Based Self-Powered Real-Time Hover Gesture Recognition System for Smart Home Control*, Almania et al., *Electronics*, 14(18), 3576) has demonstrated that PV modules can serve not only as energy harvesters but also as effective light sensors, enabling **shadow-based gesture detection** without additional sensing hardware.

This project builds on that foundation but introduces several unique innovations:

- **Extremely Large Semi-Transparent PV Panel**  
  Instead of a small solar cell, the system employs a **50% semitransparent PV module** that doubles as both the energy harvester and gesture-sensing surface. Its large form factor enables robust shadow detection over a wide interaction area.

- **Novel Coffee Table Application**  
  The PV panel is integrated into a **coffee table form factor**, making the entire surface interactive. Users can perform gestures directly above the table, which are detected in real time from the panel’s minute voltage fluctuations.

- **Simultaneous Energy Harvesting and Gesture Detection**  
  Unlike conventional PV-based gesture systems, this design not only senses shadows but also **charges a Li-ion supercapacitor** from the same panel. This ensures that the system remains **fully self-powered**, storing enough energy for continuous operation.

- **Integration Potential with Displays**  
  Because the panel is semi-transparent, it can in principle be mounted above **embedded displays or lighting**, enabling rich **interactive, visual feedback surfaces** powered entirely by ambient light.

### Initial PV Panel Characterization

Initial measurements with a multimeter under **660 lux mixed laboratory lighting** (LED plus filtered daylight through a modern south-facing window) showed that the PV panel, connected to the LTC3638 converter, produced:

- **Vout = 3.35 V**  
- **Isc = 3.89 mA**  

These readings confirm that the panel can reliably generate sufficient voltage and current to power the Arduino Nano 33 BLE Sense and provide a measurable AC signal for gesture detection.

Together, these innovations position the system as a step toward **interactive, energy-autonomous smart furniture**: surfaces that are at once **functional, aesthetic, and responsive**, with no external wiring or power supply required.

---

## 1. Project Overview

This project implements an **ultra-low power, energy-harvesting system** designed to detect shadow gestures or subtle environmental changes using a standard **Photovoltaic (PV) panel** as its primary sensor. The system measures minute **AC-coupled voltage fluctuations (ΔVPV)** present on the PV module’s output, while simultaneously harvesting energy to operate the **Arduino Nano 33 BLE Sense** microcontroller and charge the primary energy storage element.

### Core Objectives
- **Energy Harvesting**: Efficiently transfer power from the PV panel (≈70 V DC) to a 3.35 V system bus and storage element.  
- **Stable Storage**: Select an energy storage component that maximizes energy density without compromising the PV module’s high DC operating point.  
- **Signal Conditioning**: Isolate and amplify the small AC signal (±150 mV) riding on the high DC bus for accurate ADC measurement.  
- **Gesture Detection**: Use the amplified ΔVPV signal for robust, low-latency shadow gesture detection.  

---

## 2. Energy Harvesting and Storage Design

### 2.1 Component Selection Rationale
The system’s feasibility hinges on maintaining the PV panel at a high operating voltage (≈70 V) for signal integrity. An experimental comparison between **EDLC** and **Li-Ion supercapacitors** showed critical stability differences:

| Capacitor | C | PV Operating Voltage (Test) | Energy Stored (Emax) | Status |
|-----------|---|-----------------------------|--------------------|--------|
| 50 F EDLC | 50 F | Collapsed to ≈4.143 V | ≈120 J | ❌ Rejected |
| 30 F Li-Ion | 30 F | Maintained 69–73 V | ≈146 J | ✅ Selected |

#### Critical Finding: MPPT Instability
- **EDLC Failure**: Large capacitance + low ESR created a demanding load. During charge-up, the MPPT control loop in the DC-DC converter became unstable, collapsing PV voltage to ≈4.1 V.  
- **Li-Ion Success**: Higher ESR and complex electrochemical behavior presented a smoother load, allowing stable MPPT operation at 69–73 V.  

Selected component: **30 F Li-Ion Supercapacitor (e.g., Vinatech VEL08203R8306G)**.

### 2.2 Power Converter and MPPT Strategy
- **Converter**: LTC3638 Buck-Boost Converter.  
- **Function**: Tracks the PV module’s Maximum Power Point (MPP) by dynamically adjusting input impedance (Rt) via duty cycle (D).  
- Impedance Reflection: Rt = Ro / D²  
- Input capacitance requirement: ≥75 V capacitor at PV terminals to smooth switching current and stabilize MPPT sensing.

### 2.3 Li-Ion Supercapacitor Connection & Series Load Resistor
The **30 F Li-Ion supercapacitor** is connected to the **output of the buck converter** in **series with a resistor (Rload)**.  

**Purpose of Rload:**
1. **Limits Inrush Current**: Prevents excessive current when the capacitor is initially uncharged, avoiding voltage collapse and converter stress.  
2. **Maintains MPPT Stability**: Ensures smooth current draw, keeping PV panel near its MPP.  
3. **Smooths Voltage Transients**: Forms an RC network with the capacitor to reduce voltage spikes, critical for AC signal measurement.

**Effect on Charging Current:**
Charging current is governed by Icharge = (Vconverter_out - Vcap) / Rload.  
- **Smaller Rload** → higher current, faster charging, but risk of converter instability.  
- **Larger Rload** → lower current, safer operation, slower charging.  
- **Optimal Rload**: Small enough to charge efficiently, large enough to protect the system.

---

## 3. Shadow Gesture Signal Conditioning

### 3.1 Signal Characteristics
- ΔVPV (gesture shadow) = ±150 mV (≈300 mV p-p)  
- DC offset ≈70 V  
- ADC input target: centered around VCC/2 ≈ 1.675 V

### 3.2 Op-Amp Circuit (LT1496)
- AC-Coupled Inverting Amplifier  
- Components: R_in = 500 kΩ, R_f = 1.5 MΩ, R_A = 1 MΩ, R_B = 1 MΩ, C_block = 10 µF (≥75 V)  
- AC Gain = -R_f / R_in = -3  
- DC Offset = VCC * (R_B / (R_A + R_B)) ≈ 1.675 V  

### 3.3 Frequency Response
- Cutoff frequency: fc = 1 / (2π * R_in * C_block) ≈ 0.0318 Hz  
- Preserves AC signals (0.1–10 Hz) while filtering slow drifts.

---

## 4. Theoretical Considerations

### 4.1 Li-Ion Supercapacitor Dynamics
- Battery-like intercalation on one electrode  
- Charging: relatively linear voltage rise to ≈3.8 V  
- Discharging: gradual voltage decline → higher usable energy density than EDLC  
- ESR softens inrush current → stable PV voltage

### 4.2 PV Panel Operating Point
- Load line: V = I * R_L  
- MPP occurs near "knee" of I–V curve  
- R_L too low → current-limited, R_L too high → voltage-limited  
- MPPT ensures converter impedance matches PV source impedance

### 4.3 Buck Converter Input Impedance
- R_in ≈ D² * R_load  
- Duty cycle D adjusts R_in to maintain PV MPP  
- Low R_in can collapse PV voltage (seen with EDLCs)

---

## 5. Microcontroller Power Management & TinyML

### Arduino Nano 33 BLE Sense
- Active: 4–6 mA @ 3.3 V  
- Deep sleep: 1–3 µA, wake via external interrupts (e.g., PIR)  
- MCU mostly in deep sleep, waking only for gesture capture

### TinyML Deployment
- TFLM or similar engines for compact models (~10–20 gestures, tens of kB)  
- Fits within 1 MB Flash / 256 kB RAM  
- Enables on-device inference, maintaining self-powered operation

---

## Summary
This system demonstrates a **high-voltage, energy-harvesting PV sensor** capable of:  
- Maintaining **stable high-voltage operation** via Li-Ion supercapacitor  
- Extracting and amplifying **minute AC fluctuations** for gesture detection  
- Operating entirely off **harvested solar energy**  
- Leveraging **Li-ion charge dynamics, PV load-line analysis, and buck converter impedance** for robust design  
- Novel **semi-transparent coffee table** application with self-powered interactivity  
- Aggressive MCU power management and **TinyML** for autonomous operation




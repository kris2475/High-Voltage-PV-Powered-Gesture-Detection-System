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

This project implements an **ultra-low power, energy-harvesting system** designed to detect shadow gestures or subtle environmental changes using a standard **Photovoltaic (PV) panel** as its primary sensor.  

The system measures minute **AC-coupled voltage fluctuations (ΔV<sub>PV</sub>)** present on the PV module’s output, while simultaneously harvesting energy to operate the **Arduino Nano 33 BLE Sense** microcontroller and charge the primary energy storage element.

### Core Objectives
- **Energy Harvesting**: Efficiently transfer power from the PV panel (≈70 V DC) to a 3.35 V system bus and storage element.  
- **Stable Storage**: Select an energy storage component that maximizes energy density without compromising the PV module’s high DC operating point.  
- **Signal Conditioning**: Isolate and amplify the small AC signal (±150 mV) riding on the high DC bus for accurate ADC measurement.  
- **Gesture Detection**: Use the amplified ΔV<sub>PV</sub> signal for robust, low-latency shadow gesture detection.  

---

## 2. Energy Harvesting and Storage Design

### 2.1 Component Selection Rationale
The system’s feasibility hinges on maintaining the PV panel at a high operating voltage (≈70 V) for signal integrity.  
An experimental comparison between **EDLC** and **Li-Ion supercapacitors** showed critical stability differences:

| Capacitor | C | PV Operating Voltage (Test) | Energy Stored (E<sub>max</sub>) | Status |
|-----------|---|-----------------------------|---------------------------------|--------|
| 50 F EDLC | 50 F | Collapsed to ≈4.143 V | ≈120 J | ❌ Rejected |
| 30 F Li-Ion | 30 F | Maintained 69–73 V | ≈146 J | ✅ Selected |

#### Critical Finding: MPPT Instability
- **EDLC Failure**: Large capacitance + low ESR created a demanding load. During charge-up, the MPPT control loop in the DC-DC converter became unstable, collapsing PV voltage to ≈4.1 V (insufficient for operation).  
- **Li-Ion Success**: Higher ESR and complex electrochemical behavior presented a smoother load, allowing stable MPPT operation at 69–73 V.  

Selected component: **30 F Li-Ion Supercapacitor (e.g., Vinatech VEL08203R8306G)**.

---

### 2.2 Power Converter and MPPT Strategy
- **Converter**: LTC3638 Buck-Boost Converter.  
- **Function**: Tracks the PV module’s **Maximum Power Point (MPP)** by dynamically adjusting input impedance (R<sub>t</sub>) via duty cycle (D).  

**Impedance Reflection (Buck Reference):**

R_t = R_o / D^2

- **Input Capacitance Requirement**:  
  - ≥75 V capacitor placed at PV terminals.  
  - Acts as an energy buffer, smoothing discontinuous switching current and stabilizing MPPT sensing.  

---

## 3. Shadow Gesture Signal Conditioning

### 3.1 Signal Characteristics and Constraints
- **Input Signal**: ΔV<sub>PV</sub> (gesture shadow) = ±150 mV (≈300 mV p-p).  
- **DC Offset**: Rides on ≈70 V bias.  
- **ADC Target**: Output centered around V<sub>CC</sub>/2 (≈1.675 V), Arduino Nano 33 BLE Sense (V<sub>CC</sub> ≈ 3.35 V).  

---

### 3.2 Op-Amp Circuit (LT1496)
Configured as **AC-Coupled Inverting Amplifier**:

| PCB Component (Netlist) | Value | Role |
|--------------------------|-------|------|
| Op-Amp | LT1496 | Low-power, rail-to-rail output amplifier |
| R<sub>in</sub> (R_LT1496) | 500 kΩ | Inverting input resistor |
| R<sub>f</sub> (RF_LT1496) | 1.5 MΩ (Trimpot Max) | Feedback resistor (sets gain) |
| R<sub>A</sub> (RDC_LT1496) | 2 MΩ (set to 1.0 MΩ) | Upper bias resistor |
| R<sub>B</sub> (RDIV_LT1496) | 1 MΩ | Lower bias resistor |
| C<sub>block</sub> (C_DC_LT1496) | 10 µF (≥75 V) | DC blocking capacitor |

---

### 3.3 DC Blocking Capacitor & Frequency Response
- **C<sub>block</sub> = 10 µF** → forms high-pass filter with R<sub>in</sub>.  

**Cutoff Frequency:**

f_c = 1 / (2π * R_in * C_block)

Using R<sub>in</sub> = 500 kΩ and C<sub>block</sub> = 10 µF:

f_c ≈ 0.0318 Hz

- **Impact**:  
  - Attenuates long-term DC drifts (temperature, slow irradiance).  
  - Preserves AC signals (0.1 Hz–10 Hz) for gesture detection.  

---

### 3.4 Optimized Trimpot Configuration
- **AC Gain (A_v):**

A_v = -R_f / R_in  
A_v = -1.5 MΩ / 500 kΩ = -3  

Input: 300 mV p-p → Output: 0.9 V p-p.  

- **DC Offset (V_REF):**

V_REF = V_CC * (R_B / (R_A + R_B))  
V_REF = V_CC / 2 ≈ 1.675 V  

✅ Ensures amplified signal is **centered and scaled** for ADC conversion.  

---

## 4. Theoretical Considerations

### 4.1 Li-Ion Supercapacitor Charge/Discharge Cycle
- Li-ion hybrid capacitors differ from EDLCs in that they use **battery-like intercalation** on one electrode.  
- **Charging phase**: Voltage rises relatively linearly until ≈3.8 V, with a mild plateau due to faradaic reactions.  
- **Discharging phase**: Provides a more gradual voltage decline, offering **higher usable energy density** than EDLCs.  
- **Cycle Stability**: Operates between 2.2–3.8 V; exceeding these limits reduces cycle life.  
- Advantage in this system: naturally higher ESR softens inrush current, preventing PV voltage collapse.  

---

### 4.2 PV Panel Operating Point & Resistive Load Line
The operating point of a PV module is determined by the intersection of its **I–V characteristic curve** and the **load line**:  

- Load resistance R<sub>L</sub> defines a straight line on the I–V plane:  

V = I × R_L  

- The Maximum Power Point (MPP) occurs near the “knee” of the I–V curve where:  

P = V × I is maximized.  

- If R<sub>L</sub> is too low → panel is current-limited (low voltage, high current).  
- If R<sub>L</sub> is too high → panel is voltage-limited (high voltage, low current).  
- A properly tuned MPPT ensures the **reflected input impedance of the converter** matches the PV source impedance at MPP.  

---

### 4.3 Input Impedance of a Buck Converter
For a buck converter operating in continuous conduction:  

R_in ≈ (D² × R_load)  

where:  
- D = duty cycle,  
- R_load = effective resistance seen at the output.  

Implications:  
- As D increases, R_in decreases quadratically.  
- MPPT controllers adjust D to reflect the correct R_in onto the PV panel so it operates at MPP.  
- If the converter + storage element demand too much current (i.e., R_in too small), the PV voltage collapses — which explains the instability seen with EDLCs.  

---

## 📌 Summary
This system demonstrates a **high-voltage, energy-harvesting PV sensor** capable of:  
- Maintaining **stable high-voltage operation** via Li-Ion supercapacitor storage.  
- Extracting and amplifying **minute AC fluctuations** for shadow gesture detection.  
- Operating entirely off **harvested solar energy** with ultra-low-power circuitry.  
- Leveraging theoretical insights from **Li-ion charge dynamics, PV load-line analysis, and buck converter input impedance** for robust design.  
- Exploring a novel **semi-transparent coffee table application**, combining **gesture interactivity** with **self-powered autonomy** and future potential for **under-panel displays**.


 


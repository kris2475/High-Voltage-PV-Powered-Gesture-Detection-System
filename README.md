# High-Voltage PV-Powered Gesture Detection System

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
\[
R_t = \frac{R_o}{D^2}
\]

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
- **Cutoff Frequency**:  
\[
f_c = \frac{1}{2 \pi R_{in} C_{block}} \approx 0.0318 \, \text{Hz}
\]

- **Impact**:  
  - Attenuates long-term DC drifts (temperature, slow irradiance).  
  - Preserves AC signals (0.1 Hz–10 Hz) for gesture detection.  

---

### 3.4 Optimized Trimpot Configuration
- **AC Gain (A<sub>v</sub>)**:  
  - R<sub>f</sub> = 1.5 MΩ, R<sub>in</sub> = 500 kΩ → A<sub>v</sub> = −3.  
  - Input: 300 mV p-p → Output: 0.9 V p-p.  

- **DC Offset (V<sub>REF</sub>)**:  
  - R<sub>A</sub> = 1.0 MΩ, R<sub>B</sub> = 1.0 MΩ.  
  - V<sub>REF</sub> = V<sub>CC</sub>/2 = 1.675 V.  

✅ Ensures amplified signal is **centered and scaled** for ADC conversion.  

---

## 📌 Summary
This system demonstrates a **high-voltage, energy-harvesting PV sensor** capable of:  
- Maintaining **stable high-voltage operation** via Li-Ion supercapacitor storage.  
- Extracting and amplifying **minute AC fluctuations** for shadow gesture detection.  
- Operating entirely off **harvested solar energy** with ultra-low-power circuitry.  

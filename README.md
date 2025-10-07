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

### Initial PV Panel Characterisation

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
- **Stable Storage**: Select an energy storage component that maximises energy density without compromising the PV module’s high DC operating point.  
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
- **Li-Ion Success**: Higher ESR and complex electrochemical behaviour presented a smoother load, allowing stable MPPT operation at 69–73 V.  

Selected component: **30 F Li-Ion Supercapacitor (e.g., Vinatech VEL08203R8306G)**.

---

### 2.2 Power Converter and MPPT Strategy
- **Converter**: LTC3638 Buck-Boost Converter.  
- **Function**: Tracks the PV module’s **Maximum Power Point (MPP)** by dynamically adjusting input impedance (R<sub>t</sub>) via duty cycle (D).  

**Impedance Reflection (Buck Reference):**

R_t = R_o / D^2

- **Input Capacitance Requirement**:  
  - ≥75 V capacitor placed at PV terminals.  
  - Acts as an energy buffer, smoothing discontinuous switching current and stabilising MPPT sensing.  

---

### 2.3 Li-Ion Supercapacitor Connection & Series Load Resistor
The **30 F Li-Ion supercapacitor** is connected at the **output of the buck converter** in **series with a resistor (R<sub>load</sub>)**.

#### Why R<sub>load</sub> is Needed
1. **Limits Inrush Current**  
   - When the supercapacitor is initially uncharged, it behaves almost like a short circuit.  
   - R<sub>load</sub> limits the inrush current from the buck converter, preventing **voltage collapse** or stress on the converter’s switching elements.

2. **Maintains MPPT Stability**  
   - Excessive instantaneous current can destabilise the Maximum Power Point Tracking (MPPT) loop.  
   - The series resistor ensures the converter output rises smoothly, keeping the PV panel near its MPP.

3. **Smooths Voltage Transients**  
   - The R<sub>load</sub> + supercapacitor combination forms a simple **RC damping network**, reducing voltage spikes and ripples, which is critical for accurate AC-coupled gesture detection.

#### How R<sub>load</sub> Affects Charging Current
The series resistor directly determines the **charging current**:

I_charge = (V_out_converter - V_SC) / R_load

- **Smaller R<sub>load</sub>** → Higher charging current, faster charging, but risk of inrush and converter stress.  
- **Larger R<sub>load</sub>** → Lower charging current, safer operation, but slower energy accumulation.  
- **Optimal Choice:** Small enough to allow efficient charging but large enough to **limit inrush current** and maintain stable converter operation.  

---

## 3. Shadow Gesture Signal Conditioning

### 3.1 Signal Characteristics and Constraints
- **Input Signal**: ΔV<sub>PV</sub> (gesture shadow) = ±150 mV (≈300 mV p-p).  
- **DC Offset**: Rides on ≈70 V bias.  
- **ADC Target**: Output centred around V<sub>CC</sub>/2 (≈1.675 V), Arduino Nano 33 BLE Sense (V<sub>CC</sub> ≈ 3.35 V).  

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

### 3.4 Optimised Trimpot Configuration
- **AC Gain (A_v):**

A_v = -R_f / R_in  
A_v = -1.5 MΩ / 500 kΩ = -3  

Input: 300 mV p-p → Output: 0.9 V p-p.  

- **DC Offset (V_REF):**

V_REF = V_CC * (R_B / (R_A + R_B))  
V_REF = V_CC / 2 ≈ 1.675 V  

✅ Ensures amplified signal is **centred and scaled** for ADC conversion.  

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

P = V × I is maximised.  

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

## 5. Microcontroller Power Management & ML/DL Feasibility

### Arduino Nano 33 BLE Sense Current Consumption
The Arduino Nano 33 BLE Sense is based on the **nRF52840 SoC**, which offers multiple low-power operating modes:

- **Active Mode** (CPU running, peripherals active):  
  ~4–6 mA typical at 3.3 V during sampling and logging.  

- **Deep Sleep / System OFF Mode**:  
  ~1–3 µA typical, with wake-up sources such as external interrupts (e.g., PIR sensor).  

In this project, the Nano 33 BLE Sense is intended to remain in **deep sleep** most of the time. It will be woken by a **PIR motion detector** (or another low-power external trigger) just prior to gesture sensing. Once the gesture capture is complete and data is logged, the MCU returns to deep sleep, minimising average energy consumption.

### TinyML Model Deployment
Any trained **gesture recognition model** (e.g., Random Forest, 1D CNN, or LSTM) is designed to be compact enough for **TinyML deployment** on the Nano 33 BLE Sense.  

- **TensorFlow Lite for Microcontrollers (TFLM)** or similar lightweight inference engines can run efficiently within the device’s memory (1 MB Flash, 256 kB RAM).  
- The expected model size for classifying simple gestures (≈10–20 repetitions per gesture) is **tens of kilobytes**, easily fitting within






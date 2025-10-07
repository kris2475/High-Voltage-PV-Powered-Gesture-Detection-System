# High-Voltage PV-Powered Gesture Detection System

## Introduction
Gesture recognition using photovoltaic (PV) panels has emerged as a promising approach for self-powered human–machine interaction. Prior work (e.g., *A Photovoltaic Light Sensor-Based Self-Powered Real-Time Hover Gesture Recognition System for Smart Home Control*, Almania et al., *Electronics*, 14(18), 3576) has demonstrated that PV modules can serve not only as energy harvesters but also as effective light sensors, enabling shadow-based gesture detection without additional sensing hardware.

This project builds on that foundation but introduces several unique innovations:

### Extremely Large Semi-Transparent PV Panel
Instead of a small solar cell, the system employs a 50% semitransparent PV module that doubles as both the energy harvester and gesture-sensing surface. Its large form factor enables robust shadow detection over a wide interaction area.

### Novel Coffee Table Application
The PV panel is integrated into a coffee table form factor, making the entire surface interactive. Users can perform gestures directly above the table, which are detected in real time from the panel’s minute voltage fluctuations.

### Simultaneous Energy Harvesting and Gesture Detection
Unlike conventional PV-based gesture systems, this design not only senses shadows but also charges a Li-ion supercapacitor from the same panel. This ensures that the system remains fully self-powered, storing enough energy for continuous operation.

### Integration Potential with Displays
Because the panel is semi-transparent, it can in principle be mounted above embedded displays or lighting, enabling rich interactive, visual feedback surfaces powered entirely by ambient light.

## Initial PV Panel Characterisation
Initial measurements with a multimeter under 660 lux mixed laboratory lighting (LED plus filtered daylight) with the PV panel connected to the LTC3638 converter showed:

$$
V_{out} = 3.35 \ \text{V}
$$

$$
I_{sc} = 3.89 \ \text{mA}
$$

These readings confirm that the panel can reliably generate sufficient voltage and current to power the Arduino Nano 33 BLE Sense and provide a measurable AC signal for gesture detection.

Together, these innovations position the system as a step toward interactive, energy-autonomous smart furniture: surfaces that are functional, aesthetic, and responsive, with no external wiring or power supply required.

---

## 1. Project Overview
This project implements an ultra-low power, energy-harvesting system designed to detect shadow gestures or subtle environmental changes using a standard PV panel as its primary sensor.

The system measures minute AC-coupled voltage fluctuations ($\Delta V_{PV}$) present on the PV module’s output, while simultaneously harvesting energy to operate the Arduino Nano 33 BLE Sense microcontroller and charge the primary energy storage element.

### Core Objectives
- **Energy Harvesting:** Efficiently transfer power from the PV panel (≈70 V DC) to a 3.35 V system bus and storage element.
- **Stable Storage:** Select an energy storage component that maximises energy density without compromising the PV module’s high DC operating point.
- **Signal Conditioning:** Isolate and amplify the small AC signal (±150 mV) riding on the high DC bus for accurate ADC measurement.
- **Gesture Detection:** Use the amplified $\Delta V_{PV}$ signal for robust, low-latency shadow gesture detection.

---

## 2. Energy Harvesting and Storage Design

### 2.1 Component Selection Rationale
The system’s feasibility hinges on maintaining the PV panel at a high operating voltage (≈70 V) for signal integrity. An experimental comparison between EDLC and Li-Ion supercapacitors showed critical stability differences:

| Capacitor | C | PV Operating Voltage (Test) | Energy Stored ($E_{max}$) | Status |
|-----------|---|-----------------------------|---------------------------|--------|
| 50 F EDLC | 50 F | Collapsed to ≈4 V | ≈120 J | ❌ Rejected |
| 30 F Li-Ion | 30 F | Maintained 69–73 V | ≈146 J | ✅ Selected |

#### Critical Finding: MPPT Instability

**EDLC Failure (Root Cause Analysis):**  
The 50 F EDLC supercapacitor caused the PV voltage to collapse to about 4 V throughout the charging cycle.  

- **Severe Load-Induced Overcurrent:**  
  The MPPT controller exceeded the maximum power current ($I_{MPP}$) of the PV panel, forcing its voltage down into the current-limited region near $I_{SC}$.  

- **Control Loop Instability:**  
  Very low ESR and high capacitance shifted the ESR zero close to the control loop crossover frequency, destabilising the converter and reinforcing the collapse.

**Li-Ion Success:**  
Higher ESR and battery-like behavior created a smoother, resistive-like load profile. This allowed stable MPPT operation at 69–73 V.

**Selected Component:** 30 F Li-Ion Supercapacitor (e.g., Vinatech VEL08203R8306G).

---

### 2.2 Power Converter and MPPT Strategy
- **Converter:** LTC3638 Buck-Boost Converter
- **Function:** Tracks PV Maximum Power Point (MPP) by adjusting input impedance ($R_t$) via duty cycle ($D$).

$$
R_t = D^2 \cdot R_o
$$

- **Input Capacitance Requirement:**  
  - ≥75 V capacitor at PV terminals to buffer energy, smooth switching current, and stabilise MPPT sensing.

---

### 2.3 Li-Ion Supercapacitor Connection & Series Load Resistor
The 30 F Li-Ion supercapacitor is connected at the buck converter output in series with $R_{load}$.

**Charging Current:**

$$
I_{charge} = \frac{V_{out\_converter} - V_{SC}}{R_{load}}
$$

Optimal $R_{load}$ balances fast charging with stable converter operation.

---

## 3. Shadow Gesture Signal Conditioning

### 3.1 Signal Characteristics
- Input: $\Delta V_{PV} = \pm 150\text{ mV} \, (\approx 300\text{ mV p-p})$
- DC Offset: ≈70 V
- ADC Target: Output centred at $V_{CC}/2 \approx 1.675\text{ V}$, Arduino Nano 33 BLE Sense ($V_{CC} \approx 3.35\text{ V}$).

---

### 3.2 Op-Amp Circuit (LT1496)
Configured as AC-Coupled Inverting Amplifier:

| Component | Value | Role |
|-----------|-------|------|
| Op-Amp | LT1496 | Low-power, rail-to-rail output amplifier |
| $R_{in}$ | 500 kΩ | Inverting input resistor |
| $R_f$ | 1.5 MΩ (Trimpot Max) | Feedback resistor (sets gain) |
| $R_A$ | 2 MΩ (set to 1.0 MΩ) | Upper bias resistor |
| $R_B$ | 1 MΩ | Lower bias resistor |
| $C_{block}$ | 10 μF (≥75 V) | DC blocking capacitor |

---

### 3.3 DC Blocking Capacitor & Frequency Response

$$
f_c = \frac{1}{2 \pi R_{in} C_{block}}
$$

Using $R_{in} = 500 \,\text{kΩ}$ and $C_{block} = 10 \,\mu\text{F}$:

$$
f_c \approx 0.0318 \,\text{Hz}
$$

---

### 3.4 Optimised Trimpot Configuration
**AC Gain:**

$$
A_v = -\frac{R_f}{R_{in}} = -\frac{1.5 \,\text{MΩ}}{500 \,\text{kΩ}} = -3
$$

Input 300 mV p-p → Output 0.9 V p-p.

**DC Offset:**

$$
V_{REF} = V_{CC} \cdot \frac{R_A}{R_A + R_B} \approx 1.675 \,\text{V}
$$

---

## 4. Theoretical Considerations

### 4.1 Li-Ion Supercapacitor Charge/Discharge
- Charging: Voltage rises linearly to ≈3.8 V with mild plateau.
- Discharging: Gradual voltage decline, higher usable energy than EDLCs.
- Cycle Stability: Operates between 2.2–3.8 V, higher ESR prevents PV collapse.

### 4.2 PV Panel Operating Point & Resistive Load Line

$$
V = I \cdot R_L
$$

- MPP occurs near the "knee" of the I–V curve.
- Proper MPPT ensures converter input impedance matches PV source impedance at MPP.

### 4.3 Input Impedance of a Buck Converter

$$
R_{in} \approx D^2 \cdot R_{load}
$$

If $R_{in}$ is too small → PV voltage collapses (explains EDLC instability).

---

## 5. Microcontroller Power Management & ML/DL Feasibility

### Arduino Nano 33 BLE Sense Current Consumption
- **Active Mode:** ∼4–6 mA at 3.3 V during sampling.
- **Deep Sleep:** ∼1–3 μA, woken by PIR or external trigger.

### TinyML Model Deployment
- Models (Random Forest, 1D CNN, LSTM) compact enough for TinyML.
- TensorFlow Lite for Microcontrollers (TFLM) or similar fits in 1 MB Flash, 256 kB RAM.
- Expected model size: tens of kilobytes, sufficient for on-device inference.

---

## 📌 Summary
This system demonstrates a high-voltage, energy-harvesting PV sensor capable of:

- Maintaining stable high-voltage operation via Li-Ion supercapacitor.
- Extracting and amplifying minute AC fluctuations for shadow gesture detection.
- Operating entirely off harvested solar energy with ultra-low-power circuitry.
- Leveraging theoretical insights from Li-ion charge dynamics, PV load-line analysis, and buck converter input impedance.
- Exploring a novel semi-transparent coffee table application with future potential for under-panel displays.
- Incorporating aggressive MCU power management (deep sleep with PIR wake) and TinyML deployment, ensuring practical, scalable self-powered interaction.







# High-Voltage PV-Powered Gesture Detection System

## Introduction
Gesture recognition using photovoltaic (PV) panels has emerged as a promising approach for self-powered human–machine interaction. Prior work (e.g., A Photovoltaic Light Sensor-Based Self-Powered Real-Time Hover Gesture Recognition System for Smart Home Control, Almania et al., Electronics, 14(18), 3576) has demonstrated that PV modules can serve not only as energy harvesters but also as effective light sensors, enabling shadow-based gesture detection without additional sensing hardware.

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
Initial measurements with a multimeter under 660 lux mixed laboratory lighting (LED plus filtered daylight through a modern south-facing window) showed that the PV panel, connected to the LTC3638 converter, produced:

$$
V_{out} = 3.35 \,\text{V}
$$

$$
I_{sc} = 3.89 \,\text{mA}
$$

These readings confirm that the panel can reliably generate sufficient voltage and current to power the Arduino Nano 33 BLE Sense and provide a measurable AC signal for gesture detection.

Together, these innovations position the system as a step toward interactive, energy-autonomous smart furniture: surfaces that are at once functional, aesthetic, and responsive, with no external wiring or power supply required.

---

## 1. Project Overview
This project implements an ultra-low power, energy-harvesting system designed to detect shadow gestures or subtle environmental changes using a standard Photovoltaic (PV) panel as its primary sensor.

The system measures minute AC-coupled voltage fluctuations ($\Delta V_{PV}$) present on the PV module’s output, while simultaneously harvesting energy to operate the Arduino Nano 33 BLE Sense microcontroller and charge the primary energy storage element.

### Core Objectives
- **Energy Harvesting:** Efficiently transfer power from the PV panel (≈70 V DC) to a 3.35 V system bus and storage element.  
- **Stable Storage:** Select an energy storage component that maximises energy density without compromising the PV module’s high DC operating point.  
- **Signal Conditioning:** Isolate and amplify the small AC signal (±150 mV) riding on the high DC bus for accurate ADC measurement.  
- **Gesture Detection:** Use the amplified $\Delta V_{PV}$ signal for robust, low-latency shadow gesture detection.  

---

## 2. Energy Harvesting and Storage Design

### 2.1 Component Selection Rationale
The system’s feasibility hinges on maintaining the PV panel at a high operating voltage (≈70 V) for signal integrity.
An experimental comparison between EDLC and Li-Ion supercapacitors showed critical stability differences:

| Capacitor | C   | PV Operating Voltage (Test) | Energy Stored ($E_{max}$) | Status     |
|-----------|-----|-----------------------------|---------------------------|------------|
| 50 F EDLC | 50 F | Collapsed to ≈4 V           | ≈120 J                    | ❌ Rejected |
| 30 F Li-Ion | 30 F | Maintained 69–73 V         | ≈146 J                    | ✅ Selected |

#### Critical Finding: MPPT Instability
**EDLC Failure (Root Cause Analysis):** The 50 F EDLC supercapacitor caused the PV voltage to collapse to about 4 V **throughout the charging cycle**.  

- **Severe Load-Induced Overcurrent:** The EDLC's large capacitance and very low ESR present an instantaneous, demanding load (a near-short circuit demand) to the buck converter's output, especially when nearly discharged. The MPPT controller attempts to satisfy this demand by drawing a correspondingly high current from the PV input. This reflected current demand exceeds the maximum power current ($I_{MPP}$) of the PV panel, forcing its operating point down the I–V curve into the current-limited region near $I_{SC}$. This voltage collapse is sustained throughout the charge cycle, as the demanding load cannot be met at the high-voltage MPP.  

- **Control Loop Instability:** The combination of very low ESR and large capacitance in the EDLC shifts the frequency of the ESR zero in the converter's power stage transfer function to a very low frequency. If this low-frequency zero falls too close to the control loop's crossover frequency, it drastically reduces the phase margin. An unstable control loop cannot settle, causing the MPPT to oscillate or latch into a low-impedance state, reinforcing the collapse and preventing recovery to the high ≈70 V operating point.  

**Why this happens (Simplified):** The EDLC's ideal capacitor behaviour provides too perfect a sink during charging. This immediate, high-current demand reflected back to the PV panel overloads it so heavily that its operating voltage is instantly dragged down past the Maximum Power Point (MPP) into the current-limited region, forcing the PV panel into a low-voltage collapse that the MPPT controller cannot overcome.  

**Additional perspective:** In a practical system, energy harvesters and MPPT controllers require a load that behaves somewhat resistively across the charge cycle. EDLCs deny the system that buffer, forcing the converter to oscillate or collapse.  

**Li-Ion Success:** Higher ESR and complex electrochemical behaviour presented a smoother, more resistive-like load profile across the entire charge cycle. The Li-ion hybrid capacitor tapers current more gracefully, avoiding a sudden discontinuity when approaching full voltage. This allowed stable MPPT operation to be maintained at 69–73 V, even when the capacitor was near full charge.  

**Selected component:** 30 F Li-Ion Supercapacitor (e.g., Vinatech VEL08203R8306G).  

---

### 2.2 Power Converter and MPPT Strategy
- **Converter:** LTC3638 Buck-Boost Converter.  
- **Function:** Tracks the PV module’s Maximum Power Point (MPP) by dynamically adjusting input impedance ($R_t$) via duty cycle ($D$).  

$$
R_t = D^2 \cdot R_o
$$

- **Input Capacitance Requirement:**  
  - ≥75 V capacitor placed at PV terminals.  
  - Acts as an energy buffer, smoothing discontinuous switching current and stabilising MPPT sensing.  

---

### 2.3 Li-Ion Supercapacitor Connection & Series Load Resistor
The 30 F Li-Ion supercapacitor is connected at the output of the buck converter in series with a resistor ($R_{load}$).

#### Why $R_{load}$ is Needed
- **Limits Inrush Current**  
   - When the supercapacitor is initially uncharged, it behaves almost like a short circuit.  
   - $R_{load}$ limits the inrush current from the buck converter, preventing voltage collapse or stress on the converter’s switching elements.  

- **Maintains MPPT Stability**  
   - Excessive instantaneous current can destabilise the MPPT loop.  
   - The series resistor ensures the converter output rises smoothly, keeping the PV panel near its MPP.  

- **Smooths Voltage Transients**  
   - The $R_{load} + C_{SC}$ combination forms a simple RC damping network, reducing voltage spikes and ripples, which is critical for accurate AC-coupled gesture detection.  

#### How $R_{load}$ Affects Charging Current
The series resistor directly determines the charging current:

$$
I_{charge} = \frac{V_{out\_converter} - V_{SC}}{R_{load}}
$$

- Smaller $R_{load}$ → Higher charging current, faster charging, but risk of inrush and converter stress.  
- Larger $R_{load}$ → Lower charging current, safer operation, but slower energy accumulation.  

**Optimal Choice:** Small enough to allow efficient charging but large enough to limit inrush current and maintain stable converter operation.  

---

## 3. Shadow Gesture Signal Conditioning

### 3.1 Signal Characteristics and Constraints
- **Input Signal:** $\Delta V_{PV}$ (gesture shadow) = ±150 mV (≈300 mV p-p).  
- **DC Offset:** Rides on ≈70 V bias.  
- **ADC Target:** Output centred around $V_{CC}/2$ (≈1.675 V), Arduino Nano 33 BLE Sense ($V_{CC}$ ≈3.35 V).  

---

### 3.2 Op-Amp Circuit (LT1496)
Configured as AC-Coupled Inverting Amplifier:

| PCB Component (Netlist) | Value | Role |
|--------------------------|-------|------|
| Op-Amp | LT1496 | Low-power, rail-to-rail output amplifier |
| $R_{in}$ ($R_{LT1496}$) | 500 kΩ | Inverting input resistor |
| $R_f$ ($RF_{LT1496}$) | 1.5 MΩ (Trimpot Max) | Feedback resistor (sets gain) |
| $R_A$ ($RDC_{LT1496}$) | 2 MΩ (set to 1.0 MΩ) | Upper bias resistor |
| $R_B$ ($RDIV_{LT1496}$) | 1 MΩ | Lower bias resistor |
| $C_{block}$ ($C_{DC\_LT1496}$) | 10 μF (≥75 V) | DC blocking capacitor |

---

### 3.3 DC Blocking Capacitor & Frequency Response
$C_{block} = 10 \,\mu\text{F}$ → forms high-pass filter with $R_{in}$.

Cutoff Frequency:

$$
f_c = \frac{1}{2 \pi R_{in} C_{block}}
$$

Using $R_{in} = 500 \,\text{kΩ}$ and $C_{block} = 10 \,\mu\text{F}$:

$$
f_c \approx 0.0318 \,\text{Hz}
$$

**Impact:**  
 - Attenuates long-term DC drifts (temperature, slow irradiance).  
 - Preserves AC signals (0.1 Hz–10 Hz) for gesture detection.  

---

### 3.4 Optimised Trimpot Configuration
**AC Gain ($A_v$):**

$$
A_v = -\frac{R_f}{R_{in}}
$$

$$
A_v = -\frac{1.5 \,\text{MΩ}}{500 \,\text{kΩ}} = -3
$$

Input: 300 mV p-p → Output: 0.9 V p-p.  

**DC Offset ($V_{REF}$):**

$$
V_{REF} = V_{CC} \cdot \frac{R_A}{R_A + R_B}
$$

$$
V_{REF} \approx \frac{2}{3} V_{CC} \approx 1.675 \,\text{V}
$$

✅ Ensures amplified signal is centred and scaled for ADC conversion.  

---

## 4. Theoretical Considerations

### 4.1 Li-Ion Supercapacitor Charge/Discharge Cycle
Li-ion hybrid capacitors differ from EDLCs in that they use battery-like intercalation on one electrode.

- **Charging phase:** Voltage rises relatively linearly until ≈3.8 V, with a mild plateau due to faradaic reactions.  
- **Discharging phase:** Provides a more gradual voltage decline, offering higher usable energy density than EDLCs.  
- **Cycle Stability:** Operates between 2.2–3.8 V; exceeding these limits reduces cycle life.  
- **Advantage in this system:** naturally higher ESR softens inrush current, preventing PV voltage collapse.  

---

### 4.2 PV Panel Operating Point & Resistive Load Line
The operating point of a PV module is determined by the intersection of its I–V characteristic curve and the load line:

Load resistance $R_L$ defines a straight line on the I–V plane:

$$
V = I \cdot R_L
$$

The Maximum Power Point (MPP) occurs near the “knee” of the I–V curve where:

$$
P = V \cdot I \quad \text{is maximised}
$$

- If $R_L$ is too low → panel is current-limited (low voltage, high current).  
- If $R_L$ is too high → panel is voltage-limited (high voltage, low current).  
- A properly tuned MPPT ensures the reflected input impedance of the converter matches the PV source impedance at MPP.  

---

### 4.3 Input Impedance of a Buck Converter
For a buck converter operating in continuous conduction:

$$
R_{in} \approx D^2 \cdot R_{load}
$$

where:  
- $D =$ duty cycle,  
- $R_{load} =$ effective resistance seen at the output.  

**Implications:**  
- As $D$ increases, $R_{in}$ decreases quadratically.  
- MPPT controllers adjust $D$ to reflect the correct $R_{in}$ onto the PV panel so it operates at MPP.  
- If the converter + storage element demand too much current (i.e., $R_{in}$ too small), the PV voltage collapses — which explains the instability seen with EDLCs.  

---

## 5. Microcontroller Power Management & ML/DL Feasibility

### Arduino Nano 33 BLE Sense Current Consumption
The Arduino Nano 33 BLE Sense is based on the nRF52840 SoC, which offers multiple low-power operating modes:

- **Active Mode (CPU running, peripherals active):**  
  ∼4–6 mA typical at 3.3 V during sampling and logging.  

- **Deep Sleep / System OFF Mode:**  
  ∼1–3 μA typical, with wake-up sources such as external interrupts (e.g., PIR sensor).  

In this project, the Nano 33 BLE Sense is intended to remain in deep sleep most of the time. It will be woken by a PIR motion detector (or another low-power external trigger) just prior to gesture sensing. Once the gesture capture is complete and data is logged, the MCU returns to deep sleep, minimising average energy consumption.  

---

### TinyML Model Deployment
Any trained gesture recognition model (e.g., Random Forest, 1D CNN, or LSTM) is designed to be compact enough for TinyML deployment on the Nano 33 BLE Sense.

TensorFlow Lite for Microcontrollers (TFLM) or similar lightweight inference engines can run efficiently within the device’s memory (1 MB Flash,256 kB RAM).

The expected model size for classifying simple gestures (≈10–20 repetitions per gesture) is tens of kilobytes, easily fitting within the available resources.

This ensures on-device inference is possible, eliminating the need for cloud connectivity while preserving the self-powered, autonomous design philosophy.

---

## 📌 Summary
This system demonstrates a high-voltage, energy-harvesting PV sensor capable of:

- Maintaining stable high-voltage operation via Li-Ion supercapacitor storage.  
- Extracting and amplifying minute AC fluctuations for shadow gesture detection.  
- Operating entirely off harvested solar energy with ultra-low-power circuitry.  
- Leveraging theoretical insights from Li-ion charge dynamics, PV load-line analysis, and buck converter input impedance for robust design.  
- Exploring a novel semi-transparent coffee table application, combining gesture interactivity with self-powered autonomy and future potential for under-panel displays.  
- Incorporating aggressive MCU power management (deep sleep with PIR wake) and TinyML model deployment, ensuring the system remains both practical and scalable.  











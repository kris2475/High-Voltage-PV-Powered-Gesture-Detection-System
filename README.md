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

---

## Initial PV Panel Characterisation
Initial measurements with a multimeter under **660 lux** mixed laboratory lighting (LED plus filtered daylight through a modern south-facing window) showed that the PV panel, connected to the LTC3638 converter, produced:

- LaTeX form: `$$V_{out} = 3.35\ \text{V}$$`  
  Plain text (copy-safe): ``V_out = 3.35 V``

- LaTeX form: `$$I_{sc} = 3.89\ \text{mA}$$`  
  Plain text (copy-safe): ``I_sc = 3.89 mA``

These readings confirm that the panel can reliably generate sufficient voltage and current to power the Arduino Nano 33 BLE Sense and provide a measurable AC signal for gesture detection.

Together, these innovations position the system as a step toward interactive, energy-autonomous smart furniture: surfaces that are at once functional, aesthetic, and responsive, with no external wiring or power supply required.

---

## 1. Project Overview
This project implements an ultra-low power, energy-harvesting system designed to detect shadow gestures or subtle environmental changes using a standard Photovoltaic (PV) panel as its primary sensor.

The system measures minute AC-coupled voltage fluctuations (`ΔV_PV`) present on the PV module’s output, while simultaneously harvesting energy to operate the Arduino Nano 33 BLE Sense microcontroller and charge the primary energy storage element.

> Notation (copy-safe): use `Delta V_PV` for `ΔV_{PV}` if needed.

### Core Objectives
- **Energy Harvesting:** Efficiently transfer power from the PV panel (≈70 V DC) to a 3.35 V system bus and storage element.  
  (copy-safe: `PV_nominal ≈ 70 V DC → system bus 3.35 V`)
- **Stable Storage:** Select an energy storage component that maximises energy density without compromising the PV module’s high DC operating point.
- **Signal Conditioning:** Isolate and amplify the small AC signal (±150 mV) riding on the high DC bus for accurate ADC measurement.
- **Gesture Detection:** Use the amplified `ΔV_{PV}` signal for robust, low-latency shadow gesture detection.

---

## 2. Energy Harvesting and Storage Design

### 2.1 Component Selection Rationale
The system’s feasibility hinges on maintaining the PV panel at a high operating voltage (≈70 V) for signal integrity. An experimental comparison between EDLC and Li-Ion supercapacitors showed critical stability differences:

| Capacitor     | C        | PV Operating Voltage (Test)             | Energy Stored (`E_max`) | Status        |
|---------------|----------|------------------------------------------|-------------------------|---------------|
| 50 F EDLC     | 50 F     | Collapsed to ≈4 V (Throughout charge)    | ≈120 J                  | ❌ Rejected    |
| 30 F Li-Ion   | 30 F     | Maintained 69–73 V                       | ≈146 J                  | ✅ Selected    |

**Export to Sheets**

### Critical Finding: MPPT Instability

**EDLC Failure (Root Cause Analysis):**  
The 50 F EDLC supercapacitor caused the PV voltage to collapse to about **4 V** *throughout* the charging cycle, failing to maintain the required ≈70 V operating point. This is explained by two interacting mechanisms:

- **Severe Load-Induced Overcurrent:**  
  The EDLC's large capacitance and very low ESR present an instantaneous, demanding load (near-short) to the buck converter's output. The MPPT controller attempts to satisfy this demand by drawing a correspondingly high current from the PV input. This reflected current demand exceeds the maximum power current (`I_MPP`) of the PV panel, forcing its operating point down the I–V curve into the current-limited region near `I_SC`. This voltage collapse is sustained throughout the charge cycle, as the demanding load cannot be met at the high-voltage MPP.

  (copy-safe summary: `EDLC → near-short at converter output → MPPT draws high input current > I_MPP → PV voltage collapses toward I_SC region`)

- **Control Loop Instability:**  
  The combination of very low ESR and large capacitance in the EDLC shifts the ESR zero of the converter's power stage transfer function down in frequency. If that low-frequency zero lies near the control-loop crossover frequency, phase margin is drastically reduced. An unstable control loop cannot settle; it may oscillate or latch into a low-impedance state, reinforcing the collapse and preventing recovery to the intended ≈70 V operating point.

**Why this happens (Simplified):**  
The EDLC's near-ideal capacitor behaviour provides an almost instantaneous sink during charging; the converter sees a near short and the MPPT/MPPT controller ends up either oscillating or forcing the PV into its current-limited region.

**Additional perspective:**  
MPPT controllers and energy harvesters usually expect a load that behaves somewhat resistively across the charge cycle. EDLCs violate that assumption.

**Li-Ion Success:**  
Higher ESR and electrochemical dynamics produce a smoother, more resistive-like load profile across the charge cycle. The Li-ion hybrid capacitor tapers current more gracefully, avoiding the sudden discontinuity and allowing stable MPPT operation at 69–73 V, even near full charge.

**Selected component:** `30 F Li-Ion Supercapacitor` (example: Vinatech VEL08203R8306G).

---

### 2.2 Power Converter and MPPT Strategy
- **Converter:** LTC3638 Buck-Boost Converter.  
- **Function:** Tracks the PV module’s Maximum Power Point (MPP) by dynamically adjusting input impedance (`R_t`) via duty cycle (`D`).

LaTeX: `$$R_t = D^2 \cdot R_o$$`  
Copy-safe: ``R_t = D^2 * R_o``

**Input Capacitance Requirement:**
- A ≥75 V capacitor is required at PV terminals.
- It acts as an energy buffer, smoothing discontinuous switching currents and stabilising MPPT sensing.

---

### 2.3 Li-Ion Supercapacitor Connection & Series Load Resistor
The 30 F Li-Ion supercapacitor is connected at the output of the buck converter in series with a resistor `R_load`.

**Why `R_load` is Needed**
- **Limits Inrush Current**  
  - When the supercapacitor is initially uncharged it behaves almost like a short; `R_load` limits the inrush current from the converter.
- **Maintains MPPT Stability**  
  - Excessive instantaneous current can destabilise the MPPT loop; the series resistor helps the converter output rise smoothly and keeps the PV panel near MPP.
- **Smooths Voltage Transients**  
  - `R_load + C_SC` forms an RC damping network, which reduces spikes and ripples that would otherwise corrupt the AC-coupled gesture signal.

**How `R_load` Affects Charging Current**

LaTeX: `$$I_{charge} = \frac{V_{out\_converter} - V_{SC}}{R_{load}}$$`  
Copy-safe: ``I_charge = (V_out_converter - V_SC) / R_load``

- Smaller `R_load` → higher charging current (faster), but greater inrush and converter stress.  
- Larger `R_load` → lower charging current (slower), but safer and more stable.  
**Optimal choice:** Small enough for efficient charging, large enough to limit inrush and maintain stable converter behavior.

---

## 3. Shadow Gesture Signal Conditioning

### 3.1 Signal Characteristics and Constraints
- **Input Signal:** `ΔV_PV` (gesture shadow) = ±150 mV (≈300 mV p-p).  
  LaTeX: `$\Delta V_{PV} = \pm 150\ \text{mV} \approx 300\ \text{mV}_{p-p}$`  
  Copy-safe: ``Delta V_PV = ±150 mV (≈300 mV p-p)``

- **DC Offset:** Rides on ≈70 V DC bias.  
- **ADC Target:** Output centred around `V_CC / 2 ≈ 1.675 V` (Arduino Nano 33 BLE Sense with `V_CC ≈ 3.35 V`).

---

### 3.2 Op-Amp Circuit (LT1496)
Configured as an **AC-coupled inverting amplifier** to extract the small AC fluctuations riding on the high DC bias.

**PCB Component (Netlist)**

| Component | Value | Role |
|-----------|-------|------|
| Op-Amp | LT1496 | Low-power, rail-to-rail output amplifier |
| `R_in` (`R_LT1496`) | 500 kΩ | Inverting input resistor |
| `R_f` (`RF_LT1496`) | 1.5 MΩ (Trimpot Max) | Feedback resistor (sets gain) |
| `R_A` (`RDC_LT1496`) | 2 MΩ (set to 1.0 MΩ) | Upper bias resistor |
| `R_B` (`RDIV_LT1496`) | 1 MΩ | Lower bias resistor |
| `C_block` (`C_DC_LT1496`) | 10 μF (≥75 V) | DC blocking capacitor |

**Export to Sheets**

---

### 3.3 DC Blocking Capacitor & Frequency Response
The DC-blocking capacitor `C_block = 10 μF` forms a high-pass filter with `R_in`.

LaTeX: `$$f_c = \frac{1}{2\pi R_{in} C_{block}}$$`  
Copy-safe: ``f_c = 1 / (2 * pi * R_in * C_block)``

Using `R_in = 500 kΩ` and `C_block = 10 μF`:

LaTeX: `$$f_c \approx 0.0318\ \text{Hz}$$`  
Copy-safe: ``f_c ≈ 0.0318 Hz``

**Impact:**
- Attenuates long-term DC drifts (temperature, slow irradiance changes).  
- Preserves AC signals in the gesture band (≈0.1 Hz–10 Hz).

---

### 3.4 Optimised Trimpot Configuration
**AC Gain (`A_v`)** for the inverting amplifier:

LaTeX: `$$A_v = -\frac{R_f}{R_{in}}$$`  
Copy-safe: ``A_v = - R_f / R_in``

Using the given values:

LaTeX: `$$A_v = -\frac{1.5\ \text{M}\Omega}{500\ \text{k}\Omega} = -3$$`  
Copy-safe: ``A_v = -3``

- Input: 300 mV p-p → Output: 0.9 V p-p.

**DC Offset (`V_REF`)**: created by the resistor divider `R_A` (top) and `R_B` (bottom), biasing the amplifier output to mid-supply for ADC:

LaTeX: `$$V_{REF} = V_{CC} \cdot \frac{R_B}{R_A + R_B}$$`  
Copy-safe: ``V_REF = V_CC * (R_B / (R_A + R_B))``

With `R_A` set to `1.0 MΩ` and `R_B = 1.0 MΩ`:

LaTeX: `$$V_{REF} \approx \frac{V_{CC}}{2} \approx 1.675\ \text{V}$$`  
Copy-safe: ``V_REF ≈ V_CC/2 ≈ 1.675 V``

✅ Ensures amplified signal is centred and scaled for ADC conversion.

---

## 4. Theoretical Considerations

### 4.1 Li-Ion Supercapacitor Charge/Discharge Cycle
Li-ion hybrid capacitors differ from EDLCs in that they use battery-like intercalation on one electrode.

- **Charging phase:** Voltage rises relatively linearly until ≈3.8 V, with a mild plateau due to faradaic reactions.  
- **Discharging phase:** Provides a more gradual voltage decline, offering higher usable energy density than EDLCs.  
- **Cycle Stability:** Operates between ~2.2–3.8 V; exceeding these limits reduces cycle life.  
- **Advantage in this system:** Naturally higher ESR softens inrush current and prevents PV voltage collapse.

---

### 4.2 PV Panel Operating Point & Resistive Load Line
The operating point of a PV module is determined by the intersection of its I–V characteristic curve and the load line. The load resistance `R_L` defines a straight line on the I–V plane:

LaTeX: `$$V = I \cdot R_L$$`  
Copy-safe: ``V = I * R_L``

- The Maximum Power Point (MPP) occurs near the knee of the I–V curve where `P = V * I` is maximised.  
- If `R_L` is too low → panel is current-limited (low voltage, high current).  
- If `R_L` is too high → panel is voltage-limited (high voltage, low current).  
A properly tuned MPPT ensures the reflected input impedance of the converter matches the PV source impedance at MPP.

---

### 4.3 Input Impedance of a Buck Converter
For a buck converter (in continuous conduction), the reflected input resistance is approximately:

LaTeX: `$$R_{in} \approx D^2 \cdot R_{load}$$`  
Copy-safe: ``R_in ≈ D^2 * R_load``

Where `D` is the duty cycle and `R_load` is the effective resistance at the output.

**Implications:**
- As `D` increases, `R_in` decreases quadratically.  
- MPPT controllers adjust `D` so that `R_in` reflects the correct value at the PV panel MPP.  
- If the converter + storage element demand too much current (i.e., `R_in` too small), the PV voltage collapses — which explains the instability seen with EDLCs.

---

## 5. Microcontroller Power Management & ML/DL Feasibility

### Arduino Nano 33 BLE Sense Current Consumption
The Arduino Nano 33 BLE Sense is based on the nRF52840 SoC and offers multiple low-power modes:

- **Active Mode (CPU running, peripherals active):** ∼4–6 mA typical at 3.3 V during sampling and logging.  
- **Deep Sleep / System OFF Mode:** ∼1–3 μA typical, with wake-up sources such as external interrupts (e.g., PIR sensor).

**Strategy in this project:** Keep the Nano 33 BLE Sense in deep sleep most of the time. Wake via a PIR motion detector (or other low-power trigger) just prior to gesture sensing; perform capture & logging; then return to deep sleep to minimise average energy consumption.

### TinyML Model Deployment
- Any trained gesture recognition model (e.g., Random Forest, 1D CNN, or LSTM) should be compact enough for TinyML deployment on the Nano 33 BLE Sense.  
- **Inference engines:** TensorFlow Lite for Microcontrollers (TFLM) or similar.  
- **Memory envelope:** 1 MB Flash, 256 kB RAM. Expected model size for simple gestures (≈10–20 repetitions per gesture) is tens of kilobytes — well within the device limits.  
- **Outcome:** On-device inference is feasible, eliminating the need for cloud connectivity while preserving the self-powered, autonomous design philosophy.

---

## 📌 Summary
This system demonstrates a high-voltage, energy-harvesting PV sensor capable of:

- Maintaining stable high-voltage operation via Li-Ion supercapacitor storage.  
- Extracting and amplifying minute AC fluctuations for shadow gesture detection.  
- Operating entirely off harvested solar energy with ultra-low-power circuitry.  
- Leveraging theoretical insights from Li-ion charge dynamics, PV load-line analysis, and buck converter input impedance for robust design.  
- Exploring a novel semi-transparent coffee-table application, combining gesture interactivity with self-powered autonomy and future potential for under-panel displays.  
- Incorporating aggressive MCU power management (deep sleep with PIR wake) and TinyML model deployment, ensuring the system remains both practical and scalable.

---










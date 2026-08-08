# 2-Phase Synchronous Digitally Controlled Buck Converter

<img width="75%" alt="Hardware Setup" src="https://github.com/user-attachments/assets/4e6a6dc8-d4ab-449f-abea-d0e89fef0ae7" />

## Project Overview

**Goal:** Build a multi-phase, MCU-controlled high-power buck converter to act as a current amplifier for a linear lab power supply (30V/5A) at lower voltages.

**Project Scope:**
- **Custom Hardware:** 4-layer power stage and control circuitry designed in KiCad with focus on EMI mitigation. Switching at 250 kHz, designed for 10A at 12V output.
- **Advanced MCU Control:** Firmware written in C (STM32 HAL) for the STM32G474, utilizing a software-defined voltage loop (via FMAC), a pure-hardware current protection loop, and hardware encoder UI integration.

---

## System Architecture

<img width="75%" alt="System Schematic Block Diagram" src="https://github.com/user-attachments/assets/4f523aab-9181-44df-9282-12249193e45a" />

The system is split into two primary domains: the digital control stage, centered around the STM32G474's advanced power-control peripherals, and the power stage, featuring dedicated gate drivers and power MOSFETs.

---

## Hardware Details

### The Power Stage

<img width="75%" alt="Power stage" src="https://github.com/user-attachments/assets/4f237d1f-80b9-4e3f-808f-0b290d7c2947" />

The power stage consists of two identical, interleaved synchronous buck topologies:
- **Gate Drivers:** UCC27211A drivers were selected for their 30V bootstrap tolerance, high 4A peak drive current, and discrete inputs, which allow for precise dead-time insertion directly from the MCU's HRTIM.
- **MOSFETs:** Two different Alpha & Omega DFN-8 MOSFETs are utilized to optimize the specific roles of the buck topology. The AONS66406 is used on the high side for its lower gate charge (faster switching, lower switching losses), while the AON6144 is used on the low side for its lower Rds(on) (better conduction efficiency). 
- **Passives:** The system uses 22µH molded inductors (derated from 8.5A to around 5A). Input capacitance consists of 3x 10µF 50V MLCCs + 100µF electrolytic, while the output utilizes 3x 22µF 25V MLCCs + 100µF electrolytic.

### PCB Layout & EMI Mitigation

<img width="75%" alt="PCB Layout Diagram" src="https://github.com/user-attachments/assets/29182e1a-4364-4b2f-b1c3-541b1a4166f1" />

Great care was taken during the PCB routing phase to minimize radiated and conducted emissions:
- **4-Layer Stackup:** Allows for smaller L1-to-L2 distances, increasing plane coupling and reducing radiated emissions.
- **Strategic Placement:** The MLCC capacitors "hug" the MOSFETs on the input and the inductors on the output to minimize high-frequency current return paths. The slower bulk electrolytic capacitors are placed further out at the input/output terminals.
- **Node Optimization:** The physical copper area of the noisy switching (SW) nodes was kept to an absolute minimum. Molded inductors were chosen specifically to further contain magnetic emissions.
- **Signal Isolation:** Delicate analog feedback signals for the current loops were routed exclusively on the bottom layer to shield them from the noisy power stage on the top layer. They were also routed as differential pairs in order to minimise common mode noise.

### The Digital Stage & UI

<img width="75%" alt="Digital stage" src="https://github.com/user-attachments/assets/06829eec-8f17-4a4e-af50-fba1cdd01146" />

The control circuitry relies heavily on the STM32G474's internal mixed-signal capabilities. Additionally, the system features a local user interface: a rotary encoder is connected to a hardware timer in encoder mode, allowing the user to adjust voltage and current setpoints on the fly, which are displayed on an integrated 16x2 LCD.

---

## Control Loop Architecture

<p align="center">
  <img width="49%" alt="Voltage loop" src="https://github.com/user-attachments/assets/03311fad-7e00-4300-a4e3-63d522c22aa8" />
  <img width="49%" alt="Current loop" src="https://github.com/user-attachments/assets/e8ec99c6-7c0e-4e91-8c09-d1dc25072f6f" />
</p>

The core of the project is its dual-loop control scheme, leveraging the specific hardware accelerators of the STM32G4 family:

### 1. Software Voltage Control Loop
- **Averaged Sampling:** The High-Resolution Timer (HRTIM) triggers ADC3 to sample the feedback voltage (buffered via an internal OpAmp) at the exact midpoint of the MOSFET's on-time. This effectively averages the signal without requiring software filtering.
- **Hardware Math Acceleration:** The ADC value is sent via DMA directly to the Filter Math Accelerator (FMAC) `WDATA` register, where a Proportional (P) control algorithm is calculated in hardware. 
- **Duty Cycle Update:** Once the FMAC presents the result in the `RDATA` register, it generates an interrupt. The MCU core wakes up, applies corrections for phase current mismatch, and writes the new duty cycle to the HRTIM `CMP1xR` register.

### 2. Hardware Current Control Loop
- **Differential Measurement:** Current is measured via a shunt resistor at the low-side source. The small signal is read differentially for improved noise rejection.
- **Internal Analog Processing:** The voltage is amplified by an internal MCU difference amplifier and immediately fed into a built-in hardware comparator against a threshold set by an internal DAC.
- **Asynchronous Shutdown:** The comparator output is routed directly to the HRTIM external event port. This asynchronous link ensures near-zero latency between an overcurrent event and power stage shutoff, effectively realizing valley current control mode.
- **Blanking Window:** The HRTIM utilizes an internal event blanking window to ignore switching transients, preventing false safety triggers.

---

## Results & Verification

Testing was conducted in progressive stages to ensure system stability and safety:

<img width="75%" alt="Test setup" src="https://github.com/user-attachments/assets/127d3293-2de8-4bf3-a415-9189e9150239" />

**1. Controller Verification (No Power Stage):** 
Feedback voltages were injected directly into the digital section to validate the software. Interleaving, dead-time insertion, duty cycle adjustment, soft start, current balancing, and overcurrent protection were all verified before high voltage was applied.

**2. Real-World Ripple Testing:** 
With the power stage assembled and hooked to a 25V lab supply, the output voltage ripple was measured at no load.

<img width="75%" alt="ripple oscope showcase" src="https://github.com/user-attachments/assets/1e51d080-27ea-41bc-8013-400a72b5851d" />

The dual-phase interleaved topology yielded a voltage ripple of just 7mVp-p. Compared to single-phase operation (which measured 35mVp-p), interleaving provided an 80% reduction in ripple purely through topology.

**3. Efficiency Analysis:**
The converter was tested across its full operating range using an electronic DC load. 

| Input Volts (V) | Input Current (A) | Output Volts (V) | Output Current (A) | Efficiency (%) |
| :---: | :---: | :---: | :---: | :---: |
| 25.12 | 0.077 | 12.03 | 0.000 | 0.00 |
| 25.12 | 0.218 | 11.91 | 0.299 | 64.82 |
| 24.98 | 0.538 | 11.16 | 0.999 | 82.97 |
| 24.96 | 0.989 | 11.07 | 2.000 | 89.71 |
| 24.90 | 2.110 | 10.92 | 4.500 | 93.60 |
| 24.64 | 3.229 | 10.70 | 7.000 | 94.19 |
| 24.60 | 3.677 | 10.65 | 8.000 | 94.21 |
| 24.56 | 4.573 | 10.55 | 10.000 | 93.95 |

<img width="75%" alt="efficiency graph" src="https://github.com/user-attachments/assets/58ac6008-74c0-400b-8118-b2fd837ad2f9" />

*Test conditions note: Testing was capped at 25V input (instead of 30V) due to observed ringing at the switching node, which threatened to exceed the transistor voltage ratings.*

---

## Future Improvements

While the current revision serves as a highly successful proof of concept, several areas are targeted for future improvement:

- **Regulator Tuning:** The observed voltage drop under load is due to the implementation of a pure Proportional (P) regulator. A PI regulator caused stability issues in this revision, but with proper parameter tuning, stable operation with minimal offset drop is achievable.
- **Ringing Attenuation:** Adding a snubber circuit or increasing gate resistor values will dampen the switching node ringing. Alternatively, transistors with higher voltage ratings could be used, though at a slight cost to Rds(on) and gate capacitance.
- **Component Optimization:** The passive components and MOSFETs were intentionally over-provisioned for this prototype. A production-ready version would undergo stricter component selection to reduce costs.
- **Software Hardening:** The firmware requires further refinement and exhaustive safety testing to handle edge cases gracefully (e.g., during testing, one phase shut down, and the converter was ultimately saved by the external PSU's overcurrent protection).

---

## Documentation

Comprehensive project documentation is available in the repository (written in Polish, prepared for university coursework in Power Electronics and Digital Signal Controllers):
- **Raport Techniczny:** Initial project considerations and mathematical foundations.
- **Raport Implementacyjny:** Simulated testing and core code fragments.
- **Raport Wdrożeniowy:** Physical implementation and final hardware measurements.
- **Raport Projektowy:** Executive summary of the project lifecycle.

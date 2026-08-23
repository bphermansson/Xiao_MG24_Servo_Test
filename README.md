# Seeed Studio XIAO MG24 Servo Test Project

This project provides a reliable, custom Arduino servo testing setup tailored specifically for the **Seeed Studio XIAO MG24** (featuring the Silicon Labs EFR32MG24 microcontroller). 

Since standard Arduino `Servo.h` libraries rely on AVR/ESP32-specific hardware timers and are not natively supported by the Silicon Labs board package, this project implements a timing-accurate, custom non-blocking software `ServoDriver` class. This ensures complete compatibility, smooth sweeps, and manual microsecond calibration.

---

## 1. Wiring & Connections

> [!WARNING]
> **Voltage & Power Safety:**
> - The Seeed Studio XIAO MG24 is a **3.3V logic device**. High-torque or multiple servos can exceed the power limits of the USB port/board and cause restarts or damage.
> - **Micro-Servos (e.g., SG90):** Can be powered directly from the `5V` pin on the XIAO when connected to USB.
> - **Standard/Large Servos:** Must be powered via an external 5V power supply. **Crucial:** Connect the external supply's Ground (GND) to the XIAO's Ground (GND) to establish a common reference.

### Pinout Table

| Servo Wire Color | Signal Type | Connection on XIAO MG24 | Notes |
| :--- | :--- | :--- | :--- |
| **Orange / Yellow** | Signal (PWM) | **D0** (PC00) | PWM control signal (3.3V logic) |
| **Red** | Power (VCC) | **5V** | 5V output from USB or external 5V source |
| **Brown / Black** | Ground (GND) | **GND** | Common reference ground |

---

## 2. Setting Up Arduino IDE for XIAO MG24

To upload this code to your Seeed Studio XIAO MG24:

1. Open **Arduino IDE**.
2. Go to **File** > **Preferences**.
3. In **Additional Boards Manager URLs**, add the Seeed Studio board manager URL:
   ```text
   https://files.seeedstudio.com/arduino/package_seeed_index.json
   ```
   *(Note: You can also use the official Silicon Labs Arduino Core URL: `https://github.com/SiliconLabs/arduino/releases/download/v2.3.0/package_siliconlabs_index.json` depending on your preferred core version).*
4. Go to **Tools** > **Board** > **Boards Manager...**, search for **Seeed Studio XIAO MG24** (or **Silicon Labs**) and click **Install**.
5. Select **Tools** > **Board** > **Seeed Studio XIAO MG24** (or **Arduino Nano Matter** which uses the same EFR32MG24 chip).
6. Connect your board via USB-C, choose the correct COM/Serial port in **Tools** > **Port**, and upload the sketch.

---

## 3. How to Test & Commands

Once uploaded, open the **Serial Monitor** at **115200 baud** (with `Newline` or `Both NL & CR` ending). You can interactively test and calibrate the servo using the following serial commands:

*   **`[0-180]`**
    *   *Example:* `90`
    *   Moves the servo to a specific angle in degrees.
*   **`u[500-2500]`**
    *   *Example:* `u1500`
    *   Writes a raw pulse duration in microseconds. Excellent for precision alignment or calibrating maximum and minimum limits.
*   **`sweep`**
    *   Starts automatic continuous back-and-forth sweeping (0 to 180 degrees and back).
*   **`stop`** / **`manual`**
    *   Halts the automated sweep at the current position and returns to manual mode.
*   **`status`**
    *   Prints the current mode, target angle, pulse duration in microseconds, and active control pin.
*   **`help`**
    *   Displays the command list again.

# Rotor Simulator for PC

**Arduino-based AZ/EL rotor simulator for PC rotor tracking software testing**

Compact test instrument designed to simulate a real azimuth/elevation rotator for software development, bench testing and experimentation.

This project interfaces with **PstRotator** using an Easycom/K3NG style protocol and allows testing without a physical rotor system.

**Designed and built by Emanuele Rossi (IK1APW)**
---

# Overview

This simulator uses two potentiometers to generate:

* Azimuth (AZ)
* Elevation (EL)

and sends live position data to the PC exactly as a real rotor controller would.

It is intended for:

* PstRotator testing
* Rotor controller development
* EME / satellite experimentation
* Bench debugging
* Educational use
---

# Features

* Arduino Nano based
* Two-potentiometer AZ/EL simulation
* Full OLED display (128x64 SSD1306)
* Real-time AZ/EL readout
* Motion direction arrows
* Compatible with PC rotor tracking software
* Virtual rotator for lab use
---

# Hardware

## Main Components

* Arduino Nano or Uno
* 2 linear potentiometers
* OLED 0.96” SSD1306 (128x64 I2C)
* USB connection to PC

## OLED Wiring
```text
OLED       Arduino Nano
----------------------
GND   ->   GND
VCC   ->   5V (or 3.3V)
SDA   ->   A4
SCL   ->   A5
```

Typical I2C address:

```text
0x3C
```

---

# PstRotator Compatibility

Tested with **PstRotator** using Easycom / K3NG style protocol.

Typical response format:

```text
AZ257.3 EL034.5
```

General format:

```text
AZxxx.x ELyyy.y
```

Use it as a virtual rotor for:

* Tracking software testing
* Protocol debugging
* User interface testing
* Controller development

without connecting motors or a real rotator.
---

# Notes About Analog Resolution

Small fluctuations in decimal readings are normal with analog potentiometers.

They are due to:

* ADC resolution
* Potentiometer tolerances
* Small analog noise

This does **not** affect simulator operation.

Integer-degree stability is what matters for practical use.
---

# Why This Project

Useful when developing or testing rotor-related software before connecting real hardware.

A small but practical instrument for experimentation.
---

# Final Note

Shared in the spirit of amateur radio experimentation and open technical collaboration.

If you improve it, contributions are welcome.

**Built by a radio amateur, for radio amateurs.**

73,

**IK1APW**

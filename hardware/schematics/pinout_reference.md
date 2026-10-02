# 🔌 Hardware Pinout & Wiring Reference

This document details the exact pinouts for the ESP32-WROVER-E controller, 74HC595 shift register cascade, ULN2803A Darlington drivers, and the 8-pin JST-XH digit connectors.

---

## 1. ESP32 to Shift Register & GSM Connections

| ESP32 Pin | Function | Target Component | Description |
|---|---|---|---|
| **GPIO 13** | `DATA` (DS) | 74HC595 Pin 14 (SER) | Serial data line for segment bits |
| **GPIO 14** | `CLOCK` (SH_CP) | 74HC595 Pin 11 (SRCLK) | Shift register clock |
| **GPIO 12** | `LATCH` (ST_CP) | 74HC595 Pin 12 (RCLK) | Output latch clock |
| **GPIO 16** | `RX2` | SIM800L `TX` | Hardware Serial 2 Receive |
| **GPIO 17** | `TX2` | SIM800L `RX` | Hardware Serial 2 Transmit |
| **GPIO 4** | `COLON_CTRL` | NPN Transistor Base | Optional colon separator `:` blinking |
| **5V / VIN** | `VCC_5V` | LM2596 Output (+) | Main 5V regulated logic power |
| **GND** | `GND` | Common Ground Rail | System ground reference |

---

## 2. 8-Pin JST-XH Digit Connector Pinout (Per Digit)

Each 7-segment digit uses a dedicated 8-pin JST-XH connector wired to the PCB:

```text
 ┌────────────────────────────────────────────────────────┐
 │ [Pin 1] [Pin 2] [Pin 3] [Pin 4] [Pin 5] [Pin 6] [Pin 7] [Pin 8] │  (8-Pin JST Male)
 └────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───┘
      │       │       │       │       │       │       │       │
    +24V    Seg A   Seg B   Seg C   Seg D   Seg E   Seg F   Seg G
  (Common)  (Top)  (Top-R) (Bot-R)  (Bot)  (Bot-L) (Top-L)  (Mid)
```

| JST Pin | Wire Function | Destination on LED Digit | ULN2803 Output |
|---|---|---|---|
| **Pin 1** | **+24V Common Anode** | Shared `+24V` loop connecting all 7 segments | Direct +24V Bus |
| **Pin 2** | **Segment A** | Top horizontal segment `-` pad | Channel 1 (Out 1) |
| **Pin 3** | **Segment B** | Top-right vertical segment `-` pad | Channel 2 (Out 2) |
| **Pin 4** | **Segment C** | Bottom-right vertical segment `-` pad | Channel 3 (Out 3) |
| **Pin 5** | **Segment D** | Bottom horizontal segment `-` pad | Channel 4 (Out 4) |
| **Pin 6** | **Segment E** | Bottom-left vertical segment `-` pad | Channel 5 (Out 5) |
| **Pin 7** | **Segment F** | Top-left vertical segment `-` pad | Channel 6 (Out 6) |
| **Pin 8** | **Segment G** | Middle horizontal segment `-` pad | Channel 7 (Out 7) |

---

## 3. 74HC595 to ULN2803A Interfacing

```text
 74HC595 Shift Register          ULN2803A Darlington Driver        24V LED Strip Segment
 ┌──────────────────────┐        ┌────────────────────────┐        ┌─────────────────────┐
 │ Pin 15 (Q0)          ├───────>│ Pin 1 (IN1)   OUT1 (18)├───────>│ Segment A (-) Pad   │
 │ Pin 1  (Q1)          ├───────>│ Pin 2 (IN2)   OUT2 (17)├───────>│ Segment B (-) Pad   │
 │ Pin 2  (Q2)          ├───────>│ Pin 3 (IN3)   OUT3 (16)├───────>│ Segment C (-) Pad   │
 │ Pin 3  (Q3)          ├───────>│ Pin 4 (IN4)   OUT4 (15)├───────>│ Segment D (-) Pad   │
 │ Pin 4  (Q4)          ├───────>│ Pin 5 (IN5)   OUT5 (14)├───────>│ Segment E (-) Pad   │
 │ Pin 5  (Q5)          ├───────>│ Pin 6 (IN6)   OUT6 (13)├───────>│ Segment F (-) Pad   │
 │ Pin 6  (Q6)          ├───────>│ Pin 7 (IN7)   OUT7 (12)├───────>│ Segment G (-) Pad   │
 │ Pin 7  (Q7 - DP)     ├───────>│ Pin 8 (IN8)   OUT8 (11)├───────>│ Colon / DP (-) Pad  │
 └──────────────────────┘        └────────────────────────┘        └─────────────────────┘
```

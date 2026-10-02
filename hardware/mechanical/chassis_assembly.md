# 🛠️ Mechanical & Chassis Assembly Guide

This document explains the physical fabrication and clean wiring methodology for mounting the 24V COB LED strips onto the chronometer chassis with **zero visible wires on the front panel**.

---

## 1. Chassis Preparation & Optical Baffling

1. **Surface Cleaning:**
   * Remove all old components, glue, or solder residues.
   * Degrease the segment channels with isopropyl alcohol to ensure strong adhesion of the 3M LED backing tape.
2. **Optical Baffles:**
   * Ensure 5mm walls/baffles separate adjacent segments to prevent light bleed between segments (e.g., between Segments A, B, and G).
   * A 3mm Opal PMMA (Acrylic) diffuser is placed 10mm above the LED plane for uniform neon-style light distribution.

---

## 2. Feed-Through Holes & "Invisible Wiring"

To hide all wires completely behind the chassis:

```text
 ┌────────────────────────────────────────────────────────┐
 │ [24V COB LED STRIP]                          [+ Pad] (o) Drill 3-4mm hole
 │                                              [- Pad] (o) to pass wires to back
 └────────────────────────────────────────────────────────┘
```

1. Drill a **3mm to 4mm hole** at one end of every segment channel (42 holes total across the 6 digits).
2. Solder 15cm flexible wires (Red for `+`, Black for `-`) to the LED strip pads **on the workbench before mounting**.
3. Feed the wires through the drilled holes to the rear side of the chassis.
4. Peel the double-sided tape and press the LED strip firmly into the channel.
5. Apply a dab of hot glue on the rear side of each hole as a **strain relief**.

---

## 3. Rear-Side Wiring Topology (Common Anode "Star" Knot)

Behind each digit (7 segments = 14 wires total):

```text
 Seg A (+) Red ──┐
 Seg B (+) Red ──┤
 Seg C (+) Red ──┤
 Seg D (+) Red ──┼── (Twist 7 Red wires together) ──> Solder to JST Pin 1 (+24V)
 Seg E (+) Red ──┤
 Seg F (+) Red ──┤
 Seg G (+) Red ──┘

 Seg A (-) Black ───────────────────────────────────> Solder to JST Pin 2 (Seg A)
 Seg B (-) Black ───────────────────────────────────> Solder to JST Pin 3 (Seg B)
 Seg C (-) Black ───────────────────────────────────> Solder to JST Pin 4 (Seg C)
 Seg D (-) Black ───────────────────────────────────> Solder to JST Pin 5 (Seg D)
 Seg E (-) Black ───────────────────────────────────> Solder to JST Pin 6 (Seg E)
 Seg F (-) Black ───────────────────────────────────> Solder to JST Pin 7 (Seg F)
 Seg G (-) Black ───────────────────────────────────> Solder to JST Pin 8 (Seg G)
```

1. **Group Positive Wires:** Twist all 7 Red wires together into one single bundle and solder to the **JST Pin 1 (+24V)**. Insulate with heat-shrink tubing.
2. **Direct Segment Control:** Solder each of the 7 Black wires to its dedicated JST segment pin.
3. **Cable Dressing:** Secure the 6 digit harnesses flat against the rear panel using adhesive cable clips or spiral wrap.

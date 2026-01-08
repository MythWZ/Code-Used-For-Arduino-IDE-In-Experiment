# Plant Circadian Rhythm Experiment – Arduino Code

This repository contains the Arduino IDE source code used in the experiment in the paper "The Effect of the Removal of External Light Cues on Plants' Circadian Rhythms."

# Investigating circadian rhythms in plants under three lighting conditions:
- Continuous LED light
- Continuous darkness
- Natural 12h light / 12h dark cycle

## Hardware
- Arduino Uno
- AD620 instrumentation amplifiers
- Analog inputs A0–A5
- Sampling interval: once every 10 minutes

## Files
- plant_logger.ino — main data-logging program

The code logs timestamped analog values and outputs data via the serial monitor.


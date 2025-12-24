# 05_adc_read - ADC Reading Test

## Description

This program demonstrates analog input reading using the Raspberry Pi Pico 2's built-in ADC (Analog-to-Digital Converter). It reads both an external analog input and the internal temperature sensor, displaying values in multiple formats.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- Optional: 10kΩ potentiometer for testing external ADC input
- Optional: Analog sensor (temperature, light, etc.)

## Wiring

### Basic Test (Temperature Sensor Only)
No external wiring required - uses internal temperature sensor.

### With Potentiometer (Recommended)
```
3.3V ----[Potentiometer]---- GND
              |
           (Wiper)
              |
           GPIO 26 (ADC0)
```

### With Analog Sensor (e.g., LM35 Temperature Sensor)
```
Sensor Output ---- GPIO 26 (ADC0)
Sensor VCC ------- 3.3V
Sensor GND ------- GND
```

## Features

- 12-bit ADC resolution (4096 levels: 0-4095)
- Multi-sample averaging (10 samples) for noise reduction
- External analog input reading (GPIO 26 / ADC0)
- Internal temperature sensor reading (ADC4)
- Multiple output formats:
  - Raw ADC value (0-4095)
  - Voltage (0.0 - 3.3V)
  - Percentage (0-100%)
  - Temperature (Celsius and Fahrenheit)
- Visual bar graph display
- Continuous sampling at 1 Hz

## Expected Output

Serial output:
```
========================================
  Pico 2 ADC Reading Test
========================================
External ADC: GPIO 26 (ADC0)
Temperature:  ADC4 (internal sensor)
Resolution:   12-bit (0-4095)
Reference:    3.3 V
Averaging:    10 samples
Sample Rate:  Every 1000 ms
========================================

Optional: Connect potentiometer to GPIO 26
  - One end to 3.3V
  - Other end to GND
  - Wiper (middle) to GPIO 26

========== Reading #0 ==========

--- ADC Readings ---
Time: 2.345 seconds

External Input (GPIO 26, ADC0):
  Raw value:  2048 / 4095 (0x800)
  Voltage:    1.650 V
  Percentage: 50.0%

Internal Temperature Sensor (ADC4):
  Temperature: 23.45 C (74.21 F)

Bar Graph [==========          ] 50%

========== Reading #1 ==========
...
```

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 05_adc_read
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/05_adc_read/05_adc_read.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

### Test 1: Temperature Sensor Only
1. Upload the program
2. Connect to USB serial at 115200 baud
3. Observe internal temperature readings
4. Temperature should be close to ambient (room temperature)
5. Verify temperature changes if you warm the chip (hold it gently)

### Test 2: With Potentiometer
1. Connect potentiometer as shown in wiring diagram
2. Upload the program and connect to serial
3. Turn the potentiometer
4. Observe voltage changing from 0V to 3.3V
5. Verify bar graph reflects potentiometer position

### Test 3: With Analog Sensor
1. Connect analog sensor output to GPIO 26
2. Power sensor from 3.3V (or appropriate voltage)
3. Verify readings match sensor specifications
4. Example: LM35 outputs 10mV/°C

## How It Works

**ADC Basics:**
- Converts analog voltage (0-3.3V) to digital value (0-4095)
- 12-bit resolution: 3.3V / 4096 = 0.806 mV per step
- RP2350 has 5 ADC channels (ADC0-ADC3 on GPIOs, ADC4 is temperature sensor)
- Conversion time: ~2 µs per sample

**Multi-Sample Averaging:**
- Takes 10 consecutive readings
- Averages them to reduce noise
- Improves accuracy and stability
- Trade-off: Slightly slower (but still fast at 10 samples)

**Internal Temperature Sensor:**
- Built into RP2350 chip
- Measures die temperature (chip temperature)
- Not highly accurate (±2-3°C typical)
- Useful for thermal monitoring and relative measurements
- Formula: T = 27 - (V - 0.706) / 0.001721

**ADC Channel Mapping:**
- GPIO 26 → ADC0
- GPIO 27 → ADC1
- GPIO 28 → ADC2
- GPIO 29 → ADC3
- Internal → ADC4 (temperature sensor)

## Customization

You can modify these parameters in the code:
- `ADC_EXTERNAL_PIN`: Change to GPIO 27, 28, or 29 for ADC1-3
- `SAMPLE_INTERVAL_MS`: Adjust reading frequency
- `AVERAGE_SAMPLES`: More samples = less noise but slower
- Temperature conversion constants (calibrate for your specific chip)

## Important Notes

**Voltage Limits:**
- NEVER apply more than 3.3V to ADC pins
- Maximum safe input: 3.3V
- Exceeding this may damage the chip
- Use voltage divider for higher voltages

**Input Impedance:**
- ADC has relatively low input impedance (~200kΩ)
- High-impedance sources may give inaccurate readings
- Use buffer amplifier for high-impedance sensors
- Potentiometer (10kΩ) works well

**Temperature Sensor Accuracy:**
- Factory calibration varies chip-to-chip
- Typical accuracy: ±2-3°C
- Reading affected by chip self-heating
- Better for relative measurements than absolute
- For precise temperature, use external sensor (e.g., DS18B20, LM35)

**Resolution vs. Accuracy:**
- 12-bit = 4096 levels (good resolution)
- Actual accuracy ~8-10 bits due to noise
- Averaging improves effective resolution
- Consider external ADC for >12-bit precision

## Troubleshooting

**Reading stuck at 0 or 4095:**
- Check wiring connections
- Verify sensor is powered
- Check for short circuits

**Noisy readings:**
- Increase `AVERAGE_SAMPLES`
- Add capacitor (0.1µF) near ADC pin
- Keep wires short
- Avoid running near PWM or digital signals

**Temperature seems wrong:**
- Normal range: 20-40°C at room temperature
- Calibration constants may need adjustment
- Chip self-heating can add 5-10°C
- Wait after boot for temperature to stabilize

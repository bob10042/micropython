#!/usr/bin/env python3
"""
Real-time Accelerometer Logger for PyBoard
Logs accelerometer data for 30 seconds to CSV file
"""

import serial
import time
import sys
import os
from datetime import datetime

PORT = 'COM3'
BAUD = 115200
DURATION = 30  # seconds
SAMPLE_RATE_MS = 20  # 50 Hz sampling

def main():
    print("=" * 60)
    print("PyBoard Accelerometer Logger")
    print("=" * 60)
    print(f"Port: {PORT}")
    print(f"Duration: {DURATION} seconds")
    print(f"Sample interval: {SAMPLE_RATE_MS}ms (~{1000//SAMPLE_RATE_MS} Hz)")
    print()

    # Create output filename with timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_file = f"accel_log_{timestamp}.csv"
    output_path = os.path.join(os.path.dirname(__file__), output_file)

    try:
        ser = serial.Serial(PORT, BAUD, timeout=2)
        time.sleep(0.5)
        ser.write(b'\x03\x03')  # Ctrl-C to interrupt
        time.sleep(0.3)
        ser.reset_input_buffer()
        print(f"Connected to {PORT}")
    except Exception as e:
        print(f"ERROR: Cannot open {PORT}: {e}")
        return 1

    # Calculate number of samples
    num_samples = (DURATION * 1000) // SAMPLE_RATE_MS

    # MicroPython code to stream accelerometer data
    accel_code = f'''
import pyb
import time
import math

accel = pyb.Accel()
led = pyb.LED(1)

# Warm up accelerometer
for _ in range(10):
    accel.x()
    time.sleep_ms(10)

print("ACCEL_START")
start = time.ticks_ms()

for i in range({num_samples}):
    t = time.ticks_diff(time.ticks_ms(), start)
    x = accel.x()
    y = accel.y()
    z = accel.z()

    # Toggle LED every 25 samples (~0.5 sec)
    if i % 25 == 0:
        led.toggle()

    print("D", t, x, y, z)

    # Wait for next sample
    elapsed = time.ticks_diff(time.ticks_ms(), start)
    target = (i + 1) * {SAMPLE_RATE_MS}
    if target > elapsed:
        time.sleep_ms(target - elapsed)

led.off()
print("ACCEL_END")
'''

    # Send code using paste mode
    print("Uploading logging code to PyBoard...")
    ser.write(b'\x05')  # Ctrl-E paste mode
    time.sleep(0.2)
    ser.reset_input_buffer()

    for line in accel_code.strip().split('\n'):
        ser.write((line + '\r\n').encode())
        time.sleep(0.01)

    ser.write(b'\x04')  # Ctrl-D execute
    time.sleep(0.5)

    # Wait for start signal
    print("Waiting for accelerometer start...")
    started = False
    timeout = time.time() + 5
    while not started and time.time() < timeout:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if 'ACCEL_START' in line:
                started = True
                break

    if not started:
        print("ERROR: Failed to start accelerometer logging")
        ser.close()
        return 1

    print(f"\nLogging to: {output_file}")
    print("-" * 60)
    print("Time(ms)     X       Y       Z      Mag    Status")
    print("-" * 60)

    # Open output file
    samples = []
    start_time = time.time()
    last_print = 0

    with open(output_path, 'w') as f:
        # Write CSV header
        f.write("timestamp_ms,x,y,z,magnitude\n")

        while True:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore').strip()

                if line.startswith('D '):
                    parts = line.split()
                    if len(parts) >= 5:
                        t = int(parts[1])
                        x = int(parts[2])
                        y = int(parts[3])
                        z = int(parts[4])
                        mag = int((x*x + y*y + z*z) ** 0.5)

                        # Save to file
                        f.write(f"{t},{x},{y},{z},{mag}\n")
                        samples.append((t, x, y, z, mag))

                        # Print progress every 500ms
                        if t - last_print >= 500:
                            elapsed = t / 1000
                            remaining = DURATION - elapsed
                            bar_len = int(elapsed / DURATION * 30)
                            bar = '█' * bar_len + '░' * (30 - bar_len)

                            # Detect motion
                            status = "MOTION!" if mag > 30 else "stable"

                            print(f"{t:>7}ms  {x:>4}    {y:>4}    {z:>4}   {mag:>4}   [{bar}] {remaining:.1f}s {status}")
                            last_print = t

                elif 'ACCEL_END' in line:
                    break

            # Timeout safety
            if time.time() - start_time > DURATION + 10:
                print("\nTimeout reached")
                break

            time.sleep(0.001)

    ser.close()

    # Calculate statistics
    print("\n" + "=" * 60)
    print("LOGGING COMPLETE")
    print("=" * 60)

    if samples:
        x_vals = [s[1] for s in samples]
        y_vals = [s[2] for s in samples]
        z_vals = [s[3] for s in samples]
        mag_vals = [s[4] for s in samples]

        print(f"Total samples: {len(samples)}")
        print(f"Duration: {samples[-1][0] / 1000:.2f} seconds")
        print(f"Actual sample rate: {len(samples) / (samples[-1][0] / 1000):.1f} Hz")
        print()
        print("Axis Statistics:")
        print(f"  X: min={min(x_vals):>4}, max={max(x_vals):>4}, avg={sum(x_vals)/len(x_vals):>6.1f}")
        print(f"  Y: min={min(y_vals):>4}, max={max(y_vals):>4}, avg={sum(y_vals)/len(y_vals):>6.1f}")
        print(f"  Z: min={min(z_vals):>4}, max={max(z_vals):>4}, avg={sum(z_vals)/len(z_vals):>6.1f}")
        print(f"  Mag: min={min(mag_vals):>4}, max={max(mag_vals):>4}, avg={sum(mag_vals)/len(mag_vals):>6.1f}")
        print()

        # Detect events
        motion_events = sum(1 for m in mag_vals if m > 30)
        if motion_events > 0:
            print(f"Motion events detected: {motion_events} samples ({100*motion_events/len(samples):.1f}%)")
        else:
            print("No significant motion detected (board was stationary)")

    print()
    print(f"Data saved to: {output_path}")
    print("=" * 60)

    return 0

if __name__ == "__main__":
    sys.exit(main())

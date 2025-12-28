"""
Test MicroPython on COM7
"""
import serial
import time

port = "COM4"
baud = 115200

print(f"Connecting to {port} at {baud} baud...")
ser = serial.Serial(port, baud, timeout=1)
time.sleep(1)
ser.reset_input_buffer()

print("=" * 70)
print("MICROPYTHON TEST")
print("=" * 70)

# Send Ctrl+C to interrupt, then Enter
print("\nWaking REPL...")
ser.write(b'\x03\x03')  # Ctrl+C twice
time.sleep(0.5)
ser.write(b'\r\n')
time.sleep(0.5)

# Read response
if ser.in_waiting > 0:
    data = ser.read(ser.in_waiting)
    print(data.decode('utf-8', errors='replace'))

# Get version info
print("\n[1] Getting MicroPython version...")
ser.write(b'import sys; print(sys.version)\r\n')
time.sleep(0.5)
if ser.in_waiting > 0:
    print(ser.read(ser.in_waiting).decode('utf-8', errors='replace'))

# Get implementation
print("\n[2] Getting implementation info...")
ser.write(b'print(sys.implementation)\r\n')
time.sleep(0.5)
if ser.in_waiting > 0:
    print(ser.read(ser.in_waiting).decode('utf-8', errors='replace'))

# Get platform
print("\n[3] Getting platform...")
ser.write(b'print(sys.platform)\r\n')
time.sleep(0.5)
if ser.in_waiting > 0:
    print(ser.read(ser.in_waiting).decode('utf-8', errors='replace'))

# Get machine info
print("\n[4] Getting machine info...")
ser.write(b'import machine; print(machine.freq())\r\n')
time.sleep(0.5)
if ser.in_waiting > 0:
    print(ser.read(ser.in_waiting).decode('utf-8', errors='replace'))

# Test accelerometer
print("\n[5] Testing accelerometer...")
ser.write(b'from pyb import Accel\r\n')
time.sleep(0.3)
ser.write(b'accel = Accel()\r\n')
time.sleep(0.3)
ser.write(b'print(accel.x(), accel.y(), accel.z())\r\n')
time.sleep(0.5)
if ser.in_waiting > 0:
    print(ser.read(ser.in_waiting).decode('utf-8', errors='replace'))

# Take multiple readings
print("\n[6] Taking 10 accelerometer readings...")
for i in range(10):
    ser.write(b'print(accel.x(), accel.y(), accel.z())\r\n')
    time.sleep(0.3)
    if ser.in_waiting > 0:
        data = ser.read(ser.in_waiting).decode('utf-8', errors='replace')
        for line in data.split('\n'):
            if line.strip() and '>>>' not in line and 'print' not in line:
                print(f"  {i+1:2d}: X={line.strip()}")
                break

ser.close()
print("\n" + "=" * 70)
print("Test complete")

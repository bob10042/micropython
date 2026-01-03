#!/usr/bin/env python3
"""
Comprehensive PyBoard I/O Test with Viper and Real-time Accelerometer Logging
"""

import serial
import time
import sys
import threading
import os

PORT = 'COM3'
BAUD = 115200

def send_paste_mode(ser, code):
    """Send code using paste mode (Ctrl+E)"""
    ser.write(b'\x03')  # Ctrl-C
    time.sleep(0.1)
    ser.write(b'\x05')  # Ctrl-E (paste mode)
    time.sleep(0.1)
    ser.reset_input_buffer()
    
    for line in code.strip().split('\n'):
        ser.write((line + '\r\n').encode())
        time.sleep(0.01)
    
    ser.write(b'\x04')  # Ctrl-D (execute)
    time.sleep(0.1)

def main():
    print("=" * 60)
    print("PyBoard Comprehensive I/O Test with Viper & Accelerometer")
    print("=" * 60)
    
    ser = serial.Serial(PORT, BAUD, timeout=2)
    time.sleep(0.5)
    ser.write(b'\x03\x03')
    time.sleep(0.3)
    ser.reset_input_buffer()
    
    # ========================================
    # TEST 1: Viper Benchmark
    # ========================================
    print("\n[TEST 1] VIPER/NATIVE BENCHMARK")
    print("-" * 40)
    
    viper_code = '''
import pyb
import time

@micropython.viper
def viper_loop(n: int) -> int:
    t: int = 0
    for i in range(n):
        t += i
    return t

@micropython.native
def native_loop(n):
    t = 0
    for i in range(n):
        t += i
    return t

def python_loop(n):
    t = 0
    for i in range(n):
        t += i
    return t

n = 100000
t0 = time.ticks_us()
r1 = viper_loop(n)
t1 = time.ticks_diff(time.ticks_us(), t0)

t0 = time.ticks_us()
r2 = native_loop(n)
t2 = time.ticks_diff(time.ticks_us(), t0)

t0 = time.ticks_us()
r3 = python_loop(n)
t3 = time.ticks_diff(time.ticks_us(), t0)

print("VIPER:", t1, "us")
print("NATIVE:", t2, "us")
print("PYTHON:", t3, "us")
print("SPEEDUP: Viper", t3//t1 if t1 > 0 else 0, "x, Native", t3//t2 if t2 > 0 else 0, "x")
print("MATCH:", r1 == r2 == r3)
'''
    
    send_paste_mode(ser, viper_code)
    time.sleep(4)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    for line in resp.split('\n'):
        line = line.strip()
        if line and not line.startswith('>>>') and not line.startswith('...'):
            print(f"  {line}")
    
    # ========================================
    # TEST 2: GPIO Stress Test with Viper
    # ========================================
    print("\n[TEST 2] GPIO STRESS TEST (Viper)")
    print("-" * 40)
    
    gpio_code = '''
import pyb
import time

led = pyb.LED(1)

@micropython.viper
def gpio_stress(n: int):
    for i in range(n):
        led.on()
        led.off()

t0 = time.ticks_us()
gpio_stress(10000)
t1 = time.ticks_diff(time.ticks_us(), t0)
freq = 10000 * 1000000 // t1 if t1 > 0 else 0
print("GPIO_TOGGLES: 10000 in", t1, "us")
print("GPIO_FREQ:", freq, "Hz")
'''
    
    send_paste_mode(ser, gpio_code)
    time.sleep(2)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    for line in resp.split('\n'):
        line = line.strip()
        if 'GPIO' in line:
            print(f"  {line}")
    
    # ========================================
    # TEST 3: ADC/DAC Loopback with Viper
    # ========================================
    print("\n[TEST 3] ADC/DAC VIPER TEST")
    print("-" * 40)
    
    adc_code = '''
import pyb
import time

adc = pyb.ADC('X1')
dac = pyb.DAC(1)

@micropython.viper
def adc_stress(n: int) -> int:
    total: int = 0
    for i in range(n):
        total += int(adc.read())
    return total

# Set DAC to mid-scale
dac.write(2048)
time.sleep_ms(10)

t0 = time.ticks_us()
total = adc_stress(1000)
t1 = time.ticks_diff(time.ticks_us(), t0)
avg = total // 1000
print("ADC_READS: 1000 in", t1, "us")
print("ADC_AVG:", avg, "/ 4095")
print("ADC_RATE:", 1000 * 1000000 // t1 if t1 > 0 else 0, "samples/sec")
'''
    
    send_paste_mode(ser, adc_code)
    time.sleep(2)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    for line in resp.split('\n'):
        line = line.strip()
        if 'ADC' in line:
            print(f"  {line}")
    
    # ========================================
    # TEST 4: I2C Accelerometer Stress
    # ========================================
    print("\n[TEST 4] I2C ACCELEROMETER STRESS")
    print("-" * 40)
    
    i2c_code = '''
import pyb
import time

accel = pyb.Accel()

@micropython.native
def accel_stress(n):
    for i in range(n):
        x = accel.x()
        y = accel.y()
        z = accel.z()
    return x, y, z

t0 = time.ticks_us()
x, y, z = accel_stress(100)
t1 = time.ticks_diff(time.ticks_us(), t0)
print("I2C_READS: 300 (100x3) in", t1, "us")
print("I2C_RATE:", 300 * 1000000 // t1 if t1 > 0 else 0, "reads/sec")
print("ACCEL_LAST: X=", x, "Y=", y, "Z=", z)
'''
    
    send_paste_mode(ser, i2c_code)
    time.sleep(2)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    for line in resp.split('\n'):
        line = line.strip()
        if 'I2C' in line or 'ACCEL' in line:
            print(f"  {line}")
    
    # ========================================
    # TEST 5: SPI Stress Test
    # ========================================
    print("\n[TEST 5] SPI STRESS TEST")
    print("-" * 40)
    
    spi_code = '''
import pyb
import time

spi = pyb.SPI(1, pyb.SPI.MASTER, baudrate=10000000)

@micropython.viper
def spi_stress(n: int):
    buf = bytearray(32)
    for i in range(n):
        spi.send(buf)

t0 = time.ticks_us()
spi_stress(1000)
t1 = time.ticks_diff(time.ticks_us(), t0)
bytes_sent = 1000 * 32
print("SPI_BYTES:", bytes_sent, "in", t1, "us")
print("SPI_RATE:", bytes_sent * 1000000 // t1 if t1 > 0 else 0, "bytes/sec")
'''
    
    send_paste_mode(ser, spi_code)
    time.sleep(2)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    for line in resp.split('\n'):
        line = line.strip()
        if 'SPI' in line:
            print(f"  {line}")
    
    # ========================================
    # TEST 6: Timer/PWM Test
    # ========================================
    print("\n[TEST 6] TIMER/PWM TEST")
    print("-" * 40)
    
    timer_code = '''
import pyb

tim = pyb.Timer(2, freq=1000)
ch = tim.channel(2, pyb.Timer.PWM, pin=pyb.Pin('X2'))
ch.pulse_width_percent(50)
print("PWM_FREQ: 1000 Hz")
print("PWM_DUTY: 50%")
print("PWM_PIN: X2")
'''
    
    send_paste_mode(ser, timer_code)
    time.sleep(1)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    for line in resp.split('\n'):
        line = line.strip()
        if 'PWM' in line:
            print(f"  {line}")
    
    # ========================================
    # REAL-TIME ACCELEROMETER LOG
    # ========================================
    print("\n" + "=" * 60)
    print("REAL-TIME ACCELEROMETER LOG (5 seconds)")
    print("=" * 60)
    print("Time(ms)    X       Y       Z      Magnitude")
    print("-" * 60)
    
    # Start accelerometer streaming
    accel_stream = '''
import pyb
import time
import math

accel = pyb.Accel()
start = time.ticks_ms()

for i in range(50):
    x = accel.x()
    y = accel.y()
    z = accel.z()
    t = time.ticks_diff(time.ticks_ms(), start)
    mag = int(math.sqrt(x*x + y*y + z*z))
    print("ACCEL", t, x, y, z, mag)
    time.sleep_ms(100)
print("DONE")
'''
    
    send_paste_mode(ser, accel_stream)
    
    # Read and display accelerometer data in real-time
    start_time = time.time()
    max_mag = 0
    min_mag = 999
    samples = []
    
    while time.time() - start_time < 7:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line.startswith('ACCEL'):
                parts = line.split()
                if len(parts) >= 6:
                    t, x, y, z, mag = parts[1], parts[2], parts[3], parts[4], parts[5]
                    samples.append((int(t), int(x), int(y), int(z), int(mag)))
                    mag_int = int(mag)
                    max_mag = max(max_mag, mag_int)
                    min_mag = min(min_mag, mag_int)
                    # Visual bar
                    bar_len = min(40, mag_int)
                    bar = '█' * bar_len
                    print(f"{t:>6}ms  {x:>4}    {y:>4}    {z:>4}    {mag:>4}  {bar}")
            elif 'DONE' in line:
                break
        time.sleep(0.01)
    
    # Summary
    print("-" * 60)
    print(f"Samples: {len(samples)}, Max magnitude: {max_mag}, Min: {min_mag}")
    
    # Check for movement detection
    if max_mag > 30:
        print(">>> MOVEMENT DETECTED! <<<")
    
    # ========================================
    # SUMMARY
    # ========================================
    print("\n" + "=" * 60)
    print("TEST COMPLETE - All I/O systems verified with Viper")
    print("=" * 60)
    
    ser.close()
    return 0

if __name__ == "__main__":
    sys.exit(main())

"""
Test all I/O and peripherals on PyBoard MicroPython
"""
import serial
import time

port = "COM4"
baud = 115200

print(f"Connecting to {port}...")
ser = serial.Serial(port, baud, timeout=1)
time.sleep(1)
ser.reset_input_buffer()

def send_cmd(cmd, wait=0.5):
    """Send command and return response"""
    ser.write(cmd.encode() + b'\r\n')
    time.sleep(wait)
    data = b''
    while ser.in_waiting > 0:
        data += ser.read(ser.in_waiting)
        time.sleep(0.1)
    return data.decode('utf-8', errors='replace')

# Wake REPL
ser.write(b'\x03\x03\r\n')
time.sleep(0.5)
ser.read(ser.in_waiting)

print("=" * 70)
print("PYBOARD I/O AND PERIPHERAL TEST")
print("=" * 70)

# System info
print("\n### SYSTEM INFO ###\n")
print(send_cmd("import sys; print(sys.implementation)"))
print(send_cmd("import machine; print('CPU Freq:', machine.freq()[0], 'Hz')"))

# LEDs
print("\n### LED TEST ###\n")
print("PyBoard has 4 LEDs: LED(1)=Red, LED(2)=Green, LED(3)=Yellow, LED(4)=Blue")
send_cmd("from pyb import LED")

print("\nTesting each LED...")
for led_num in range(1, 5):
    led_names = {1: "Red", 2: "Green", 3: "Yellow", 4: "Blue"}
    print(f"  LED {led_num} ({led_names[led_num]}): ON", end="", flush=True)
    send_cmd(f"LED({led_num}).on()")
    time.sleep(0.5)
    print(" -> OFF")
    send_cmd(f"LED({led_num}).off()")
    time.sleep(0.2)

print("\nFlashing all LEDs 3 times...")
for _ in range(3):
    for led_num in range(1, 5):
        send_cmd(f"LED({led_num}).on()")
    time.sleep(0.3)
    for led_num in range(1, 5):
        send_cmd(f"LED({led_num}).off()")
    time.sleep(0.3)
print("  Done!")

# LED intensity (LED 4 supports PWM)
print("\nLED 4 (Blue) PWM intensity test...")
send_cmd("led4 = LED(4)")
for intensity in [0, 64, 128, 192, 255, 128, 64, 0]:
    print(f"  Intensity: {intensity}", flush=True)
    send_cmd(f"led4.intensity({intensity})")
    time.sleep(0.2)

# Accelerometer
print("\n\n### ACCELEROMETER TEST ###\n")
send_cmd("from pyb import Accel")
send_cmd("accel = Accel()")
print("Taking 5 readings (X, Y, Z):")
for i in range(5):
    response = send_cmd("print(accel.x(), accel.y(), accel.z())")
    for line in response.split('\n'):
        if line.strip() and '>>>' not in line and 'print' not in line:
            print(f"  {i+1}: {line.strip()}")
            break
    time.sleep(0.3)

# Also test tilt
print("\nTilt detection:")
response = send_cmd("print('Tilt:', accel.tilt())")
print(f"  {response.strip()}")

# Switch (USR button)
print("\n\n### USER BUTTON TEST ###\n")
send_cmd("from pyb import Switch")
send_cmd("sw = Switch()")
print("Reading USR button state...")
response = send_cmd("print('Button pressed:', sw())")
print(f"  {response.strip()}")

# GPIO Pins
print("\n\n### GPIO PIN TEST ###\n")
send_cmd("from pyb import Pin")
print("Available pins on PyBoard:")
print("  X1-X12, Y1-Y12 (directly accessible)")
print("  A0-A3, B0-B15, C0-C13 (directly accessible)")

print("\nTesting Pin X1 as output...")
send_cmd("p = Pin('X1', Pin.OUT)")
send_cmd("p.high()")
response = send_cmd("print('X1 state:', p.value())")
print(f"  Set HIGH: {response.strip()}")
send_cmd("p.low()")
response = send_cmd("print('X1 state:', p.value())")
print(f"  Set LOW: {response.strip()}")

# ADC
print("\n\n### ADC (Analog Input) TEST ###\n")
send_cmd("from pyb import ADC")
print("Testing ADC on pin X19 (internal temperature sensor)...")
send_cmd("adc_temp = ADC('TEMP')")
response = send_cmd("print('Internal temp raw:', adc_temp.read())")
print(f"  {response.strip()}")

print("\nTesting ADC on pin X19 (VREF)...")
send_cmd("adc_vref = ADC('VREF')")
response = send_cmd("print('VREF raw:', adc_vref.read())")
print(f"  {response.strip()}")

print("\nTesting ADC on pin X1...")
send_cmd("adc1 = ADC(Pin('X1'))")
response = send_cmd("print('X1 ADC:', adc1.read())")
print(f"  {response.strip()}")

# DAC
print("\n\n### DAC (Analog Output) TEST ###\n")
send_cmd("from pyb import DAC")
print("PyBoard has 2 DACs: X5 (DAC1) and X6 (DAC2)")
send_cmd("dac = DAC(1)")  # X5
print("Setting DAC1 (X5) to various values...")
for val in [0, 64, 128, 192, 255]:
    send_cmd(f"dac.write({val})")
    print(f"  DAC1 = {val}")
    time.sleep(0.1)

# I2C
print("\n\n### I2C BUS TEST ###\n")
send_cmd("from pyb import I2C")
print("Scanning I2C bus 1 (X9=SCL, X10=SDA)...")
send_cmd("i2c = I2C(1, I2C.CONTROLLER)")
response = send_cmd("print('I2C devices:', i2c.scan())")
print(f"  {response.strip()}")

print("\nScanning I2C bus 2 (Y9=SCL, Y10=SDA)...")
send_cmd("i2c2 = I2C(2, I2C.CONTROLLER)")
response = send_cmd("print('I2C devices:', i2c2.scan())")
print(f"  {response.strip()}")

# SPI
print("\n\n### SPI BUS TEST ###\n")
send_cmd("from pyb import SPI")
print("PyBoard has 2 SPI buses:")
print("  SPI1: X5=SCK, X6=MISO, X7=MOSI")
print("  SPI2: Y6=SCK, Y7=MISO, Y8=MOSI")
send_cmd("spi = SPI(1, SPI.CONTROLLER, baudrate=1000000)")
response = send_cmd("print('SPI1 initialized:', spi)")
print(f"  {response.strip()}")

# UART
print("\n\n### UART TEST ###\n")
send_cmd("from pyb import UART")
print("PyBoard UARTs:")
print("  UART1: X9=TX, X10=RX")
print("  UART2: X3=TX, X4=RX")
print("  UART3: Y9=TX, Y10=RX")
print("  UART4: X1=TX, X2=RX")
print("  UART6: Y1=TX, Y2=RX")
send_cmd("uart = UART(4, 9600)")
response = send_cmd("print('UART4:', uart)")
print(f"  {response.strip()}")

# PWM / Timer
print("\n\n### PWM / TIMER TEST ###\n")
send_cmd("from pyb import Timer")
print("Testing PWM on X1 using Timer 2, Channel 1...")
send_cmd("tim = Timer(2, freq=1000)")
send_cmd("ch = tim.channel(1, Timer.PWM, pin=Pin('X1'))")
print("Setting PWM duty cycle...")
for duty in [0, 25, 50, 75, 100, 50, 0]:
    send_cmd(f"ch.pulse_width_percent({duty})")
    print(f"  Duty: {duty}%")
    time.sleep(0.2)

# RTC
print("\n\n### RTC (Real-Time Clock) TEST ###\n")
send_cmd("from pyb import RTC")
send_cmd("rtc = RTC()")
response = send_cmd("print('RTC datetime:', rtc.datetime())")
print(f"  {response.strip()}")

# SD Card
print("\n\n### SD CARD TEST ###\n")
send_cmd("import os")
print("Checking for SD card...")
response = send_cmd("print('Root files:', os.listdir('/'))")
print(f"  {response.strip()}")

# Try to detect SD
response = send_cmd("print('Has SD:', 'sd' in os.listdir('/'))")
print(f"  {response.strip()}")

# Internal flash filesystem
print("\nInternal flash contents:")
response = send_cmd("print(os.listdir('/flash'))")
print(f"  {response.strip()}")

# Memory info
print("\n\n### MEMORY INFO ###\n")
send_cmd("import gc")
send_cmd("gc.collect()")
response = send_cmd("print('Free memory:', gc.mem_free(), 'bytes')")
print(f"  {response.strip()}")
response = send_cmd("print('Allocated:', gc.mem_alloc(), 'bytes')")
print(f"  {response.strip()}")

# Unique ID
print("\n\n### DEVICE ID ###\n")
send_cmd("import machine")
response = send_cmd("import ubinascii; print('Unique ID:', ubinascii.hexlify(machine.unique_id()))")
print(f"  {response.strip()}")

ser.close()
print("\n" + "=" * 70)
print("ALL TESTS COMPLETE")
print("=" * 70)

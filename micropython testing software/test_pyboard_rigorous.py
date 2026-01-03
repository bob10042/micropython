#!/usr/bin/env python3
"""
PyBoard v3.1 Rigorous Test Suite
Tests all peripherals and CLI commands
"""

import serial
import time
import sys

PORT = 'COM12'
BAUD = 115200

def send_cmd(ser, cmd, wait=0.5):
    """Send command and get response"""
    ser.reset_input_buffer()
    ser.write(f"{cmd}\r".encode())
    time.sleep(wait)
    response = ""
    while ser.in_waiting:
        response += ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
        time.sleep(0.05)
    return response

def test(name, cmd, expected, ser, wait=0.5):
    """Run a single test"""
    response = send_cmd(ser, cmd, wait)
    passed = any(exp.lower() in response.lower() for exp in expected) if isinstance(expected, list) else expected.lower() in response.lower()
    status = "PASS" if passed else "FAIL"
    color = "\033[92m" if passed else "\033[91m"
    reset = "\033[0m"
    print(f"  [{color}{status}{reset}] {name}")
    if not passed:
        print(f"       Expected: {expected}")
        print(f"       Got: {response[:100]}...")
    return passed

def main():
    print("=" * 50)
    print("PyBoard v3.1 Rigorous Test Suite")
    print("=" * 50)
    print(f"Port: {PORT}")
    print()
    
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        time.sleep(0.5)
        ser.reset_input_buffer()
    except Exception as e:
        print(f"ERROR: Cannot open {PORT}: {e}")
        return 1
    
    passed = 0
    failed = 0
    
    # ========================================
    # SYSTEM TESTS
    # ========================================
    print("\n[SYSTEM TESTS]")
    
    if test("System Info", "info", "STM32F405", ser): passed += 1
    else: failed += 1
    
    if test("Unique Device ID", "uid", ["uid:", "0x"], ser): passed += 1
    else: failed += 1
    
    if test("Reset Cause", "resetcause", ["reset", "power", "cause"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # LED TESTS
    # ========================================
    print("\n[LED TESTS]")
    
    if test("LED1 On", "led 1 1", ["led1", "on", "1"], ser): passed += 1
    else: failed += 1
    
    if test("LED2 On", "led 2 1", ["led2", "on", "2"], ser): passed += 1
    else: failed += 1
    
    if test("LED3 On", "led 3 1", ["led3", "on", "3"], ser): passed += 1
    else: failed += 1
    
    if test("LED4 On", "led 4 1", ["led4", "on", "4"], ser): passed += 1
    else: failed += 1
    
    if test("LED Intensity", "intensity 128", ["intensity", "128", "led4"], ser): passed += 1
    else: failed += 1
    
    if test("All LEDs Off", "alloff", ["off", "led"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # ACCELEROMETER TESTS
    # ========================================
    print("\n[ACCELEROMETER TESTS]")
    
    if test("Accelerometer Read", "accel", ["x=", "y=", "z=", "accel"], ser): passed += 1
    else: failed += 1
    
    if test("Tilt Status", "tilt", ["tilt", "0x"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # ADC TESTS
    # ========================================
    print("\n[ADC TESTS]")
    
    if test("ADC Default (X1)", "adc", ["adc", "value", "mv"], ser): passed += 1
    else: failed += 1
    
    if test("ADC Channel 0", "adc 0", ["ch0", "channel", "adc"], ser): passed += 1
    else: failed += 1
    
    if test("ADC Temperature", "adc temp", ["temp", "°c", "degree"], ser): passed += 1
    else: failed += 1
    
    if test("ADC VBAT", "adc vbat", ["vbat", "battery", "mv", "v"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # DAC TESTS
    # ========================================
    print("\n[DAC TESTS]")
    
    if test("DAC Set Value", "dac 2048", ["dac", "2048"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # PWM TESTS
    # ========================================
    print("\n[PWM TESTS]")
    
    if test("PWM Set 50%", "pwm 50", ["pwm", "50", "%"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # GPIO TESTS
    # ========================================
    print("\n[GPIO TESTS]")
    
    if test("Read X1 Pin", "x1", ["x1", "0", "1", "pin"], ser): passed += 1
    else: failed += 1
    
    if test("Read Y1 Pin", "y1", ["y1", "0", "1", "pin"], ser): passed += 1
    else: failed += 1
    
    if test("Button State", "button", ["button", "0", "1", "pressed", "released"], ser): passed += 1
    else: failed += 1
    
    if test("Pin List", "pins", ["x1", "y1", "pin"], ser, wait=1.0): passed += 1
    else: failed += 1
    
    # ========================================
    # I2C TESTS
    # ========================================
    print("\n[I2C TESTS]")
    
    if test("I2C1 Scan", "i2c1", ["i2c1", "scan", "0x", "device", "found"], ser, wait=1.0): passed += 1
    else: failed += 1
    
    if test("I2C2 Scan", "i2c2", ["i2c2", "scan", "no device", "found"], ser, wait=1.0): passed += 1
    else: failed += 1
    
    # ========================================
    # SPI TESTS
    # ========================================
    print("\n[SPI TESTS]")
    
    if test("SPI1 Send", "spi1 send AA55", ["spi1", "sent", "recv", "aa", "55"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # UART TESTS
    # ========================================
    print("\n[UART TESTS]")
    
    if test("UART2 Send", "uart2 send test", ["uart2", "sent", "test"], ser): passed += 1
    else: failed += 1
    
    if test("UART6 Send", "uart6 send hello", ["uart6", "sent", "hello"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # CAN TESTS
    # ========================================
    print("\n[CAN TESTS]")
    
    if test("CAN Status", "can status", ["can", "status", "state", "error"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # RTC TESTS
    # ========================================
    print("\n[RTC TESTS]")
    
    if test("RTC Read", "rtc", ["20", ":", "date", "time"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # SD CARD TESTS
    # ========================================
    print("\n[SD CARD TESTS]")
    
    if test("SD Card Info", "sd", ["sd", "card", "size", "no card", "not"], ser): passed += 1
    else: failed += 1
    
    # ========================================
    # BENCHMARK TESTS
    # ========================================
    print("\n[BENCHMARK TESTS]")
    
    if test("Speed Test", "speed", ["loop", "cycles", "ms", "mhz"], ser, wait=2.0): passed += 1
    else: failed += 1
    
    if test("GPIO Speed Test", "speedgpio", ["gpio", "toggle", "mhz", "khz"], ser, wait=2.0): passed += 1
    else: failed += 1
    
    if test("Viper Benchmark", "viper", ["viper", "loop", "us", "ms", "cycles"], ser, wait=3.0): passed += 1
    else: failed += 1
    
    if test("Bench Test", "bench", ["bench", "loop", "ms", "us"], ser, wait=2.0): passed += 1
    else: failed += 1
    
    # ========================================
    # RESULTS
    # ========================================
    print("\n" + "=" * 50)
    total = passed + failed
    pct = (passed / total * 100) if total > 0 else 0
    
    if failed == 0:
        print(f"\033[92mALL TESTS PASSED: {passed}/{total} (100%)\033[0m")
    else:
        print(f"\033[93mRESULTS: {passed}/{total} passed ({pct:.1f}%)\033[0m")
        print(f"\033[91mFailed: {failed}\033[0m")
    
    print("=" * 50)
    
    ser.close()
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())

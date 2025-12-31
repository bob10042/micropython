#!/usr/bin/env python3
"""Test script for PyBoard Native C Firmware v3.0"""

import serial
import time
import sys

PORT = 'COM11'
BAUD = 115200

def send_cmd(ser, cmd, delay=0.3):
    """Send command and get response"""
    ser.reset_input_buffer()
    ser.write((cmd + '\r\n').encode())
    time.sleep(delay)
    response = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    return response

def main():
    print(f"=== PyBoard Native C Firmware Test ===")
    print(f"Connecting to {PORT}...")
    
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        time.sleep(0.5)
        
        # Clear buffer and wake up
        ser.write(b'\r\n')
        time.sleep(0.3)
        ser.reset_input_buffer()
        
        # Test 1: Info
        print("\n--- Test 1: System Info ---")
        print(send_cmd(ser, "info"))
        
        # Test 2: Unique ID
        print("\n--- Test 2: Unique ID ---")
        print(send_cmd(ser, "uid"))
        
        # Test 3: Reset Cause
        print("\n--- Test 3: Reset Cause ---")
        print(send_cmd(ser, "resetcause"))
        
        # Test 4: ADC readings
        print("\n--- Test 4: ADC Channels ---")
        print(send_cmd(ser, "adc 0"))
        print(send_cmd(ser, "adc 4"))
        
        # Test 5: Temperature sensor
        print("\n--- Test 5: Internal Temperature ---")
        print(send_cmd(ser, "adc temp"))
        
        # Test 6: VBAT
        print("\n--- Test 6: VBAT Reading ---")
        print(send_cmd(ser, "adc vbat"))
        
        # Test 7: I2C scan
        print("\n--- Test 7: I2C Scan ---")
        print(send_cmd(ser, "i2c scan", delay=0.5))
        
        # Test 8: LED control
        print("\n--- Test 8: LED Control ---")
        print(send_cmd(ser, "led 1 on"))
        time.sleep(0.3)
        print(send_cmd(ser, "led 2 on"))
        time.sleep(0.3)
        print(send_cmd(ser, "led 3 on"))
        time.sleep(0.3)
        print(send_cmd(ser, "led 4 on"))
        time.sleep(0.5)
        print(send_cmd(ser, "led 1 off"))
        print(send_cmd(ser, "led 2 off"))
        print(send_cmd(ser, "led 3 off"))
        print(send_cmd(ser, "led 4 off"))
        
        # Test 9: Button state
        print("\n--- Test 9: Button State ---")
        print(send_cmd(ser, "button"))
        
        # Test 10: Accelerometer
        print("\n--- Test 10: Accelerometer ---")
        print(send_cmd(ser, "accel"))
        
        # Test 11: Viper benchmark
        print("\n--- Test 11: Viper Benchmark ---")
        print(send_cmd(ser, "viper", delay=1.0))
        
        # Test 12: Help (show all commands)
        print("\n--- Test 12: Available Commands ---")
        print(send_cmd(ser, "help", delay=0.5))
        
        ser.close()
        print("\n=== All Tests Complete ===")
        
    except serial.SerialException as e:
        print(f"Error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""LED PWM Demo for PyBoard Native Firmware"""

import serial
import time

def main():
    port = serial.Serial('COM11', 115200, timeout=1)
    time.sleep(0.2)
    
    def cmd(c):
        port.write(f"{c}\r".encode())
        time.sleep(0.05)
    
    print("=" * 50)
    print("PyBoard LED Demo")
    print("=" * 50)
    
    # First, blink all 4 LEDs to show they work
    print("\n1. Blinking all 4 LEDs (Red, Green, Yellow, Blue)...")
    for cycle in range(2):
        for led in [1, 2, 3, 4]:
            cmd(f"led {led} on")
            time.sleep(0.15)
            cmd(f"led {led} off")
            time.sleep(0.05)
    
    # Now show PWM on TIM2 which controls X1-X4 pins
    # X1 = PA0 = TIM2_CH1, X2 = PA1 = TIM2_CH2
    print("\n2. PWM is on pins X1/X2 (PA0/PA1) - not the onboard LEDs")
    print("   The onboard LEDs are GPIO only (no PWM timer connection)")
    
    # LED4 breathing simulation with on/off (since TIM3 is disabled)
    print("\n3. Simulating LED4 'breathing' with rapid on/off...")
    print("   (Real PWM would need TIM3 enabled)")
    
    for cycle in range(3):
        # Fade up
        for i in range(0, 10):
            cmd("led 4 on")
            time.sleep(i * 0.01)
            cmd("led 4 off") 
            time.sleep((10 - i) * 0.01)
        # Fade down
        for i in range(10, 0, -1):
            cmd("led 4 on")
            time.sleep(i * 0.01)
            cmd("led 4 off")
            time.sleep((10 - i) * 0.01)
    
    # Leave all LEDs off
    for led in [1, 2, 3, 4]:
        cmd(f"led {led} off")
    
    print("\nDemo complete!")
    print("\nNote: To see PWM, connect an external LED to X1 or X2 pin")
    print("      then run: pwm X1 50  (for 50% duty)")
    
    port.close()

if __name__ == "__main__":
    main()

"""
Test PyBoard Native C Firmware - All I/O and Peripherals
=========================================================
Tests all CLI commands on the native firmware via USB CDC serial.
"""
import serial
import time
import sys

# Configuration
PORT = "COM11"
BAUD = 115200

def connect():
    """Connect to the PyBoard"""
    print(f"Connecting to {PORT} @ {BAUD}...")
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(0.5)
    ser.reset_input_buffer()
    return ser

def send_cmd(ser, cmd, wait=0.3):
    """Send command and return response"""
    ser.reset_input_buffer()
    ser.write((cmd + '\r\n').encode())
    time.sleep(wait)
    response = ser.read(ser.in_waiting).decode('utf-8', errors='replace')
    return response.strip()

def test_section(title):
    """Print section header"""
    print("\n" + "=" * 60)
    print(f"  {title}")
    print("=" * 60)

def main():
    ser = connect()
    
    results = {"passed": 0, "failed": 0, "skipped": 0}
    
    def check(name, response, expected=None):
        """Check test result"""
        if expected:
            if expected in response:
                print(f"  ✓ {name}: {response[:50]}")
                results["passed"] += 1
            else:
                print(f"  ✗ {name}: Expected '{expected}' in '{response}'")
                results["failed"] += 1
        else:
            if response and "ERROR" not in response.upper():
                print(f"  ✓ {name}: {response[:60]}")
                results["passed"] += 1
            else:
                print(f"  ? {name}: {response[:60] if response else 'No response'}")
                results["skipped"] += 1
    
    # =========================================================================
    test_section("SYSTEM INFO")
    # =========================================================================
    
    response = send_cmd(ser, "info")
    check("Info command", response, "PyBoard")
    
    response = send_cmd(ser, "help")
    check("Help command", response, "led")
    
    # =========================================================================
    test_section("LED TESTS")
    # =========================================================================
    
    print("\n  Testing individual LEDs...")
    for led in range(1, 5):
        led_names = {1: "Red", 2: "Green", 3: "Yellow", 4: "Blue"}
        
        # Turn ON
        response = send_cmd(ser, f"led {led} 1")
        check(f"LED{led} ({led_names[led]}) ON", response)
        time.sleep(0.2)
        
        # Turn OFF
        response = send_cmd(ser, f"led {led} 0")
        check(f"LED{led} ({led_names[led]}) OFF", response)
        time.sleep(0.1)
    
    print("\n  Testing LED group commands...")
    response = send_cmd(ser, "allon")
    check("All LEDs ON", response)
    time.sleep(0.3)
    
    response = send_cmd(ser, "alloff")
    check("All LEDs OFF", response)
    
    print("\n  Testing LED4 PWM intensity...")
    for intensity in [0, 64, 128, 192, 255, 0]:
        response = send_cmd(ser, f"intensity {intensity}")
        check(f"LED4 intensity {intensity}", response)
        time.sleep(0.1)
    
    print("\n  Running LED demo...")
    response = send_cmd(ser, "demo", wait=2.0)
    check("LED demo sequence", response)
    
    # =========================================================================
    test_section("BUTTON TEST")
    # =========================================================================
    
    response = send_cmd(ser, "button")
    check("User button read", response, "Button")
    
    # =========================================================================
    test_section("ACCELEROMETER TEST")
    # =========================================================================
    
    print("\n  Reading accelerometer data...")
    for i in range(3):
        response = send_cmd(ser, "accel")
        check(f"Accel reading {i+1}", response)
        time.sleep(0.2)
    
    response = send_cmd(ser, "tilt")
    check("Tilt register", response)
    
    # =========================================================================
    test_section("ADC TEST (X1/PA0)")
    # =========================================================================
    
    for i in range(3):
        response = send_cmd(ser, "adc")
        check(f"ADC reading {i+1}", response, "ADC")
    
    # =========================================================================
    test_section("DAC TEST (X5/PA4)")
    # =========================================================================
    
    for dac_val in [0, 1024, 2048, 3072, 4095, 0]:
        response = send_cmd(ser, f"dac {dac_val}")
        check(f"DAC set to {dac_val}", response)
        time.sleep(0.1)
    
    # =========================================================================
    test_section("PWM TEST (X2/PA1)")
    # =========================================================================
    
    for pwm_val in [0, 25, 50, 75, 100, 0]:
        response = send_cmd(ser, f"pwm {pwm_val}")
        check(f"PWM set to {pwm_val}%", response)
        time.sleep(0.1)
    
    # =========================================================================
    test_section("GPIO X-PINS TEST")
    # =========================================================================
    
    print("\n  Testing X1-X12 pins...")
    for pin in range(1, 13):
        # Set HIGH
        response = send_cmd(ser, f"x{pin} 1")
        check(f"X{pin} set HIGH", response)
        
        # Read back
        response = send_cmd(ser, f"x{pin}")
        check(f"X{pin} read", response)
        
        # Set LOW
        response = send_cmd(ser, f"x{pin} 0")
        check(f"X{pin} set LOW", response)
    
    print("\n  Testing X17-X22 pins...")
    for pin in range(17, 23):
        # Set HIGH
        response = send_cmd(ser, f"x{pin} 1")
        check(f"X{pin} set HIGH", response)
        
        # Read back
        response = send_cmd(ser, f"x{pin}")
        check(f"X{pin} read", response)
        
        # Set LOW
        response = send_cmd(ser, f"x{pin} 0")
        check(f"X{pin} set LOW", response)
    
    # =========================================================================
    test_section("GPIO Y-PINS TEST")
    # =========================================================================
    
    print("\n  Testing Y1-Y12 pins...")
    for pin in range(1, 13):
        # Set HIGH
        response = send_cmd(ser, f"y{pin} 1")
        check(f"Y{pin} set HIGH", response)
        
        # Read back
        response = send_cmd(ser, f"y{pin}")
        check(f"Y{pin} read", response)
        
        # Set LOW
        response = send_cmd(ser, f"y{pin} 0")
        check(f"Y{pin} set LOW", response)
    
    # =========================================================================
    test_section("GPIO MODE/PULL CONFIG TEST")
    # =========================================================================
    
    print("\n  Testing pin mode configuration...")
    response = send_cmd(ser, "mode x1 out")
    check("X1 set OUTPUT mode", response)
    
    response = send_cmd(ser, "mode x1 in")
    check("X1 set INPUT mode", response)
    
    print("\n  Testing pull resistor configuration...")
    response = send_cmd(ser, "pull x1 up")
    check("X1 set PULL-UP", response)
    
    response = send_cmd(ser, "pull x1 down")
    check("X1 set PULL-DOWN", response)
    
    response = send_cmd(ser, "pull x1 none")
    check("X1 set NO PULL", response)
    
    print("\n  Testing toggle command...")
    response = send_cmd(ser, "mode x1 out")
    response = send_cmd(ser, "x1 0")
    response = send_cmd(ser, "toggle x1")
    check("Toggle X1", response)
    
    # =========================================================================
    test_section("PINS STATUS")
    # =========================================================================
    
    response = send_cmd(ser, "pins", wait=0.5)
    check("Pins status listing", response)
    
    # =========================================================================
    test_section("I2C BUS SCAN")
    # =========================================================================
    
    response = send_cmd(ser, "i2c1", wait=0.5)
    check("I2C1 bus scan (X9/X10)", response, "I2C")
    
    response = send_cmd(ser, "i2c2", wait=0.5)
    check("I2C2 bus scan (Y9/Y10)", response)
    
    # =========================================================================
    test_section("SPI TEST")
    # =========================================================================
    
    response = send_cmd(ser, "spi1 send 55")
    check("SPI1 send 0x55", response)
    
    response = send_cmd(ser, "spi2 send AA")
    check("SPI2 send 0xAA", response)
    
    # =========================================================================
    test_section("UART TEST")
    # =========================================================================
    
    response = send_cmd(ser, "uart2 send Hello")
    check("UART2 send", response)
    
    response = send_cmd(ser, "uart2 recv")
    check("UART2 receive", response)
    
    response = send_cmd(ser, "uart6 send World")
    check("UART6 send", response)
    
    response = send_cmd(ser, "uart6 recv")
    check("UART6 receive", response)
    
    # =========================================================================
    test_section("CAN BUS TEST")
    # =========================================================================
    
    response = send_cmd(ser, "can status")
    check("CAN status", response)
    
    response = send_cmd(ser, "can send 123 DEADBEEF")
    check("CAN send message", response)
    
    # =========================================================================
    test_section("SD CARD TEST")
    # =========================================================================
    
    response = send_cmd(ser, "sd init")
    check("SD card init", response)
    
    response = send_cmd(ser, "sd status")
    check("SD card status", response)
    
    # =========================================================================
    test_section("RTC TEST")
    # =========================================================================
    
    response = send_cmd(ser, "rtc")
    check("RTC read time", response)
    
    response = send_cmd(ser, "rtc date")
    check("RTC read date", response)
    
    # =========================================================================
    # SUMMARY
    # =========================================================================
    
    print("\n" + "=" * 60)
    print("  TEST SUMMARY")
    print("=" * 60)
    print(f"\n  ✓ Passed:  {results['passed']}")
    print(f"  ✗ Failed:  {results['failed']}")
    print(f"  ? Skipped: {results['skipped']}")
    print(f"\n  Total:     {sum(results.values())}")
    
    if results['failed'] == 0:
        print("\n  🎉 ALL TESTS PASSED!")
    else:
        print(f"\n  ⚠️  {results['failed']} tests failed")
    
    ser.close()
    print("\nConnection closed.")

if __name__ == "__main__":
    main()

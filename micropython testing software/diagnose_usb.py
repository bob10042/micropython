"""
USB Connection Diagnostics for PyBoard Native Firmware
=======================================================
Run this after flashing to check USB connectivity issues.
"""
import sys
import time

def check_serial():
    """Check if pyserial is installed and list all COM ports"""
    try:
        import serial.tools.list_ports
        print("=" * 60)
        print("  USB/COM Port Diagnostics")
        print("=" * 60)
        
        ports = list(serial.tools.list_ports.comports())
        
        if not ports:
            print("\n⚠️  NO COM PORTS DETECTED!")
            print("\nPossible causes:")
            print("  1. Board not connected or needs power cycle")
            print("  2. USB cable is charge-only (no data)")
            print("  3. Driver issue - check Device Manager")
            print("  4. Firmware not running (stuck in bootloader?)")
            print("\nTry:")
            print("  - Unplug the board, wait 5 seconds, plug back in")
            print("  - Try a different USB cable")
            print("  - Check Windows Device Manager for errors")
            return None
        
        print(f"\nFound {len(ports)} COM port(s):\n")
        pyboard_port = None
        
        for p in ports:
            status = ""
            
            # Check for common PyBoard/STM32 identifiers
            desc = p.description.lower()
            if 'stm' in desc or 'pyboard' in desc or 'serial' in desc:
                status = "  ← Likely your PyBoard!"
                pyboard_port = p.device
            if 'usb' in desc:
                status = status or "  ← USB Serial device"
                
            print(f"  {p.device}:")
            print(f"    Description: {p.description}")
            print(f"    VID:PID:     {p.vid}:{p.pid}" if p.vid else "    VID:PID:     N/A")
            print(f"    Manufacturer:{p.manufacturer}" if p.manufacturer else "")
            if status:
                print(f"    {status}")
            print()
        
        return pyboard_port
        
    except ImportError:
        print("ERROR: pyserial not installed!")
        print("Run: pip install pyserial")
        return None

def test_connection(port):
    """Try to connect to the board"""
    import serial
    
    print("=" * 60)
    print(f"  Testing connection to {port}")
    print("=" * 60)
    
    try:
        ser = serial.Serial(port, 115200, timeout=2)
        time.sleep(0.5)
        
        # Clear buffer
        ser.reset_input_buffer()
        
        # Send a simple command
        ser.write(b"\r\n")
        time.sleep(0.3)
        
        # Try to read response
        response = ser.read(ser.in_waiting or 100)
        
        if response:
            print(f"\n✓ Board responded!")
            print(f"\nResponse ({len(response)} bytes):")
            print("-" * 40)
            try:
                text = response.decode('utf-8', errors='replace')
                print(text)
            except:
                print(f"Raw bytes: {response}")
            print("-" * 40)
            
            # Try info command
            print("\nSending 'info' command...")
            ser.reset_input_buffer()
            ser.write(b"info\r\n")
            time.sleep(0.5)
            response = ser.read(ser.in_waiting or 500)
            if response:
                print(response.decode('utf-8', errors='replace'))
            
            print("\n✓ USB CDC is working!")
            return True
        else:
            print("\n⚠️  No response from board!")
            print("\nPossible causes:")
            print("  1. Firmware crashed or stuck")
            print("  2. Wrong baud rate")
            print("  3. USB CDC not properly initialized")
            return False
            
    except serial.SerialException as e:
        print(f"\n✗ Connection failed: {e}")
        print("\nPossible causes:")
        print("  1. Port in use by another application")
        print("  2. Permission issue")
        print("  3. Board disconnected")
        return False
    finally:
        try:
            ser.close()
        except:
            pass

def check_dfu_mode():
    """Check if board is stuck in DFU mode"""
    print("\n" + "=" * 60)
    print("  Checking for DFU Mode")
    print("=" * 60)
    
    try:
        import subprocess
        result = subprocess.run(
            ["dfu-util", "-l"],
            capture_output=True,
            text=True,
            timeout=5
        )
        
        if "0483:df11" in result.stdout:
            print("\n⚠️  Board is in DFU mode!")
            print("   The firmware might not have flashed correctly,")
            print("   or BOOT0 is held high.")
            print("\nTo exit DFU mode:")
            print("  1. Release BOOT0 button/jumper")
            print("  2. Press RESET button")
            print("  Or just unplug and replug USB cable")
            return True
        else:
            print("\n✓ Board is NOT in DFU mode (good)")
            return False
            
    except FileNotFoundError:
        print("\n  dfu-util not found - skipping DFU check")
        return False
    except Exception as e:
        print(f"\n  Could not check DFU: {e}")
        return False

def main():
    print("\n" + "=" * 60)
    print("  PyBoard Native Firmware - USB Diagnostics")
    print("=" * 60)
    print("\nPlease ensure:")
    print("  1. Board is connected via USB")
    print("  2. You've waited at least 3 seconds after plugging in")
    print("  3. No other program is using the COM port")
    print()
    
    input("Press Enter to start diagnostics...")
    
    # Check for DFU mode
    check_dfu_mode()
    
    # List COM ports
    port = check_serial()
    
    if port:
        print(f"\n→ Testing {port}...")
        test_connection(port)
    else:
        print("\n" + "=" * 60)
        print("  TROUBLESHOOTING STEPS")
        print("=" * 60)
        print("""
1. POWER CYCLE
   - Unplug the USB cable
   - Wait 5 seconds
   - Plug it back in
   - Wait 3 seconds
   - Run this script again

2. CHECK USB CABLE
   - Some cables are charge-only (no data)
   - Try a different USB cable

3. CHECK DEVICE MANAGER
   - Open Device Manager (devmgmt.msc)
   - Look under "Ports (COM & LPT)"
   - Look for devices with yellow warning icons
   - If you see "STM32 Virtual COM Port" with error:
     - Right-click → Update Driver
     - Or install STM32 Virtual COM Port driver

4. CHECK IF BOARD IS IN DFU MODE
   - LEDs should flash 3 times on boot
   - If no LED activity, firmware might not be running

5. RE-FLASH THE FIRMWARE
   - Hold BOOT0 button
   - Press RESET
   - Release BOOT0
   - Flash using DFU mode
""")

if __name__ == "__main__":
    main()

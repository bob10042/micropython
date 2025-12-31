import subprocess
import re

print("=== Checking for STM32 DFU Device ===\n")

# Check with dfu-util if available
print("Checking for dfu-util...")
try:
    result = subprocess.run(['dfu-util', '--list'], 
                          capture_output=True, 
                          text=True, 
                          timeout=5)
    if result.returncode == 0:
        print("dfu-util found!\n")
        print(result.stdout)
        if '0483' in result.stdout:
            print("\n✓ STM32 device found in DFU mode!")
        else:
            print("\n✗ No STM32 DFU device detected.")
            print("  Try entering bootloader mode:")
            print("  - Hold BOOT button")
            print("  - Press and release RESET")
            print("  - Release BOOT button")
    else:
        print("dfu-util error:", result.stderr)
except FileNotFoundError:
    print("dfu-util not installed.\n")
    print("To install: pip install pyusb")
    print("Or download: https://dfu-util.sourceforge.net/\n")
except Exception as e:
    print(f"Error running dfu-util: {e}\n")

# Also check Windows USB devices
print("\n=== Windows USB Devices (STM-related) ===")
try:
    result = subprocess.run(
        ['powershell', '-Command', 
         'Get-PnpDevice | Where-Object {$_.FriendlyName -like "*STM*" -or $_.FriendlyName -like "*DFU*" -or $_.InstanceId -like "*0483*"} | Select-Object FriendlyName, Status, InstanceId | Format-List'],
        capture_output=True,
        text=True,
        timeout=10
    )
    if result.stdout.strip():
        print(result.stdout)
    else:
        print("No STM32 or DFU devices found in Windows device manager.")
except Exception as e:
    print(f"Error checking devices: {e}")

print("\n=== Current COM8 Status ===")
import serial.tools.list_ports
ports = list(serial.tools.list_ports.comports())
for p in ports:
    if 'COM8' in p.device:
        print(f"Device: {p.device}")
        print(f"Description: {p.description}")
        print(f"VID:PID: {p.vid:04X}:{p.pid:04X}")
        print(f"Serial Number: {p.serial_number}")
        
        # Decode the VID:PID
        if p.vid == 0x0483:
            print("\n✓ This is an ST Microelectronics device!")
            if p.pid == 0x5740:
                print("  PID 5740 = Virtual COM Port (firmware is running)")
            elif p.pid == 0xDF11:
                print("  PID DF11 = DFU Bootloader mode (ready for programming!)")

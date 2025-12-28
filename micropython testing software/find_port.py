"""
Find available COM ports
"""
import serial.tools.list_ports

print("Available COM ports:")
print("=" * 70)

ports = serial.tools.list_ports.comports()
if not ports:
    print("  No COM ports found!")
else:
    for port in ports:
        print(f"\n  Port: {port.device}")
        print(f"  Description: {port.description}")
        print(f"  Hardware ID: {port.hwid}")
        if port.manufacturer:
            print(f"  Manufacturer: {port.manufacturer}")
        if port.product:
            print(f"  Product: {port.product}")
        if port.serial_number:
            print(f"  Serial: {port.serial_number}")

print("\n" + "=" * 70)

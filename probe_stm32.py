import serial
import time

def hex_dump(data):
    """Display data in hex and ASCII"""
    if not data:
        return "(empty)"
    hex_str = ' '.join(f'{b:02x}' for b in data)
    ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data)
    return f"HEX: {hex_str}\nASCII: {ascii_str}"

try:
    print("=== STM32F405 Firmware Probe ===")
    print("Device: COM8 @ 115200 baud")
    print("VID:PID = 0483:5740 (ST Microelectronics)")
    print("\nAttempting to identify firmware...\n")
    
    s = serial.Serial('COM8', 115200, timeout=1, rtscts=False, dsrdtr=False)
    time.sleep(0.5)
    
    # Clear any pending data
    s.reset_input_buffer()
    s.reset_output_buffer()
    
    # Listen for spontaneous output
    print("1. Listening for spontaneous output (5 seconds)...")
    time.sleep(5)
    if s.in_waiting:
        data = s.read(s.in_waiting)
        print(f"   Received {len(data)} bytes:")
        print(f"   {hex_dump(data)}\n")
    else:
        print("   No spontaneous output\n")
    
    # Try resetting with DTR/RTS
    print("2. Toggling DTR/RTS to reset board...")
    s.setDTR(False)
    s.setRTS(False)
    time.sleep(0.1)
    s.setDTR(True)
    s.setRTS(True)
    time.sleep(2)
    if s.in_waiting:
        data = s.read(s.in_waiting)
        print(f"   Received {len(data)} bytes after reset:")
        print(f"   {hex_dump(data)}\n")
    else:
        print("   No response after reset\n")
    
    # Try sending single byte probes
    test_bytes = [
        (b'\r\n', "CRLF"),
        (b'\x00', "NULL byte"),
        (b'\x03', "Ctrl+C (ETX)"),
        (b'\x04', "Ctrl+D (EOT)"),
        (b'\x1b', "ESC"),
        (b'AT\r\n', "AT command"),
    ]
    
    print("3. Sending probe commands...")
    for cmd, desc in test_bytes:
        print(f"   Sending {desc}...")
        s.write(cmd)
        time.sleep(0.3)
        if s.in_waiting:
            data = s.read(s.in_waiting)
            print(f"   Response: {hex_dump(data)}")
        else:
            print(f"   (no response)")
    
    s.close()
    print("\n=== Probe Complete ===")
    print("\nPossible next steps:")
    print("- The board may need to be put into a specific mode (button press, jumper)")
    print("- It might be running custom firmware with a specific protocol")
    print("- Try connecting with PuTTY or another terminal to see if there's any interaction")
    
except Exception as e:
    print(f'Error: {e}')
    import traceback
    traceback.print_exc()

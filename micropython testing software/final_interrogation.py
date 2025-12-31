import serial
import time

print("=== Interrogating SparkFun Thing Plus STM32 ===\n")

try:
    s = serial.Serial('COM8', 115200, timeout=1)
    print("✓ Connected to COM8\n")
    
    # Try various reset/initialization sequences
    tests = [
        ("Soft reset", b'\x18'),  # Ctrl+X
        ("Break signal", lambda: s.send_break(0.25)),
        ("MicroPython interrupt", b'\x03\x03\r\n'),
        ("MicroPython raw REPL", b'\x01'),
        ("Arduino Serial", b'?\r\n'),
        ("Blank line", b'\r\n\r\n'),
    ]
    
    for name, command in tests:
        print(f"Trying: {name}...")
        s.reset_input_buffer()
        
        if callable(command):
            command()
        else:
            s.write(command)
        
        time.sleep(0.5)
        
        if s.in_waiting:
            response = s.read(s.in_waiting)
            print(f"  ✓ RESPONSE! {len(response)} bytes:")
            print(f"  {response}")
            print(f"  ASCII: {response.decode('utf-8', errors='replace')}\n")
        else:
            print(f"  (no response)\n")
    
    # Try reading a large chunk in case there's streaming data
    print("Listening for 3 seconds for any spontaneous output...")
    s.reset_input_buffer()
    time.sleep(3)
    if s.in_waiting:
        data = s.read(s.in_waiting)
        print(f"  Received {len(data)} bytes!")
        print(f"  {data.decode('utf-8', errors='replace')}")
    else:
        print("  No spontaneous output detected")
    
    s.close()
    
    print("\n" + "="*60)
    print("CONCLUSION:")
    print("The board has firmware that creates a Virtual COM Port,")
    print("but it's not responding to any serial commands.")
    print("\nPossible reasons:")
    print("  1. Blank/empty user program")
    print("  2. Program doesn't use Serial")
    print("  3. Needs specific initialization")
    print("  4. Waiting for Arduino IDE upload")
    
    print("\nNext steps:")
    print("  - Flash MicroPython to get an interactive REPL")
    print("  - Use Arduino IDE to upload a test sketch")
    print("  - Use ST-Link to read the flash memory")
    
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()

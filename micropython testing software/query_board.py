import serial
import time

try:
    print("Connecting to STM32F405 on COM8 at 115200 baud...")
    s = serial.Serial('COM8', 115200, timeout=2)
    time.sleep(1)
    
    # Listen for any automatic output
    print("\n=== Listening for automatic output (3 seconds) ===")
    time.sleep(3)
    if s.in_waiting:
        response = s.read(s.in_waiting)
        print(response.decode('utf-8', errors='ignore'))
    else:
        print("(No automatic output)")
    
    # Try various common commands/sequences
    tests = [
        (b'\r\n', "Sending newline"),
        (b'help\r\n', "Sending 'help'"),
        (b'?\r\n', "Sending '?'"),
        (b'\x03', "Sending Ctrl+C (interrupt)"),
        (b'version\r\n', "Sending 'version'"),
        (b'info\r\n', "Sending 'info'"),
        (b'\r\n', "Sending another newline"),
    ]
    
    for cmd, description in tests:
        print(f"\n{description}...")
        s.write(cmd)
        time.sleep(0.5)
        if s.in_waiting:
            response = s.read(s.in_waiting)
            print("Response:", response.decode('utf-8', errors='ignore'))
        else:
            print("(No response)")
    
    # Try to see if it's echoing back
    print("\n=== Testing for echo ===")
    s.write(b'TEST123\r\n')
    time.sleep(0.5)
    if s.in_waiting:
        response = s.read(s.in_waiting)
        print("Response:", response.decode('utf-8', errors='ignore'))
    
    s.close()
    print("\nConnection closed.")
    
except Exception as e:
    print(f'Error: {e}')
    import traceback
    traceback.print_exc()

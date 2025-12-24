import serial
import time

def test_baud_rate(baud):
    """Test a specific baud rate"""
    try:
        s = serial.Serial('COM8', baud, timeout=0.5)
        time.sleep(0.2)
        
        # Clear buffers
        s.reset_input_buffer()
        
        # Try simple newline
        s.write(b'\r\n')
        time.sleep(0.2)
        
        if s.in_waiting:
            data = s.read(s.in_waiting)
            s.close()
            return data
        
        # Try help command
        s.write(b'help\r\n')
        time.sleep(0.2)
        
        if s.in_waiting:
            data = s.read(s.in_waiting)
            s.close()
            return data
            
        s.close()
        return None
    except Exception as e:
        return None

print("=== SparkFun Thing Plus STM32 Probe ===\n")

# Common baud rates for STM32 boards
baud_rates = [115200, 9600, 19200, 38400, 57600, 76800, 230400, 460800]

print("Testing different baud rates...")
for baud in baud_rates:
    print(f"  {baud}...", end=" ")
    result = test_baud_rate(baud)
    if result:
        print(f"RESPONSE! ({len(result)} bytes)")
        print(f"    {result.decode('utf-8', errors='ignore')}")
    else:
        print("no response")

print("\n\nTrying continuous monitoring at 115200 for any output...")
print("(Press Ctrl+C to stop, or press reset button on board now)\n")

try:
    s = serial.Serial('COM8', 115200, timeout=0.1)
    start_time = time.time()
    
    while time.time() - start_time < 10:  # Monitor for 10 seconds
        if s.in_waiting:
            data = s.read(s.in_waiting)
            print(f"[{time.time() - start_time:.1f}s] Received: {data.decode('utf-8', errors='ignore')}")
        time.sleep(0.1)
    
    s.close()
    print("\nMonitoring complete.")
    
except KeyboardInterrupt:
    print("\nStopped by user")
except Exception as e:
    print(f"\nError: {e}")

import serial
import sys
import threading
import msvcrt
import time

PORT = 'COM3'
BAUD = 115200

print(f"=== PyBoard Terminal ({PORT} @ {BAUD}) ===")
print("Press Ctrl+V to send Viper test")
print("Press Ctrl+C to exit\n")

VIPER_TEST = b'''
@micropython.viper
def test(n: int) -> int:
    t: int = 0
    for i in range(n):
        t += i
    return t

print("Viper result:", test(1000))
'''

try:
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
    time.sleep(0.1)
    ser.write(b'\x03')  # Ctrl-C to get prompt
    print("Connected!\n")
    
    def read_serial():
        while True:
            if ser.in_waiting:
                try:
                    data = ser.read(ser.in_waiting)
                    print(data.decode('utf-8', errors='ignore'), end='', flush=True)
                except:
                    pass
            time.sleep(0.01)
    
    reader_thread = threading.Thread(target=read_serial, daemon=True)
    reader_thread.start()
    
    while True:
        if msvcrt.kbhit():
            char = msvcrt.getwch()
            if ord(char) == 3:  # Ctrl+C
                break
            elif ord(char) == 22:  # Ctrl+V - send viper test
                print("\n>>> Sending Viper test...")
                ser.write(b'\x05')  # Ctrl-E paste mode
                time.sleep(0.1)
                ser.write(VIPER_TEST)
                time.sleep(0.1)
                ser.write(b'\x04')  # Ctrl-D execute
            elif char == '\r':
                ser.write(b'\r\n')
                print()
            else:
                ser.write(char.encode('utf-8'))
                print(char, end='', flush=True)

except KeyboardInterrupt:
    print("\n\nExiting...")
except Exception as e:
    print(f"\nError: {e}")
finally:
    if 'ser' in locals():
        ser.close()
    print("Closed.")

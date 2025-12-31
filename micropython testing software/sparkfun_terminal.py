import serial
import sys
import threading
import msvcrt

print("=== SparkFun Thing Plus STM32 Interactive Terminal ===")
print("Board: STM32F405RG")
print("Port: COM8 @ 115200 baud")
print("\nInstructions:")
print("1. If nothing appears, the board may need Arduino/MicroPython firmware")
print("2. To enter DFU bootloader mode:")
print("   - Hold BOOT button")
print("   - Press and release RESET button")
print("   - Release BOOT button")
print("\n3. Press Ctrl+C to exit this terminal")
print("\n" + "="*60 + "\n")

try:
    ser = serial.Serial('COM8', 115200, timeout=0.1)
    print("Connected! Listening for output...\n")
    
    def read_serial():
        """Continuously read from serial port"""
        while True:
            if ser.in_waiting:
                try:
                    data = ser.read(ser.in_waiting)
                    print(data.decode('utf-8', errors='ignore'), end='', flush=True)
                except:
                    pass
    
    # Start reading thread
    reader_thread = threading.Thread(target=read_serial, daemon=True)
    reader_thread.start()
    
    # Main loop - send keyboard input to serial
    print("Type anything and press Enter to send to board...\n")
    
    while True:
        if msvcrt.kbhit():
            char = msvcrt.getwch()
            if char == '\r':  # Enter key
                ser.write(b'\r\n')
                print()  # New line on console
            elif ord(char) == 3:  # Ctrl+C
                break
            else:
                ser.write(char.encode('utf-8'))
                print(char, end='', flush=True)
    
except KeyboardInterrupt:
    print("\n\nTerminal closed by user.")
except Exception as e:
    print(f"\nError: {e}")
finally:
    if 'ser' in locals():
        ser.close()
    print("\nConnection closed.")

import serial
import time

s = serial.Serial('COM3', 115200, timeout=2)
time.sleep(0.3)
s.write(b'\x03')
time.sleep(0.2)
s.read(s.in_waiting)

print('=== Viper Test on MicroPython 1.27.1 ===')
print()

# Test 1: Simple bytecode (should work)
s.write(b'print(2+2)\r\n')
time.sleep(0.3)
for _ in range(10):
    if s.in_waiting:
        print('Bytecode 2+2:', s.read(s.in_waiting).decode())
        break
    time.sleep(0.1)

# Test 2: Native decorator
s.write(b'@micropython.native\r\n')
time.sleep(0.2)
s.write(b'def nf(): return 42\r\n')
time.sleep(0.2)
s.write(b'\r\n')
time.sleep(0.3)
resp = s.read(s.in_waiting).decode()
print('Define native:', resp)

s.write(b'print(nf())\r\n')
time.sleep(1)
resp = s.read(s.in_waiting).decode()
print('Native result:', resp)

s.close()

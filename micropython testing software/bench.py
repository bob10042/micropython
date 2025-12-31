import serial
import time

s = serial.Serial('COM3', 115200, timeout=5)
time.sleep(0.3)
s.write(b'\x03\x03')
time.sleep(0.3)
s.read(s.in_waiting)

s.write(b'\x05')  # paste mode
time.sleep(0.2)

code = """
import pyb

def bc():
    t = 0
    for i in range(50000): t += i

t1 = pyb.millis()
for _ in range(5): bc()
t2 = pyb.millis()
print("Bytecode:", t2-t1, "ms")
"""

s.write(code.encode())
s.write(b'\x04')  # execute
time.sleep(5)
print(s.read(s.in_waiting).decode())
s.close()

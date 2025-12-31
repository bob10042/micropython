import serial
import time

s = serial.Serial('COM3', 115200, timeout=5)
time.sleep(0.3)
s.write(b'\x03\x03')
time.sleep(0.3)
s.read(s.in_waiting)

print('=== Speed Benchmark: Bytecode vs Native vs Viper ===')
print()

code = """
def bytecode_sum():
    t = 0
    for i in range(10000):
        t += i
    return t

@micropython.native
def native_sum():
    t = 0
    for i in range(10000):
        t += i
    return t

@micropython.viper
def viper_sum() -> int:
    t:int = 0
    for i in range(10000):
        t += i
    return t

import time
t0 = time.ticks_us()
r1 = bytecode_sum()
t1 = time.ticks_us()
r2 = native_sum()
t2 = time.ticks_us()
r3 = viper_sum()
t3 = time.ticks_us()

print("Bytecode:", time.ticks_diff(t1, t0), "us")
print("Native:", time.ticks_diff(t2, t1), "us")
print("Viper:", time.ticks_diff(t3, t2), "us")
"""

# Use paste mode
s.write(b'\x05')  # Ctrl-E
time.sleep(0.2)
s.write(code.encode())
time.sleep(0.2)
s.write(b'\x04')  # Ctrl-D
time.sleep(3)
print(s.read(s.in_waiting).decode())

s.close()

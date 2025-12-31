# Viper/Native Test for MicroPython 1.28
# Run this on the PyBoard

# Test 1: Basic Viper function
@micropython.viper
def viper_add(a: int, b: int) -> int:
    return a + b

print("Test 1 - Viper add:", viper_add(100, 23))

# Test 2: Viper with loop
@micropython.viper
def viper_sum(n: int) -> int:
    total: int = 0
    for i in range(n):
        total += i
    return total

print("Test 2 - Viper sum(1000):", viper_sum(1000))

# Test 3: Native function
@micropython.native
def native_add(a, b):
    return a + b

print("Test 3 - Native add:", native_add(100, 23))

# Test 4: Benchmark comparison
import time

def bytecode_bench():
    total = 0
    for i in range(10000):
        total += i
    return total

@micropython.native
def native_bench():
    total = 0
    for i in range(10000):
        total += i
    return total

@micropython.viper
def viper_bench() -> int:
    total: int = 0
    for i in range(10000):
        total += i
    return total

# Bytecode
t0 = time.ticks_us()
r1 = bytecode_bench()
t1 = time.ticks_us()
bc_time = time.ticks_diff(t1, t0)

# Native  
t0 = time.ticks_us()
r2 = native_bench()
t2 = time.ticks_us()
native_time = time.ticks_diff(t2, t0)

# Viper
t0 = time.ticks_us()
r3 = viper_bench()
t3 = time.ticks_us()
viper_time = time.ticks_diff(t3, t0)

print("\n=== Benchmark Results ===")
print(f"Bytecode: {bc_time} us")
print(f"Native:   {native_time} us  ({bc_time/native_time:.1f}x faster)")
print(f"Viper:    {viper_time} us  ({bc_time/viper_time:.1f}x faster)")
print("\nAll tests passed!" if r1 == r2 == r3 else "ERROR: Results mismatch!")

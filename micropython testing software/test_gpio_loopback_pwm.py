"""
GPIO Loopback and PWM Test for PyBoard
Connect X1 to X2 with a jumper wire

Tests:
1. Digital GPIO loopback (X1 output -> X2 input)
2. PWM output on X1, read frequency/duty on X2
"""

import serial
import time

def run_test(port='COM3'):
    ser = serial.Serial(port, 115200, timeout=0.1)
    time.sleep(0.5)
    ser.write(b'\x03')
    time.sleep(0.3)
    ser.reset_input_buffer()

    print('=' * 50)
    print('GPIO LOOPBACK & PWM TEST')
    print('Connect X1 to X2 with jumper wire!')
    print('=' * 50)

    ser.write(b'\x01')
    time.sleep(0.2)
    ser.reset_input_buffer()

    code = '''import pyb, time
from pyb import Pin, Timer

# Test 1: Digital GPIO Loopback
print('TEST1: GPIO Loopback')
out_pin = Pin('X1', Pin.OUT_PP)
in_pin = Pin('X2', Pin.IN, Pin.PULL_DOWN)

errors = 0
for i in range(100):
    # Test HIGH
    out_pin.high()
    time.sleep_us(10)
    if in_pin.value() != 1:
        errors += 1
    # Test LOW
    out_pin.low()
    time.sleep_us(10)
    if in_pin.value() != 0:
        errors += 1

if errors == 0:
    print('PASS: 200 transitions OK')
else:
    print('FAIL:', errors, 'errors')

# Test 2: Toggle speed test
print('TEST2: Toggle Speed')
out_pin = Pin('X1', Pin.OUT_PP)
start = time.ticks_us()
for _ in range(10000):
    out_pin.high()
    out_pin.low()
elapsed = time.ticks_diff(time.ticks_us(), start)
freq = 10000 * 1000000 // elapsed
print('Freq:', freq, 'Hz')

# Test 3: PWM Output
print('TEST3: PWM on X1')
# X1 = PA0 = TIM2_CH1 or TIM5_CH1
tim = Timer(2, freq=1000)
ch = tim.channel(1, Timer.PWM, pin=Pin('X1'))

# Test different duty cycles
for duty in [25, 50, 75, 100]:
    ch.pulse_width_percent(duty)
    time.sleep_ms(500)
    print('Duty:', duty, '%')

# Test 4: PWM frequency sweep
print('TEST4: PWM Freq Sweep')
for freq in [100, 1000, 10000, 50000]:
    tim = Timer(2, freq=freq)
    ch = tim.channel(1, Timer.PWM, pin=Pin('X1'))
    ch.pulse_width_percent(50)
    time.sleep_ms(300)
    print('PWM:', freq, 'Hz')

# Cleanup
tim.deinit()
Pin('X1', Pin.IN)
print('DONE')
'''

    ser.write(code.encode())
    ser.write(b'\x04')
    time.sleep(0.3)

    start_time = time.time()
    while time.time() - start_time < 20:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line and not line.startswith('>'):
                print(line)
            if 'DONE' in line:
                break

    ser.write(b'\x02')
    ser.close()
    print('=' * 50)

if __name__ == '__main__':
    run_test()

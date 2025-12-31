import serial
import time

s = serial.Serial('COM3', 115200, timeout=2)
time.sleep(0.3)
s.read(s.in_waiting)

print('=== DAC to ADC Loopback Test ===')
print('X5 (DAC) -> X1 (ADC)')
print()

# Setup DAC and ADC
s.write(b'from pyb import DAC, ADC, Pin\r\n')
time.sleep(0.2)
s.write(b'dac = DAC(1)\r\n')
time.sleep(0.2)
s.write(b'adc = ADC(Pin.board.X1)\r\n')
time.sleep(0.2)
s.read(s.in_waiting)

print('DAC Value -> ADC Reading:')
for dac_val in [0, 50, 100, 150, 200, 255]:
    cmd = 'dac.write(' + str(dac_val) + '); print(adc.read())\r\n'
    s.write(cmd.encode())
    time.sleep(0.15)
    resp = s.read(s.in_waiting).decode()
    for line in resp.split():
        if line.isdigit():
            adc_val = int(line)
            dac_v = dac_val * 3.3 / 255
            adc_v = adc_val * 3.3 / 4095
            print(f'  DAC={dac_val:3d} ({dac_v:.2f}V) -> ADC={adc_val:4d} ({adc_v:.2f}V)')
            break

s.close()
print()
print('Test complete!')

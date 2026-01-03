"""
Real-time Browser Oscilloscope for PyBoard
Reads ADC from X1 and displays in browser via WebSocket

Usage:
  1. Connect PyBoard to COM3
  2. Run: python server.py
  3. Open browser to http://localhost:8080
"""

import asyncio
import websockets
import json
import serial
import threading
import time
from http.server import HTTPServer, SimpleHTTPRequestHandler
import os

# Configuration
SERIAL_PORT = 'COM3'
BAUD_RATE = 115200
WS_PORT = 8765
HTTP_PORT = 8080

# Shared data
adc_buffer = []
buffer_lock = threading.Lock()
running = True
ser_instance = None
current_freq = 500
current_duty = 50

def serial_reader():
    """Read ADC values from PyBoard continuously"""
    global running, adc_buffer, ser_instance, current_freq, current_duty
    
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
        ser_instance = ser
        time.sleep(0.5)
        ser.write(b'\x03')
        time.sleep(0.3)
        ser.reset_input_buffer()

        # Start PWM sweep on X2 and read ADC on X1
        ser.write(b'\x01')
        time.sleep(0.2)
        ser.reset_input_buffer()
        
        code = '''import pyb, time
from pyb import Pin, Timer, ADC

adc = ADC('X1')
freqs = [100, 250, 500, 1000, 2000, 5000]
duties = [10, 25, 50, 75, 90]

# Cycle through frequencies and duties
idx = 0
while True:
    freq = freqs[idx % len(freqs)]
    duty = duties[(idx // len(freqs)) % len(duties)]
    
    tim = Timer(2, freq=freq)
    ch = tim.channel(2, Timer.PWM, pin=Pin('X2'))
    ch.pulse_width_percent(duty)
    
    print('CFG', freq, duty)
    
    # Sample for 2 seconds at this setting
    for _ in range(400):
        print(adc.read())
        time.sleep_us(200)
    
    idx += 1
'''
        ser.write(code.encode())
        ser.write(b'\x04')
        
        print(f"[Serial] Connected to {SERIAL_PORT}")
        
        while running:
            if ser.in_waiting:
                try:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if line.startswith('CFG'):
                        parts = line.split()
                        if len(parts) == 3:
                            current_freq = int(parts[1])
                            current_duty = int(parts[2])
                            print(f"[PWM] Freq: {current_freq} Hz, Duty: {current_duty}%")
                    elif line.isdigit():
                            adc_buffer.append(val)
                            # Keep last 2000 samples
                            if len(adc_buffer) > 2000:
                                adc_buffer = adc_buffer[-2000:]
                except:
                    pass
        
        ser.write(b'\x03')  # Stop
        ser.close()
        
    except Exception as e:
        print(f"[Serial] Error: {e}")

async def websocket_handler(websocket):
    """Send ADC data to browser clients and receive commands"""
    global adc_buffer, ser_instance, current_freq, current_duty
    print(f"[WS] Client connected")
    
    try:
        while running:
            # Check for incoming commands
            try:
                message = await asyncio.wait_for(websocket.recv(), timeout=0.01)
                cmd = json.loads(message)
                if 'freq' in cmd and ser_instance:
                    ser_instance.write(f"F{cmd['freq']}\r\n".encode())
                    print(f"[CMD] Freq: {cmd['freq']} Hz")
                if 'duty' in cmd and ser_instance:
                    ser_instance.write(f"D{cmd['duty']}\r\n".encode())
                    print(f"[CMD] Duty: {cmd['duty']}%")
            except asyncio.TimeoutError:
                pass
            
            with buffer_lock:
                if adc_buffer:
                    # Send last 500 samples
                    data = adc_buffer[-500:]
                else:
                    data = []
            
            await websocket.send(json.dumps({
                'samples': data,
                'timestamp': time.time(),
                'freq': current_freq,
                'duty': current_duty
            }))
            await asyncio.sleep(0.05)  # 20 FPS
            
    except websockets.exceptions.ConnectionClosed:
        print("[WS] Client disconnected")

def run_http_server():
    """Serve the HTML frontend"""
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    handler = SimpleHTTPRequestHandler
    httpd = HTTPServer(('', HTTP_PORT), handler)
    print(f"[HTTP] Server at http://localhost:{HTTP_PORT}")
    httpd.serve_forever()

async def main():
    global running
    
    # Start serial reader thread
    serial_thread = threading.Thread(target=serial_reader, daemon=True)
    serial_thread.start()
    
    # Start HTTP server thread
    http_thread = threading.Thread(target=run_http_server, daemon=True)
    http_thread.start()
    
    # Start WebSocket server
    print(f"[WS] Server at ws://localhost:{WS_PORT}")
    async with websockets.serve(websocket_handler, "localhost", WS_PORT):
        print("\n" + "="*50)
        print("OSCILLOSCOPE RUNNING")
        print(f"Open browser: http://localhost:{HTTP_PORT}")
        print("Press Ctrl+C to stop")
        print("="*50 + "\n")
        
        try:
            await asyncio.Future()  # Run forever
        except asyncio.CancelledError:
            pass
    
    running = False

if __name__ == '__main__':
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nShutting down...")
        running = False

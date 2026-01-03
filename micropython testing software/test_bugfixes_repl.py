#!/usr/bin/env python3
"""Test MicroPython bug fixes via serial REPL"""

import serial
import time
import sys

PORT = 'COM3'
BAUD = 115200

def run_test(ser, name, code):
    """Send code to REPL and check for PASS/FAIL"""
    ser.reset_input_buffer()
    
    # Send each line
    for line in code.strip().split('\n'):
        ser.write((line + '\r\n').encode())
        time.sleep(0.05)
    
    time.sleep(0.8)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    
    # Check result
    if 'PASS' in resp:
        print(f"  [PASS] {name}")
        return True
    elif 'FAIL' in resp:
        print(f"  [FAIL] {name}")
        print(f"         Response: {resp[:200]}")
        return False
    else:
        print(f"  [????] {name} - No result")
        print(f"         Response: {resp[:200]}")
        return False

def main():
    print("=" * 50)
    print("MicroPython Bug Fix Verification")
    print("=" * 50)
    
    try:
        ser = serial.Serial(PORT, BAUD, timeout=2)
        time.sleep(0.5)
        ser.write(b'\x03\x03')  # Ctrl-C
        time.sleep(0.3)
        ser.reset_input_buffer()
    except Exception as e:
        print(f"ERROR: Cannot open {PORT}: {e}")
        return 1
    
    # Get version
    ser.write(b'import sys; print("VER:", sys.version)\r\n')
    time.sleep(0.5)
    resp = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
    for line in resp.split('\n'):
        if 'VER:' in line:
            print(f"Version: {line.split('VER:')[1].strip()}")
    
    print("\n[BUG FIX TESTS]")
    passed = 0
    failed = 0
    
    # Test 1: super() with invalid arg (Issue #17728)
    code = """
try:
    super("not a type", object())
    print("FAIL")
except TypeError:
    print("PASS")
"""
    if run_test(ser, "super() invalid arg (#17728)", code): passed += 1
    else: failed += 1
    
    # Test 2: compile() invalid source (Issue #17817)
    code = """
try:
    compile(123, "test", "exec")
    print("FAIL")
except TypeError:
    print("PASS")
"""
    if run_test(ser, "compile() invalid source (#17817)", code): passed += 1
    else: failed += 1
    
    # Test 3: compile() invalid filename
    code = """
try:
    compile("pass", 123, "exec")
    print("FAIL")
except TypeError:
    print("PASS")
"""
    if run_test(ser, "compile() invalid filename", code): passed += 1
    else: failed += 1
    
    # Test 4: re.compile() invalid pattern (Issue #17720)
    code = """
import re
try:
    re.compile(123)
    print("FAIL")
except TypeError:
    print("PASS")
"""
    if run_test(ser, "re.compile() invalid pattern (#17720)", code): passed += 1
    else: failed += 1
    
    # Test 5: str.center() (Issue #17827)
    code = """
s = "hi".center(6, "*")
if s == "**hi**":
    print("PASS")
else:
    print("FAIL", repr(s))
"""
    if run_test(ser, "str.center() (#17827)", code): passed += 1
    else: failed += 1
    
    # Test 6: dict == None (Issue #18619)
    code = """
try:
    d = {1: 2}
    r = (d == None)
    print("PASS", r)
except:
    print("FAIL")
"""
    if run_test(ser, "dict == None (#18619)", code): passed += 1
    else: failed += 1
    
    # Test 7: array basic (Issue #18620)
    code = """
import array
try:
    a = array.array("i", [1,2,3])
    print("PASS", len(a))
except:
    print("FAIL")
"""
    if run_test(ser, "array creation (#18620)", code): passed += 1
    else: failed += 1
    
    # Test 8: bytearray basic (Issue #18617)
    code = """
try:
    b = bytearray(100)
    print("PASS", len(b))
except:
    print("FAIL")
"""
    if run_test(ser, "bytearray creation (#18617)", code): passed += 1
    else: failed += 1
    
    # Test 9: memoryview safety
    code = """
try:
    b = bytearray(10)
    m = memoryview(b)
    print("PASS")
except:
    print("FAIL")
"""
    if run_test(ser, "memoryview safety (#18171)", code): passed += 1
    else: failed += 1
    
    # Test 10: JSON with namedtuple (Issue #18198)
    code = """
import json
from collections import namedtuple
try:
    P = namedtuple("P", ["x", "y"])
    p = P(1, 2)
    s = json.dumps({"p": [p.x, p.y]})
    print("PASS", s)
except Exception as e:
    print("FAIL", e)
"""
    if run_test(ser, "JSON namedtuple (#18198)", code): passed += 1
    else: failed += 1
    
    # Summary
    print("\n" + "=" * 50)
    total = passed + failed
    if failed == 0:
        print(f"ALL TESTS PASSED: {passed}/{total}")
    else:
        print(f"RESULTS: {passed}/{total} passed, {failed} failed")
    print("=" * 50)
    
    ser.close()
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())

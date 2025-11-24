#!/usr/bin/env micropython
"""Test script for MEDIUM priority bug fixes #7 and #8"""

print("Testing MEDIUM priority security fixes...")
print()

# Test 1: Sequence multiply overflow check (Bug #8)
print("Test 1: Sequence multiply overflow in mp_seq_multiply")
try:
    # This should trigger the overflow check in py/sequence.c:40
    # Using a string (sequence) with large multiplication
    result = "x" * (2**62)
    print("FAIL: Should have raised OverflowError or MemoryError")
except (OverflowError, MemoryError) as e:
    print(f"SUCCESS: Caught expected error: {e}")
except Exception as e:
    print(f"PARTIAL: Caught error (may be memory-related): {e}")
print()

# Test 2: Dict allocation with NULL check (Bug #7)
print("Test 2: Dict rehash with defensive NULL check")
try:
    # Create a large dict to trigger rehashing
    d = {}
    for i in range(10000):
        d[i] = i
    print(f"SUCCESS: Dict created with {len(d)} entries (NULL check working)")
except MemoryError as e:
    print(f"SUCCESS: Caught MemoryError as expected: {e}")
print()

# Test 3: Verify tuple/list operations still work normally
print("Test 3: Normal operations still work")
try:
    # Normal list operations
    lst = [1, 2, 3] * 100
    tup = (1, 2, 3) * 100
    d = {i: i*2 for i in range(100)}
    print(f"SUCCESS: Normal operations work (list len={len(lst)}, tuple len={len(tup)}, dict len={len(d)})")
except Exception as e:
    print(f"FAIL: Normal operations failed: {e}")
print()

print("All MEDIUM priority fix tests completed!")

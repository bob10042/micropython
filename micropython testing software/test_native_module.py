# Test module for native compilation

def sum_loop(n):
    """Standard bytecode function"""
    total = 0
    for i in range(n):
        total += i
    return total

def fibonacci(n):
    """Calculate fibonacci - good test of function calls"""
    if n <= 1:
        return n
    a, b = 0, 1
    for _ in range(n - 1):
        a, b = b, a + b
    return b

def array_sum(arr):
    """Sum array elements"""
    total = 0
    for x in arr:
        total += x
    return total

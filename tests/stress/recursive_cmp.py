# This tests that comparing (and hashing) deeply nested data raises RuntimeError
# when MicroPython runs out of stack, instead of overflowing the C stack (AUD-019).
#
# Like recursive_iternext.py, this would crash CPython, which does not enforce a
# limit on C recursion here, so it has a .exp file.


# Progressively build bigger nested structures (100 levels at a time) and apply
# op to two equal copies, until it raises RuntimeError.  The structures are built
# without recursion, so only the comparison itself can run out of stack.
def recurse(nest, op):
    a = b = 1
    while True:
        for _ in range(100):
            a = nest(a)
            b = nest(b)
        try:
            op(a, b)
        except RuntimeError:
            print("RuntimeError")
            break
        except MemoryError:
            print("MemoryError")
            break


recurse(lambda x: [x], lambda a, b: a == b)
recurse(lambda x: [x], lambda a, b: a != b)
recurse(lambda x: [0, x], lambda a, b: a < b)
recurse(lambda x: (x,), lambda a, b: a == b)
recurse(lambda x: {1: x}, lambda a, b: a == b)
recurse(lambda x: (x,), lambda a, b: hash(a))

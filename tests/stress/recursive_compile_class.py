# This tests that the compiler, and classes with multiple bases, check the C stack
# when they recurse, so that deeply nested source or a deep chain of classes raise
# RuntimeError when MicroPython runs out of stack (AUD-031).
#
# Each operation is run from a Python call depth just short of the recursion
# limit, so it fails whether or not the heap or the real stack would run out.

try:
    compile
except NameError:
    print("SKIP")
    raise SystemExit


def at_depth(n, f):
    if n:
        return at_depth(n - 1, f)
    return f()


# find the deepest call depth at which a trivial function can still be called
n = 0
try:
    while True:
        at_depth(n + 1, lambda: None)
        n += 1
except RuntimeError:
    pass

src = "x = " + "[" * 300 + "]" * 300


def compile_nested():
    compile(src, "<nested>", "exec")
    return "compiled"


C = object
for i in range(200):
    C = type("C", (C, object), {})


tests = (
    compile_nested,
    lambda: C() and "instance",
    lambda: hasattr(C, "missing") or "lookup",
    lambda: issubclass(C, int) or "issubclass",
)

for f in tests:
    # works from the top level
    print(f())
    # raises RuntimeError near the recursion limit
    try:
        print(at_depth(n, f))
    except RuntimeError:
        print("RuntimeError")

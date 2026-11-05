# This tests that the compiler helpers that recurse without going through
# compile_node (conditions, assignment targets, with items, comprehension for
# clauses) check the C stack, so deeply nested source raises RuntimeError
# instead of overflowing the C stack (AUD-031).
#
# Each source is compiled from the top level.  Its nesting is deep enough to go
# past the stack check limit, but not so deep that an unchecked recursion would
# overflow the real stack of the unix port.

try:
    compile
except NameError:
    print("SKIP")
    raise SystemExit

# N is limited by the heap the parser needs; N_DEEP is for the cases that need
# little heap but have small stack frames on optimised builds.
N = 2000
N_DEEP = 10000

tests = (
    ("if_not", "if " + "not " * N_DEEP + "x: pass\n"),
    ("assign_list", "[" * N + "a" + "]" * N + " = 1\n"),
    ("with_items", "with " + ", ".join(["a"] * N_DEEP) + ": pass\n"),
    ("comp_for", "[x " + "for x in y " * N + "]\n"),
    ("async_with", "async def f():\n async with " + ", ".join(["a"] * N) + ": pass\n"),
)

for name, src in tests:
    try:
        compile(src, name, "exec")
        print(name, "compiled")
    except RuntimeError:
        print(name, "RuntimeError")

# shallow nesting still compiles
compile("if not not x: pass\n[[a]] = 1\nwith a, a: pass\n[x for x in y for x in y]\n", "s", "exec")
print("shallow ok")

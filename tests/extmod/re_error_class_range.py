# test that a character class ending in a range start is an error (AUD-034)

try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit


def test_re(r):
    try:
        re.compile(r)
        print("OK")
    except ValueError:
        print("Error")


# a class ending in "x-" with no closing bracket is unterminated
test_re("[a-")
test_re(r"[\]-")
test_re("[^a-")

# the terminating NUL must not be stepped over
test_re("[a-\x00]")
test_re("[ab\x00]")

# bytes stored after the pattern must not become part of it
r = "[a-" if 1 else "QQ]zz"
test_re(r)
r = "[0-" if 1 else "QQ]zz"
test_re(r)

# valid patterns using "-" still work
print(re.match("[a-]", "-").group(0))
print(re.match("[a-c]+", "abcd").group(0))
print(re.match(r"[\]-]+", "]-]").group(0))

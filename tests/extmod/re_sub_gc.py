# test re.sub with a function replacement whose result is only referenced by
# re.sub while it is copied, so a collection during the copy must keep it (AUD-037)

try:
    import re

    re.sub
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


def repl(m):
    # a new string, long enough that the result buffer grows while it is copied
    return m.group(0) * 20


s = " ".join(str(i) for i in range(50))
r = re.sub(r"\d+", repl, s)
print(len(r))
print(r == " ".join(str(i) * 20 for i in range(50)))
print(re.sub(r"\d+", lambda m: str(int(m.group(0)) * 2), "10 20 30 40 50"))

# test uctypes NATIVE float fields at addresses that are not aligned for their
# size: they must read and write the right value (AUD-033)

try:
    import uctypes

    float
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

buf = bytearray(32)
addr = uctypes.addressof(buf)

desc = {
    "f32": uctypes.FLOAT32 | 1,
    "f64": uctypes.FLOAT64 | 6,
    "arr": (uctypes.ARRAY | 15, uctypes.FLOAT64 | 1),
}
s = uctypes.struct(addr, desc, uctypes.NATIVE)
s.f32 = 1.5
s.f64 = -2.25
s.arr[0] = 0.125
print(s.f32, s.f64, s.arr[0])

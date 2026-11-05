# test uctypes NATIVE 64-bit and float fields at addresses that are not aligned
# for their size: they must read and write the right value (AUD-033)

try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

buf = bytearray(32)
addr = uctypes.addressof(buf)

desc = {
    "u64": uctypes.UINT64 | 1,
    "i64": uctypes.INT64 | 9,
    "arr": (uctypes.ARRAY | 17, uctypes.UINT64 | 1),
}
s = uctypes.struct(addr, desc, uctypes.NATIVE)
s.u64 = 0x0102030405060708
s.i64 = -2
s.arr[0] = 0x1112131415161718
print(hex(s.u64), s.i64, hex(s.arr[0]))
print(buf[1:9] == (0x0102030405060708).to_bytes(8, "little") or buf[1:9] == (0x0102030405060708).to_bytes(8, "big"))

# the same fields through a struct placed at an odd address
s2 = uctypes.struct(addr + 3, {"u64": uctypes.UINT64 | 0}, uctypes.NATIVE)
s2.u64 = 0x2122232425262728
print(hex(s2.u64))

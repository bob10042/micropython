# test FrameBuffer.poly with fill and many points: the per-row node list is
# sized by the number of points, so it must not be put on the C stack (AUD-007)

try:
    import framebuf
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

fb = framebuf.FrameBuffer(bytearray(8), 8, 8, framebuf.MONO_HLSB)

# 50000 points, all at (2, 3)
try:
    pts = array("h", bytes(4 * 50000))
except MemoryError:
    print("SKIP")
    raise SystemExit
for i in range(0, len(pts), 2):
    pts[i] = 2
    pts[i + 1] = 3
try:
    fb.poly(0, 0, pts, 1, True)
    print(fb.pixel(2, 3), fb.pixel(3, 3))
except MemoryError:
    print("MemoryError")

# test that the FrameBuffer buffer-size check does not overflow (AUD-021)

try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit

# (width, height, format, buffer size): each needs far more than the buffer
# has, but the 32-bit size sum wraps around to a value that would fit
tests = (
    (16385, 16385, framebuf.RGB565, 65538),
    (32768, 16384, framebuf.GS8, 16),
    (16384, 16384, framebuf.RGB565, 16),
)
for w, h, fmt, size in tests:
    try:
        framebuf.FrameBuffer(bytearray(size), w, h, fmt)
        print("accepted")
    except ValueError:
        print("ValueError")

# a buffer of exactly the required size is still accepted
fb = framebuf.FrameBuffer(bytearray(2 * 300 * 200), 300, 200, framebuf.RGB565)
fb.pixel(299, 199, 0xFFFF)
print(fb.pixel(299, 199))

# Test that VfsFat.open rejects a mode with more than one of r/w/x/a (or of
# b/t) instead of acting on the last letter: "rw" must not truncate the file.

try:
    import vfs

    vfs.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBDev:
    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)

    def readblocks(self, n, buf):
        buf[:] = self.data[n * self.SEC_SIZE : n * self.SEC_SIZE + len(buf)]

    def writeblocks(self, n, buf):
        self.data[n * self.SEC_SIZE : n * self.SEC_SIZE + len(buf)] = buf

    def ioctl(self, op, arg):
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMBDev(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

vfs.VfsFat.mkfs(bdev)
fs = vfs.VfsFat(bdev)

with fs.open("t.txt", "w") as f:
    f.write("keep")

# Invalid modes: ValueError, and the file is left as it is.
for mode in ("rw", "wr", "r+w", "ra", "wx", "rr", "rbt", "wbb"):
    try:
        fs.open("t.txt", mode).close()
        print(mode, "opened")
    except ValueError:
        print(mode, "ValueError")
    with fs.open("t.txt", "r") as f:
        print(mode, f.read())

# Valid modes still work.
for mode in ("r", "rb", "rt", "r+", "r+b", "br+", "a", "a+", "w+"):
    with fs.open("t.txt", mode) as f:
        print(mode, type(f).__name__)
with fs.open("t.txt", "r") as f:
    print(f.read())
with fs.open("new.txt", "x") as f:
    f.write("x")
with fs.open("new.txt", "r") as f:
    print(f.read())
